#ifndef PROCESS_H
#define PROCESS_H

#include <sys/types.h>

#include <string>
#include <unordered_map>

class PID {
   private:
    // identity
    pid_t pid;
    pid_t ppid;
    long long start_time;

    // display / state
    char state;
    std::string comm;
    std::string cmdline;

    // scheduling
    int priority;
    int nice;
    int num_threads;

    // cpu (deltas need history)
    long long utime;
    long long stime;
    long long prev_utime;
    long long prev_stime;

    // Memory
    long virtual_memory_bytes;
    long resident_memory_bytes;
    long shared_memory_bytes;

    // owner
    uid_t uid;

    // io
    long long read_bytes;
    long long write_bytes;
    long long prev_read_bytes;
    long long prev_write_bytes;

    // file descriptors
    int fd_count;

   public:
    PID(pid_t pid);
    void parse_stat();
    void parse_status();
    void parse_cmdline();
    void parse_io();
    void parse_fd();

    void update_pid();

    /**
     * Getter methods for PID statistics
     */
    pid_t get_pid() const;
    pid_t get_ppid() const;
    long long get_start_time() const;

    char get_state() const;
    const std::string& get_comm() const;
    const std::string& get_cmdline() const;

    // scheduling
    int get_priority() const;
    int get_nice() const;
    int get_num_threads() const;

    // cpu
    long long get_utime() const;
    long long get_stime() const;
    long long get_prev_utime() const;
    long long get_prev_stime() const;
    long long get_cpu_time_total() const;
    long long get_cpu_time_delta() const;

    // memory
    long get_virtual_memory_bytes() const;
    long get_resident_memory_bytes() const;
    long get_shared_memory_bytes() const;

    // owner
    uid_t get_uid() const;

    // io
    long long get_read_bytes() const;
    long long get_write_bytes() const;
    long long get_prev_read_bytes() const;
    long long get_prev_write_bytes() const;
    long long get_read_bytes_delta() const;
    long long get_write_bytes_delta() const;

    // fd
    int get_fd_count() const;
};

// Class to hold and acquire all PIDs in the system
class Processes {
   private:
    std::unordered_map<pid_t, PID> pids;

   public:
    bool is_pid(const std::string& str);
    void update_pids();
    const std::unordered_map<pid_t, PID>& get_pids() const;
};

#endif
