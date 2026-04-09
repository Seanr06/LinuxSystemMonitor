#ifndef STAT_H
#define STAT_H

#include <string>
#include <vector>
#include <fstream>

#include "proc_paths.h"

using std::string;

// ----------------------------------------------------------
// Core class
// ----------------------------------------------------------
class CoreCalculator
{
private:
    struct CoreStats
    {
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
    CoreStats statistics{};
    CoreStats preStatistics{};

public:
    bool computeStats(const string &line);

    long long calcTotal() const;
    long long getActive() const;

    float getCpuUsagePercent() const;

    long long getUser() const;
    long long getNice() const;
    long long getSystem() const;
    long long getIdle() const;
    long long getIOWait() const;
    long long getIRQ() const;
    long long getSoftIRQ() const;
    long long getSteal() const;
};

// ----------------------------------------------------------
// StatParser class
// ----------------------------------------------------------
class StatParser
{
private:
    std::vector<CoreCalculator> cores;
    CoreCalculator totalCPUCore;
    std::ifstream file;
    long long procs_blocked;
    long long procs_running;
    long long ctxt;
    long long interuptions;

public:
    StatParser();

    void updateCPUUsage();
    void computeStats();

    CoreCalculator getTotalCore() const;
    CoreCalculator getIndividualCore(int i) const;
    int getCoreCount() const;

    long long getProcs_running() const;
    long long getProcs_blocked() const;
    long long getCtxt() const;
    long long getInteruptions() const;
};

#endif