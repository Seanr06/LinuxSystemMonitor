#ifndef UPTIME_H
#define UPTIME_H

#include <fstream>

class UptimeParser
{
private:
    std::ifstream file;
    double upTime;
    double idleTime;

public:
    UptimeParser();

    void computeUptime();
    double getUptime() const;
    double getIdleTime() const;
};

#endif