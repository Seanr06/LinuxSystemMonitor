#include "process.h"

#include <filesystem>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <system_error>
#include <unordered_set>

// initialize the fields for the PID
PID::PID(pid_t pid) {
    this->pid = pid;
    ppid = 0;
    start_time = 0;
    state = '?';
    std::string comm = "";
    std::string cmdline = "";
    priority = 0;
    nice = 0;
    num_threads = 0;
    utime = 0;
    stime = 0;
    prev_utime = 0;
    prev_stime = 0;
    virtual_memory_bytes = 0;
    resident_memory_bytes = 0;
    shared_memory_bytes = 0;
    uid = 0;
    prev_read_bytes = 0;
    prev_write_bytes = 0;
    read_bytes = 0;
    write_bytes = 0;
    fd_count = 0;
}
// Pase all the stats in /proc/[PID]/stats int its corresponding fields
void PID::parse_stat() {
    // Set prev for deltas
    prev_utime = utime;
    prev_stime = stime;

    std::ifstream file;
    file.open("/proc/" + std::to_string(pid) + "/stat");
    if (file.is_open()) {
        std::string line;
        std::getline(file, line);

        // Fine the open and closing parenthesis which surround comm in /stat
        std::size_t open_paren = line.find('(');
        std::size_t close_paren = line.rfind(')');

        // Only if they both exist
        if (open_paren != std::string::npos && close_paren != std::string::npos) {
            // Set the comm which is the process name and will be used as a fallback if cmdline is
            // empty
            comm = line.substr(open_paren + 1, close_paren - open_paren - 1);

            // Start the scanner after the closing ')'
            std::stringstream scanner(line.substr(close_paren + 1));

            int process_group_id;
            int session_id;
            int controlling_terminal_device;
            int foreground_process_group_id;
            int kernel_process_flags;
            long minor_page_faults;
            long children_minor_page_faults;
            long major_page_faults;
            long children_major_page_faults;
            long children_user_time_ticks;
            long children_system_time_ticks;
            long interval_timer_value_obsolete;
            unsigned long resident_memory_limit_bytes;
            long code_segment_start_address;
            long code_segment_end_address;
            long stack_start_address;
            unsigned long current_stack_pointer;
            unsigned long current_instruction_pointer;
            unsigned long pending_signals_bitmap_obsolete;
            unsigned long blocked_signals_bitmap_obsolete;
            unsigned long ignored_signals_bitmap_obsolete;
            unsigned long caught_signals_bitmap_obsolete;
            unsigned long waiting_channel_address;
            long pages_swapped_unmaintained;
            long children_pages_swapped_unmaintained;
            int exit_signal_sent_to_parent;
            int last_executed_cpu;
            int realtime_scheduling_priority;
            int scheduling_policy;
            long block_io_delay_ticks;
            long guest_time_ticks;
            long children_guest_time_ticks;
            long data_segment_start_address;
            long data_segment_end_address;
            long heap_start_address;
            long command_line_start_address;
            long command_line_end_address;
            long environment_start_address;
            long environment_end_address;
            int process_exit_code;

            // Use a scanner
            scanner >> state >> ppid >> process_group_id >> session_id >>
                controlling_terminal_device >> foreground_process_group_id >> kernel_process_flags

                >> minor_page_faults >> children_minor_page_faults >> major_page_faults >>
                children_major_page_faults

                >> utime >> stime >> children_user_time_ticks >> children_system_time_ticks

                >> priority >> nice >> num_threads >> interval_timer_value_obsolete

                >> start_time >> virtual_memory_bytes >> resident_memory_bytes >>
                resident_memory_limit_bytes

                >> code_segment_start_address >> code_segment_end_address >> stack_start_address >>
                current_stack_pointer >> current_instruction_pointer

                >> pending_signals_bitmap_obsolete >> blocked_signals_bitmap_obsolete >>
                ignored_signals_bitmap_obsolete >> caught_signals_bitmap_obsolete

                >> waiting_channel_address >> pages_swapped_unmaintained >>
                children_pages_swapped_unmaintained

                >> exit_signal_sent_to_parent >> last_executed_cpu >>
                realtime_scheduling_priority >> scheduling_policy

                >> block_io_delay_ticks >> guest_time_ticks >> children_guest_time_ticks

                >> data_segment_start_address >> data_segment_end_address >> heap_start_address

                >> command_line_start_address >> command_line_end_address >>
                environment_start_address >> environment_end_address >> process_exit_code;

            // Convert to bytes because resident_memory_bytes is currently in the page table count
            // and there are 4096 bytes in a page table
            resident_memory_bytes *= 4096;

            // Set prev for deltas
            if (prev_utime == 0 && prev_stime == 0) {
                prev_utime = utime;
                prev_stime = stime;
            }
        }
    }
    file.close();
}

// Parse the /proc/[PID]/status file
void PID::parse_status() {
    std::ifstream file;
    file.open("/proc/" + std::to_string(pid) + "/status");
    if (file.is_open()) {
        // Scan each line to find the UID and how much of a process's resident memory is shared
        // memory
        std::string line;
        while (std::getline(file, line)) {
            std::stringstream scanner(line);
            std::string label;
            scanner >> label;

            if (label == "Uid:") {
                scanner >> uid;
            } else if (label == "RssShmem:") {
                long shared_memory_kbytes;
                scanner >> shared_memory_kbytes;
                // get the bytes by converting from KB to B
                shared_memory_bytes = shared_memory_kbytes * 1024;
            }
        }
    }
    file.close();
}

