#include <fstream>

#include "proc_paths.h"
#include "uptime.h"

UptimeParser::UptimeParser()
{
    file.open(ProcPaths::kUptime);
    upTime = 0;
    idleTime = 0;
}

void UptimeParser::computeUptime()
{
    file.clear();
    file.seekg(0);
    if (file.is_open())
    {
        file >> upTime >> idleTime;
    }
}

double UptimeParser::getUptime() const
{
    return upTime;
}

double UptimeParser::getIdleTime() const
{
    return idleTime;
}