#include <fstream>
#include <string>
#include <sstream>
#include <vector>

#include "stat.h"

using std::string;

// ----------------------------------------------------------
// Core class
// ----------------------------------------------------------
bool CoreCalculator::computeStats(const string &line)
{
    preStatistics = statistics;

    std::stringstream scanner(line);
    std::string cpuLabel;

    scanner >> cpuLabel >> statistics.user >> statistics.nice >> statistics.system >> statistics.idle >> statistics.iowait >> statistics.irq >> statistics.softirq >> statistics.steal;

    statistics.total = statistics.user + statistics.nice + statistics.system + statistics.idle + statistics.iowait + statistics.irq + statistics.softirq + statistics.steal;

    statistics.active = statistics.user + statistics.nice + statistics.system + statistics.irq + statistics.softirq + statistics.steal;

    return !scanner.fail();
}

long long CoreCalculator::calcTotal() const
{
    long long delta = statistics.total - preStatistics.total;
    return delta < 0 ? 0 : delta;
}

long long CoreCalculator::getActive() const
{
    long long delta = statistics.active - preStatistics.active;
    return delta < 0 ? 0 : delta;
}

float CoreCalculator::getCpuUsagePercent() const
{
    long long deltaTotal = statistics.total - preStatistics.total;
    long long deltaActive = statistics.active - preStatistics.active;

    if (deltaTotal <= 0)
    {
        return 0.0;
    }

    return (static_cast<double>(deltaActive) / static_cast<double>(deltaTotal)) * 100.0;
}

long long CoreCalculator::getUser() const
{
    long long delta = statistics.user - preStatistics.user;
    return delta < 0 ? 0 : delta;
}

long long CoreCalculator::getNice() const
{
    long long delta = statistics.nice - preStatistics.nice;
    return delta < 0 ? 0 : delta;
}

long long CoreCalculator::getSystem() const
{
    long long delta = statistics.system - preStatistics.system;
    return delta < 0 ? 0 : delta;
}

long long CoreCalculator::getIdle() const
{
    long long delta = statistics.idle - preStatistics.idle;
    return delta < 0 ? 0 : delta;
}

long long CoreCalculator::getIOWait() const
{
    long long delta = statistics.iowait - preStatistics.iowait;
    return delta < 0 ? 0 : delta;
}

long long CoreCalculator::getIRQ() const
{
    long long delta = statistics.irq - preStatistics.irq;
    return delta < 0 ? 0 : delta;
}

long long CoreCalculator::getSoftIRQ() const
{
    long long delta = statistics.softirq - preStatistics.softirq;
    return delta < 0 ? 0 : delta;
}

long long CoreCalculator::getSteal() const
{
    long long delta = statistics.steal - preStatistics.steal;
    return delta < 0 ? 0 : delta;
}

// ----------------------------------------------------------
// StatParser class
// ----------------------------------------------------------
StatParser::StatParser()
{
    procs_blocked = 0;
    procs_running = 0;
    ctxt = 0;

    file.open(ProcPaths::kProcStat);
    cores.clear();
    string line;

    if (file.is_open())
    {
        std::getline(file, line);
        if (line.find("cpu") != std::string::npos)
        {
            totalCPUCore.computeStats(line);
        }

        while (std::getline(file, line) && line.find("cpu") != std::string::npos)
        {
            CoreCalculator currCore;
            currCore.computeStats(line);
            cores.push_back(currCore);
        }
    }
    file.close();
}

void StatParser::updateCPUUsage()
{
    string line;
    file.open(ProcPaths::kProcStat);

    if (file.is_open())
    {
        std::getline(file, line);
        if (line.find("cpu") != std::string::npos)
        {
            totalCPUCore.computeStats(line);
        }
        int i = 0;
        while (std::getline(file, line) && line.find("cpu") != std::string::npos)
        {
            if (i < static_cast<int>(cores.size()))
            {
                cores[i].computeStats(line);
            }
            i++;
        }
    }
    file.close();
}

void StatParser::computeStats()
{
    string label;
    string line;
    file.open(ProcPaths::kProcStat);

    if (file.is_open())
    {
        while (std::getline(file, line))
        {

            if (line.find("intr") != std::string::npos)
            {
                std::stringstream scanner(line);
                scanner >> label >> interuptions;
            }
            if (line.find("procs_running") != std::string::npos)
            {
                std::stringstream scanner(line);
                scanner >> label >> procs_running;
            }
            if (line.find("procs_blocked") != std::string::npos)
            {
                std::stringstream scanner(line);
                scanner >> label >> procs_blocked;
            }
            if (line.find("ctxt") != std::string::npos)
            {
                std::stringstream scanner(line);
                scanner >> label >> ctxt;
            }
        }
    }
    file.close();
}

int StatParser::getCoreCount() const
{
    return cores.size();
}

CoreCalculator StatParser::getTotalCore() const
{
    return totalCPUCore;
}

CoreCalculator StatParser::getIndividualCore(int i) const
{
    return cores[i];
}

long long StatParser::getProcs_running() const
{
    return procs_running;
}

long long StatParser::getProcs_blocked() const
{
    return procs_blocked;
}

long long StatParser::getCtxt() const
{
    return ctxt;
}

long long StatParser::getInteruptions() const
{
    return interuptions;
}