// Get the the full command with arguments for this PID by parsing /proc/[PID]/cmdline
void PID::parse_cmdline() {
    std::ifstream file;
    file.open("/proc/" + std::to_string(pid) + "/cmdline", std::ios::binary);
    if (file.is_open()) {
        std::string line;
        char letter;

        // Command is split with null terminators '\0' so they must be skipped and treated as spaces
        // ' '
        while (file.get(letter)) {
            if (letter == '\0') {
                line += ' ';
            } else {
                line += letter;
            }
        }

        // Fall back to comm if the command is empty
        if (line.empty()) {
            cmdline = "[" + comm + "]";
        } else {
            cmdline = line;
        }
    }
    file.close();
}

// Parse /proc/[PID]/io to obtain read and write byte data
void PID::parse_io() {
    prev_read_bytes = read_bytes;
    prev_write_bytes = write_bytes;

    std::ifstream file;
    file.open("/proc/" + std::to_string(pid) + "/io");
    if (file.is_open())  // fails on processes you don't own; leave values as-is
    {
        std::string line;
        while (std::getline(file, line)) {
            std::stringstream scanner(line);
            std::string label;
            scanner >> label;

            if (label == "read_bytes:") {
                scanner >> read_bytes;
            } else if (label == "write_bytes:") {
                scanner >> write_bytes;
            }
        }
        if (prev_read_bytes == 0 && prev_write_bytes == 0) {
            prev_read_bytes = read_bytes;
            prev_write_bytes = write_bytes;
        }
    }
    file.close();
}

// Count the amount of file descriptor numbered entries that are in the /proc/[PID]/fd
void PID::parse_fd() {
    // Some PIDs do not allow access to /fd
    std::error_code error_code;
    // This will fill the error code if opening the fd directory fails
    std::filesystem::directory_iterator fd_dir("/proc/" + std::to_string(pid) + "/fd", error_code);

    // if the directory_iterator did not fail to open then loop through all fd entries and count
    if (!error_code) {
        int count = 0;
        for (const auto& fd : fd_dir) {
            count++;
        }
        fd_count = count;
    } else {
        fd_count = 0;
    }
}

// Update the PID by parsing all /proc/[PID] files and directories
void PID::update_pid() {
    parse_stat();
    parse_status();
    parse_io();
    parse_fd();
    if (cmdline == "") {
        parse_cmdline();
    }
}

// All the getters for a PID
pid_t PID::get_pid() const { return pid; }

pid_t PID::get_ppid() const { return ppid; }

long long PID::get_start_time() const { return start_time; }

char PID::get_state() const { return state; }

const std::string& PID::get_comm() const { return comm; }

const std::string& PID::get_cmdline() const { return cmdline; }

int PID::get_priority() const { return priority; }

int PID::get_nice() const { return nice; }

int PID::get_num_threads() const { return num_threads; }

long long PID::get_utime() const { return utime; }

long long PID::get_stime() const { return stime; }

long long PID::get_prev_utime() const { return prev_utime; }

long long PID::get_prev_stime() const { return prev_stime; }

long PID::get_virtual_memory_bytes() const { return virtual_memory_bytes; }

long PID::get_resident_memory_bytes() const { return resident_memory_bytes; }

long PID::get_shared_memory_bytes() const { return shared_memory_bytes; }

uid_t PID::get_uid() const { return uid; }

long long PID::get_read_bytes() const { return read_bytes; }

long long PID::get_write_bytes() const { return write_bytes; }

int PID::get_fd_count() const { return fd_count; }

long long PID::get_cpu_time_total() const { return utime + stime; }

long long PID::get_cpu_time_delta() const { return (utime - prev_utime) + (stime - prev_stime); }

long long PID::get_prev_read_bytes() const { return prev_read_bytes; }

long long PID::get_prev_write_bytes() const { return prev_write_bytes; }

long long PID::get_read_bytes_delta() const { return read_bytes - prev_read_bytes; }

long long PID::get_write_bytes_delta() const { return write_bytes - prev_write_bytes; }

// Checks if in /proc a directory or file is really a number which means it is a PID
bool Processes::is_pid(const std::string& str) {
    if (str.empty()) return false;
    for (char digit : str) {
        if (!std::isdigit(static_cast<unsigned char>(digit))) return false;
    }
    return true;
}

// Update the map of the pids, removing the PIDs that no longer exist and updating the remaining
// PIDs by reparsing
void Processes::update_pids() {
    // Declare a set to hold the path of the PID, add all the paths of the PIDS that are currently
    // in /proc and insert
    std::unordered_set<pid_t> pid_paths;
    for (const auto& entry : std::filesystem::directory_iterator("/proc")) {
        std::string name = entry.path().filename().string();
        // Make sure its a PID otherwise continue to the next iteration
        if (!is_pid(name)) continue;

        // Convert pid string to PID num
        pid_t pid = std::stoi(name);
        pid_paths.insert(pid);
    }

    // loop throught the PID map to check if the key of the PID within that index of the map matches
    // the PID num in the pid_paths set
    for (auto i = pids.begin(); i != pids.end();) {
        // This grabs the key the index in the map by using i->first
        // Uses count to check if this PID is in the set
        // Erase if the count is 0
        if (pid_paths.count(i->first) == 0)
            // Set i to the element after the erase
            i = pids.erase(i);
        else
            i++;
    }

    // Update only existing PIDs
    for (pid_t pid : pid_paths) {
        // Try to update the PIDs in the PID map that are in the unordered set
        try {
            pids.at(pid).update_pid();
            // If it doesnt exist in the map that it will throw an std::out_of_range exception
            // Catch it and insert the PID into the map because it doesnt exist yet
        } catch (const std::out_of_range& e) {
            pids.insert({pid, PID(pid)});
            pids.at(pid).update_pid();
        }
    }
}

// Return a map of all the PIDs
const std::unordered_map<pid_t, PID>& Processes::get_pids() const { return pids; }