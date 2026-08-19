#include "collector.h"

// When collector object is created do an initial collection of the data so that get_monitor_data
// doesnt return null
// Set thread running to true, and start the collector thread
Collector::Collector() {
    current_data = std::make_shared<const StatisticsData>(collect_data());
    running_thread = true;
    // call std::thread on the run::thread method so that the thread activates and executes this
    // method
    collector_thread = std::thread(&Collector::run_thread, this);
}

// Stop the loop that causes the thread to continuously collect data when the Collector
// objecs destructor is called
Collector::~Collector() {
    running_thread = false;
    collector_thread.join();  // Tell the main thread to wait until the collector_thread has
                              // finished executing which it will because running_thread is set to
                              // false stopping the collector thread loop
}

void Collector::run_thread() {
    // Loop for continuous parsing and collection of data
    auto next_cycle = std::chrono::steady_clock::now();
    while (running_thread) {
        next_cycle += std::chrono::milliseconds(500);

        // Make the StatisticsData object and grab all the data from parsing again from this threads
        // iteration
        StatisticsData statistics;
        statistics = collect_data();

        // Allocate the StatisticsData object into memory and putting it into a shared pointer
        // uses std::move to call the move constructor so that StatisticsData object isnt copied
        // with initialized values
        auto data = std::make_shared<const StatisticsData>(std::move(statistics));
        {
            // Address of the StatisticsData object is changed when this is called which could be
            // unsafe if the main thread is reading from the previous memory location

            // Use mutex lock_guard so that the UI using the main thread cant read the
            // current data while the data is being reassigned by collector_thread

            // Locks this region until main thread is no longer trying to access current_data
            std::lock_guard<std::mutex> lock(data_mutex);
            // Swap with the new data
            current_data = std::move(data);
        }

        // Allow the main thread to run while this thread waits for 500ms to fully pass
        std::this_thread::sleep_until(next_cycle);
    }
}

// Return the shared pointer that holds the current data
std::shared_ptr<const StatisticsData> Collector::get_monitor_data() const {
    // Locks this region until main thread is no longer trying to access current_data then allow for
    // the return of the shared pointer
    std::lock_guard<std::mutex> lock(data_mutex);
    return current_data;
}

// Set the data for the cores
static CoreData build_core_data(const CoreCalculator& core) {
    CoreData core_data;
    core_data.usage_percent = core.get_cpu_usage_percent();
    core_data.user = core.get_user();
    core_data.nice = core.get_nice();
    core_data.system = core.get_system();
    core_data.idle = core.get_idle();
    core_data.iowait = core.get_io_wait();
    core_data.irq = core.get_irq();
    core_data.softirq = core.get_soft_irq();
    core_data.steal = core.get_steal();
    core_data.active = core.get_active();
    core_data.total = core.calc_total();
    return core_data;
}

