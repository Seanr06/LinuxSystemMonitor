#include "stat.h"

#include <sstream>
#include <vector>

#include "proc_paths.h"

using std::string;

// Method called to parse a line in/proc/stat that contains the cpu statistics and store the parsed
// data into the proper variables
bool CoreCalculator::compute_stats(const string& line) {
    // Get the previous core calculation
    pre_statistics = statistics;

    // Use a scanner to parse the line
    std::stringstream scanner(line);
    std::string cpu_label;

    scanner >> cpu_label >> statistics.user >> statistics.nice >> statistics.system >>
        statistics.idle >> statistics.iowait >> statistics.irq >> statistics.softirq >>
        statistics.steal;

    statistics.total = statistics.user + statistics.nice + statistics.system + statistics.idle +
                       statistics.iowait + statistics.irq + statistics.softirq + statistics.steal;
    statistics.active = statistics.user + statistics.nice + statistics.system + statistics.irq +
                        statistics.softirq + statistics.steal;

    // Return if the scanning failed to parse the line if it does not contain the cpu stats format
    return !scanner.fail();
}

// Calculate total delta
long long CoreCalculator::calc_total() const {
    long long delta = statistics.total - pre_statistics.total;
    return delta < 0 ? 0 : delta;
}

// Calculate active delta
long long CoreCalculator::get_active() const {
    long long delta = statistics.active - pre_statistics.active;
    return delta < 0 ? 0 : delta;
}

// Calculate the actual cores percentage usage
float CoreCalculator::get_cpu_usage_percent() const {
    long long delta_total = statistics.total - pre_statistics.total;
    long long delta_active = statistics.active - pre_statistics.active;

    // If the total is 0 then the cpu usage is 0
    if (delta_total <= 0) {
        return 0.0;
    }

    return (static_cast<double>(delta_active) / static_cast<double>(delta_total)) * 100.0;
}
/**
 * All the getter methods for calculating the stats from pre and post statistics
 */
long long CoreCalculator::get_user() const {
    long long delta = statistics.user - pre_statistics.user;
    return delta < 0 ? 0 : delta;
}

long long CoreCalculator::get_nice() const {
    long long delta = statistics.nice - pre_statistics.nice;
    return delta < 0 ? 0 : delta;
}

long long CoreCalculator::get_system() const {
    long long delta = statistics.system - pre_statistics.system;
    return delta < 0 ? 0 : delta;
}

long long CoreCalculator::get_idle() const {
    long long delta = statistics.idle - pre_statistics.idle;
    return delta < 0 ? 0 : delta;
}

long long CoreCalculator::get_io_wait() const {
    long long delta = statistics.iowait - pre_statistics.iowait;
    return delta < 0 ? 0 : delta;
}

long long CoreCalculator::get_irq() const {
    long long delta = statistics.irq - pre_statistics.irq;
    return delta < 0 ? 0 : delta;
}

long long CoreCalculator::get_soft_irq() const {
    long long delta = statistics.softirq - pre_statistics.softirq;
    return delta < 0 ? 0 : delta;
}

long long CoreCalculator::get_steal() const {
    long long delta = statistics.steal - pre_statistics.steal;
    return delta < 0 ? 0 : delta;
}

// Constructor that does an initial parse of all the per core stats so that it can be pushed onto
// the cores vector for later updating
StatParser::StatParser() {
    // open file and clear the cores vactor
    file.open(ProcPaths::kProcStat);
    cores.clear();
    string line;

    if (file.is_open()) {
        // Grab the lines that contain cpu and parse
        // total cpu is the first line
        std::getline(file, line);
        if (line.find("cpu") != std::string::npos) {
            total_cpu_core.compute_stats(line);
        }

        // While there are lines in /proc/stat left and the line contains cpu, parse the line and
        // push the core
        // std::string::npos means not found
        while (std::getline(file, line) && line.find("cpu") != std::string::npos) {
            CoreCalculator curr_core;
            curr_core.compute_stats(line);
            cores.push_back(curr_core);
        }
    }
    file.close();
}

// open /proc/stats again to update the cores vector
void StatParser::update_cpu_usage() {
    string line;
    file.open(ProcPaths::kProcStat);

    if (file.is_open()) {
        std::getline(file, line);
        // total cpu is the first line
        if (line.find("cpu") != std::string::npos) {
            total_cpu_core.compute_stats(line);
        }
        int i = 0;
        // While there are lines in /proc/stat left and the line contains cpu, parse the line and
        // push the core
        // std::string::npos means not found
        while (std::getline(file, line) && line.find("cpu") != std::string::npos) {
            if (i < static_cast<int>(cores.size())) {
                cores[i].compute_stats(line);
            }
            i++;
        }
    }
    file.close();
}

// parse the rest of /proc/stat to obtain the other statistices
void StatParser::compute_stats() {
    string label;
    string line;
    file.open(ProcPaths::kProcStat);

    if (file.is_open()) {
        while (std::getline(file, line)) {
            if (line.find("intr") != std::string::npos) {
                pre_interruptions = interruptions;
                std::stringstream scanner(line);
                scanner >> label >> interruptions;
            }
            if (line.find("procs_running") != std::string::npos) {
                std::stringstream scanner(line);
                scanner >> label >> procs_running;
            }
            if (line.find("procs_blocked") != std::string::npos) {
                std::stringstream scanner(line);
                scanner >> label >> procs_blocked;
            }
            if (line.find("ctxt") != std::string::npos) {
                pre_ctxt = ctxt;
                std::stringstream scanner(line);
                scanner >> label >> ctxt;
            }
        }
    }
    file.close();
}

// Getter methods returning the fields in the StatParser class
int StatParser::get_core_count() const { return static_cast<int>(cores.size()); }

CoreCalculator StatParser::get_total_core() const { return total_cpu_core; }

CoreCalculator StatParser::get_individual_core(int i) const { return cores[i]; }

long long StatParser::get_procs_running() const { return procs_running; }

long long StatParser::get_procs_blocked() const { return procs_blocked; }

long long StatParser::get_ctxt() const { return ctxt; }

long long StatParser::get_interruptions() const { return interruptions; }

long long StatParser::get_pre_ctxt() const { return pre_ctxt; }

long long StatParser::get_pre_interruptions() const { return pre_interruptions; }