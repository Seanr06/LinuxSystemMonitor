#include "json_serializer.h"

#include <chrono>
#include <sstream>

// Build the string in the .json syntax so that the Jackson parser can understand the data
// Use Cammel case to match the java records naming convention

// Takes in a CoreData struct and constructs the string using a scanner in .json syntax
std::string JsonSerializer::serialize_core_data(const CoreData& core) {
    std::stringstream scanner;
    scanner << "{"
            << "\"usagePercent\":" << core.usage_percent << ","
            << "\"userTime\":" << core.user << ","
            << "\"nice\":" << core.nice << ","
            << "\"system\":" << core.system << ","
            << "\"idle\":" << core.idle << ","
            << "\"iowait\":" << core.iowait << ","
            << "\"irq\":" << core.irq << ","
            << "\"softirq\":" << core.softirq << ","
            << "\"steal\":" << core.steal << ","
            << "\"active\":" << core.active << ","
            << "\"total\":" << core.total << "}";
    return scanner.str();
}

// Takes in the vector of CoreData structs to build the string for all cores
std::string JsonSerializer::serialize_cores(const std::vector<CoreData>& cores) {
    std::stringstream scanner;
    scanner << "[";
    // loop through the vector
    for (size_t i = 0; i < cores.size(); i++) {
        scanner << serialize_core_data(cores[i]);
        // Only add commas between core string builds if its not the last core struct
        if (i + 1 < cores.size()) {
            scanner << ",";
        }
    }
    scanner << "]";
    return scanner.str();
}

// Covert all of the monitor data by taking in the StatisticsData struct
std::string JsonSerializer::serialize_snapshot(const StatisticsData& data) {
    // Grab the captured_system_time which is currently system_clock::time_point (The time since
    // 1970 in nanoseconds)
    // time_since_epoch() returns the actual duration in nanoseconds
    // Does a cast converting it into milliseconds
    // .count() returns the time as an actual number for long long
    long long time_capture = std::chrono::duration_cast<std::chrono::milliseconds>(
                                 data.captured_system_time.time_since_epoch())
                                 .count();

    // Use a scanner to put all the stats into a string with .json syntax formatting
    std::stringstream scanner;
    scanner << "{"
            << "\"timeCapture\":" << time_capture << ","
            << "\"totalCpu\":" << serialize_core_data(data.total_cpu_core) << ","
            << "\"cores\":" << serialize_cores(data.cores) << ","
            << "\"memTotal\":" << data.mem_total << ","
            << "\"memFree\":" << data.mem_free << ","
            << "\"memAvailable\":" << data.mem_available << ","
            << "\"cached\":" << data.cached << ","
            << "\"buffers\":" << data.buffers << ","
            << "\"swapTotal\":" << data.swap_total << ","
            << "\"swapFree\":" << data.swap_free << ","
            << "\"active\":" << data.active << ","
            << "\"inactive\":" << data.inactive << ","
            << "\"activeAnon\":" << data.active_anon << ","
            << "\"inactiveAnon\":" << data.inactive_anon << ","
            << "\"activeFile\":" << data.active_file << ","
            << "\"inactiveFile\":" << data.inactive_file << ","
            << "\"slab\":" << data.slab << ","
            << "\"sreclaimable\":" << data.sreclaimable << ","
            << "\"sunreclaim\":" << data.sunreclaim << ","
            << "\"dirty\":" << data.dirty << ","
            << "\"writeback\":" << data.writeback << ","
            << "\"anonPages\":" << data.anon_pages << ","
            << "\"mapped\":" << data.mapped << ","
            << "\"shmem\":" << data.shmem << ","
            << "\"kernelInfo\":\"" << data.kernel_info << "\","
            << "\"osInfo\":\"" << data.os_info << "\","
            << "\"lastMinuteLoad\":" << data.last_minute_load << ","
            << "\"lastFiveMinutesLoad\":" << data.last_five_minutes_load << ","
            << "\"lastFifteenMinutesLoad\":" << data.last_fifteen_minutes_load << ","
            << "\"upTime\":" << data.up_time << ","
            << "\"idleTime\":" << data.idletime << ","
            << "\"procsRunning\":" << data.procs_running << ","
            << "\"procsBlocked\":" << data.procs_blocked << ","
            << "\"ctxtPerSecond\":" << data.ctxt_per_second << ","
            << "\"interruptionsPerSecond\":" << data.interruptions_per_second << "}";
    return scanner.str();
}