#ifndef SYSTEMINFO_H
#define SYSTEMINFO_H

#include <string>
#include <vector>
#include <fstream>

#include "proc_paths.h"

using std::string;

// Static stats
class StaticSystemStats
{
private:
    string kernelInfo;
    string osInfo;

public:
    StaticSystemStats();
    string getKernelInfo() const;
    string getOSinfo() const;
};

// Dynamic Stats
class DynamicSystemStats
{
private:
    std::ifstream uptimeFile;
    std::ifstream loadFile;

    float lastMinuteLoad;
    float lastFiveMinutesLoad;
    float lastFifteenMinutesLoad;

    float upTime;
    float idletime;

public:
    DynamicSystemStats();
    void updateSystemInfo();

    // Load avg
    float getLastMinuteLoad() const;
    float getLastFiveMinutesLoad() const;
    float getLastFifteenMinutesLoad() const;

    // uptime
    float getUptime() const;
    float getIdletime() const;
};

#endif