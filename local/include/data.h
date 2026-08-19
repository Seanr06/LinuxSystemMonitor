#ifndef DATA_H
#define DATA_H

#include <sys/types.h>

#include <chrono>
#include <string>
#include <vector>

// fields for core data
struct CoreData {
    float usage_percent{0};

    long long user{0};
    long long nice{0};
    long long system{0};
    long long idle{0};
    long long iowait{0};
    long long irq{0};
    long long softirq{0};
    long long steal{0};

    long long active{0};
    long long total{0};
};

// Fields for a PID
struct ProcessData {
    pid_t pid{0};
    pid_t ppid{0};
    long long start_time{0};

    char state{'?'};
    std::string comm;
    std::string cmdline;

    int priority{0};
    int nice{0};
    int num_threads{0};

    long long utime{0};
    long long stime{0};
    long long cpu_time_delta{0};

    long virtual_memory_bytes{0};
    long resident_memory_bytes{0};
    long shared_memory_bytes{0};

    uid_t uid{0};

    long long read_bytes{0};
    long long write_bytes{0};
    long long read_bytes_delta{0};
    long long write_bytes_delta{0};

    int fd_count{0};
};

// Fields from every other statistic parsed in the parsin classes
struct StatisticsData {
    // For getting MB/S amd comparing before and after
    std::chrono::steady_clock::time_point captured_at;
    // System clock capture for getting the time that the data was grabbed for ordering in
    std::chrono::system_clock::time_point captured_system_time;

    // Stat.cpp
    CoreData total_cpu_core;
    std::vector<CoreData> cores;

    long long procs_running{0};
    long long procs_blocked{0};
    long long pre_ctxt{0};
    long long pre_interruptions{0};
    long long ctxt{0};
    long long interruptions{0};
    double ctxt_per_second{0};
    double interruptions_per_second{0};

    // Mem.cpp
    long mem_total{0};
    long mem_free{0};
    long mem_available{0};

    long cached{0};
    long buffers{0};

    long swap_total{0};
    long swap_free{0};

    long active{0};
    long inactive{0};

    long active_anon{0};
    long inactive_anon{0};
    long active_file{0};
    long inactive_file{0};

    long slab{0};
    long sreclaimable{0};
    long sunreclaim{0};

    long dirty{0};
    long writeback{0};

    long anon_pages{0};
    long mapped{0};
    long shmem{0};

    // system_info.cpp
    std::string kernel_info;
    std::string os_info;

    float last_minute_load{0};
    float last_five_minutes_load{0};
    float last_fifteen_minutes_load{0};

    float up_time{0};
    float idletime{0};

    // Process.cpp
    std::vector<ProcessData> pids;
};

#endif