// Collect ALL THE DATA by calling the parsers
StatisticsData Collector::collect_data() {
    // get the previous shared ptr holding the previous data
    auto previous = get_monitor_data();

    StatisticsData statistics;
    // Needed so that /s stats can be computed and so the system time can be used in the springboot
    // server
    statistics.captured_at = std::chrono::steady_clock::now();
    statistics.captured_system_time = std::chrono::system_clock::now();

    // Update and parse
    stats.update_cpu_usage();
    stats.compute_stats();
    mem_stats.compute_memory_statistics();
    dynamic_info.update_system_info();
    processes.update_pids();

    // Grab the data for every core and the total cpu
    statistics.total_cpu_core = build_core_data(stats.get_total_core());
    int core_count = stats.get_core_count();
    statistics.cores.reserve(core_count);
    for (int i = 0; i < core_count; i++) {
        statistics.cores.push_back(build_core_data(stats.get_individual_core(i)));
    }

    statistics.procs_running = stats.get_procs_running();
    statistics.procs_blocked = stats.get_procs_blocked();
    statistics.ctxt = stats.get_ctxt();
    statistics.interruptions = stats.get_interruptions();
    statistics.pre_ctxt = stats.get_pre_ctxt();
    statistics.pre_interruptions = stats.get_pre_interruptions();
    // If there was a previous run
    if (previous) {
        // get the time ellapsed since previous data collectons so long as the elapsed time
        auto elapsed = statistics.captured_at - previous->captured_at;
        // convert the elapsed time from std::chrono::steady_clock::duration to a double or
        // nanosecods to seconds std::chrono::duration<double> does the conversion while count
        // returns the extracted resulting double
        double elapsed_seconds = std::chrono::duration<double>(elapsed).count();
        // used elpased seconds to calculate the per second stats
        statistics.ctxt_per_second = (statistics.ctxt - statistics.pre_ctxt) / elapsed_seconds;
        statistics.interruptions_per_second =
            (statistics.interruptions - statistics.pre_interruptions) / elapsed_seconds;
    }

    statistics.mem_total = mem_stats.get_mem_total();
    statistics.mem_free = mem_stats.get_mem_free();
    statistics.mem_available = mem_stats.get_mem_available();
    statistics.cached = mem_stats.get_cached();
    statistics.buffers = mem_stats.get_buffers();
    statistics.swap_total = mem_stats.get_swap_total();
    statistics.swap_free = mem_stats.get_swap_free();
    statistics.active = mem_stats.get_active();
    statistics.inactive = mem_stats.get_inactive();
    statistics.active_anon = mem_stats.get_active_anon();
    statistics.inactive_anon = mem_stats.get_inactive_anon();
    statistics.active_file = mem_stats.get_active_file();
    statistics.inactive_file = mem_stats.get_inactive_file();
    statistics.slab = mem_stats.get_slab();
    statistics.sreclaimable = mem_stats.get_sreclaimable();
    statistics.sunreclaim = mem_stats.get_sunreclaim();
    statistics.dirty = mem_stats.get_dirty();
    statistics.writeback = mem_stats.get_writeback();
    statistics.anon_pages = mem_stats.get_anon_pages();
    statistics.mapped = mem_stats.get_mapped();
    statistics.shmem = mem_stats.get_shmem();

    statistics.kernel_info = static_info.get_kernel_info();
    statistics.os_info = static_info.get_os_info();
    statistics.last_minute_load = dynamic_info.get_last_minute_load();
    statistics.last_five_minutes_load = dynamic_info.get_last_five_minutes_load();
    statistics.last_fifteen_minutes_load = dynamic_info.get_last_fifteen_minutes_load();
    statistics.up_time = dynamic_info.get_uptime();
    statistics.idletime = dynamic_info.get_idletime();

    const std::unordered_map<pid_t, PID>& current_pids = processes.get_pids();
    statistics.pids.reserve(current_pids.size());

    for (const auto& pid_entry : current_pids) {
        const PID& curr_pid = pid_entry.second;
        ProcessData process_data;

        process_data.pid = curr_pid.get_pid();
        process_data.ppid = curr_pid.get_ppid();
        process_data.start_time = curr_pid.get_start_time();

        process_data.state = curr_pid.get_state();
        process_data.comm = curr_pid.get_comm();
        process_data.cmdline = curr_pid.get_cmdline();

        process_data.priority = curr_pid.get_priority();
        process_data.nice = curr_pid.get_nice();
        process_data.num_threads = curr_pid.get_num_threads();

        process_data.utime = curr_pid.get_utime();
        process_data.stime = curr_pid.get_stime();
        process_data.cpu_time_delta = curr_pid.get_cpu_time_delta();

        process_data.virtual_memory_bytes = curr_pid.get_virtual_memory_bytes();
        process_data.resident_memory_bytes = curr_pid.get_resident_memory_bytes();
        process_data.shared_memory_bytes = curr_pid.get_shared_memory_bytes();

        process_data.uid = curr_pid.get_uid();

        process_data.read_bytes = curr_pid.get_read_bytes();
        process_data.write_bytes = curr_pid.get_write_bytes();
        process_data.read_bytes_delta = curr_pid.get_read_bytes_delta();
        process_data.write_bytes_delta = curr_pid.get_write_bytes_delta();

        process_data.fd_count = curr_pid.get_fd_count();

        statistics.pids.push_back(std::move(process_data));
    }
    return statistics;
}