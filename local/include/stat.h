#ifndef STAT_H
#define STAT_H

#include <fstream>
#include <string>
#include <vector>

using std::string;

// Core calculator used to parse and store the the current and previous cpu core stats for
// calculating the deltas
class CoreCalculator {
   private:
    // Stuct that contains the core data
    struct CoreStats {
        long long user{0};    // Time spent running normal user-space processes
        long long nice{0};    // Time spent running user-space processes that has lower priority set
        long long system{0};  // Time spent running kernel code for the process
        long long idle{0};    // Time the CPU had spend doing nothing
        long long iowait{0};  // Time the CPU was idle waiting on disk/IO operations
        long long irq{0};     // Time spent on hardware interrupts
        long long softirq{0};  // Time spent on software interrupts
        long long steal{0};    // Amount of time that a virtual CPU wanted to run, but could not

        long long active{0};  // Sum of the cpu fields that represent the cpu doing actual work
        long long total{0};   // Sum of every field including when the cpu does nothing or waits
    };
    // Store the prev and current core stats for calculating deltas
    CoreStats statistics{};
    CoreStats pre_statistics{};

   public:
    // Parse the cpu stats in /proc/stat
    bool compute_stats(const string& line);

    // Deltas of the prev and curr core active and total calculations
    long long calc_total() const;
    long long get_active() const;

    // Actual core percentage usage (active-delta/ total-delta) * 100
    float get_cpu_usage_percent() const;

    // Getter methods returning the deltas of the cpu core stats
    long long get_user() const;
    long long get_nice() const;
    long long get_system() const;
    long long get_idle() const;
    long long get_io_wait() const;
    long long get_irq() const;
    long long get_soft_irq() const;
    long long get_steal() const;
};

class StatParser {
   private:
    std::vector<CoreCalculator> cores;  // Index corresponds to core[N] in /proc/stat
    CoreCalculator
        total_cpu_core;  // The cpu stats from the aggregate cpu line in /proc/stat (first line)
    std::ifstream file;  // File which is /proc/stat
    long long procs_blocked{0};      // Current amount of processes (PIDS) blocked
    long long procs_running{0};      // Current amount of processes (PIDS) running
    long long pre_ctxt{0};           // Previously calculated context switches from last parse
    long long pre_interruptions{0};  // Previously calculated interruptions from last parse
    long long ctxt{0};               // Current calculated context switches
    long long interruptions{0};      // Current calculated interruptions

   public:
    // Contructor opens /proc/stat and grabs initial core stats
    StatParser();

    // Parse all of /proc/stat
    void update_cpu_usage();
    void compute_stats();

    // getter methods for this class
    CoreCalculator get_total_core() const;
    CoreCalculator get_individual_core(int i) const;
    int get_core_count() const;

    long long get_procs_running() const;
    long long get_procs_blocked() const;
    long long get_pre_ctxt() const;
    long long get_ctxt() const;
    long long get_interruptions() const;
    long long get_pre_interruptions() const;
};

#endif