#include <fstream>
#include <string>
#include <sstream>

#include "proc_paths.h"
#include "mem.h"

using std::string;

MemoryParser::MemoryParser()
{
    mem_total = 0;
    mem_available = 0;
    cached = 0;
    buffers = 0;
    swap_total = 0;
    swap_free = 0;
    active = 0;
    inactive = 0;
    active_anon = 0;
    inactive_anon = 0;
    active_file = 0;
    inactive_file = 0;
    slab = 0;
    sreclaimable = 0;
    sunreclaim = 0;
    dirty = 0;
    writeback = 0;
    anon_pages = 0;
    mapped = 0;
    shmem = 0;
}

void MemoryParser::computeMemoryStatistics()
{
    file.open(ProcPaths::kMemInfo);
    string line;
    if (file.is_open())
    {

        while (std::getline(file, line))
        {
            std::stringstream scanner(line);
            std::string label;
            if (line.find("MemTotal:") != std::string::npos)
            {
                scanner >> label >> mem_total;
            }
            else if (line.find("MemAvailable:") != std::string::npos)
            {
                scanner >> label >> mem_available;
            }
            else if (line.find("Cached:") != std::string::npos)
            {
                scanner >> label >> cached;
            }
            else if (line.find("Buffers:") != std::string::npos)
            {
                scanner >> label >> buffers;
            }

            else if (line.find("SwapTotal:") != std::string::npos)
            {
                scanner >> label >> swap_total;
            }
            else if (line.find("SwapFree:") != std::string::npos)
            {
                scanner >> label >> swap_free;
            }

            else if (line.find("Active:") != std::string::npos)
            {
                scanner >> label >> active;
            }
            else if (line.find("Inactive:") != std::string::npos)
            {
                scanner >> label >> inactive;
            }

            else if (line.find("Active(anon):") != std::string::npos)
            {
                scanner >> label >> active_anon;
            }
            else if (line.find("Inactive(anon):") != std::string::npos)
            {
                scanner >> label >> inactive_anon;
            }
            else if (line.find("Active(file):") != std::string::npos)
            {
                scanner >> label >> active_file;
            }
            else if (line.find("Inactive(file):") != std::string::npos)
            {
                scanner >> label >> inactive_file;
            }

            else if (line.find("Slab:") != std::string::npos)
            {
                scanner >> label >> slab;
            }
            else if (line.find("SReclaimable:") != std::string::npos)
            {
                scanner >> label >> sreclaimable;
            }
            else if (line.find("SUnreclaim:") != std::string::npos)
            {
                scanner >> label >> sunreclaim;
            }

            else if (line.find("Dirty:") != std::string::npos)
            {
                scanner >> label >> dirty;
            }
            else if (line.find("Writeback:") != std::string::npos)
            {
                scanner >> label >> writeback;
            }

            else if (line.find("AnonPages:") != std::string::npos)
            {
                scanner >> label >> anon_pages;
            }
            else if (line.find("Mapped:") != std::string::npos)
            {
                scanner >> label >> mapped;
            }
            else if (line.find("Shmem:") != std::string::npos)
            {
                scanner >> label >> shmem;
            }
        }
    }
    file.close();
}

long MemoryParser::getMemTotal() const
{
    return mem_total;
}

long MemoryParser::getMemAvailable() const
{
    return mem_available;
}

long MemoryParser::getMemUsed() const
{
    return mem_total - mem_available;
}

long MemoryParser::getCached() const
{
    return cached;
}

long MemoryParser::getBuffers() const
{
    return buffers;
}

long MemoryParser::getSwapTotal() const
{
    return swap_total;
}

long MemoryParser::getSwapFree() const
{
    return swap_free;
}

long MemoryParser::getSwapUsed() const
{
    return swap_total - swap_free;
}

long MemoryParser::getActive() const
{
    return active;
}

long MemoryParser::getInactive() const
{
    return inactive;
}

long MemoryParser::getActiveAnon() const
{
    return active_anon;
}

long MemoryParser::getInactiveAnon() const
{
    return inactive_anon;
}

long MemoryParser::getActiveFile() const
{
    return active_file;
}

long MemoryParser::getInactiveFile() const
{
    return inactive_file;
}

long MemoryParser::getSlab() const
{
    return slab;
}

long MemoryParser::getSReclaimable() const
{
    return sreclaimable;
}

long MemoryParser::getSUnreclaim() const
{
    return sunreclaim;
}

long MemoryParser::getDirty() const
{
    return dirty;
}

long MemoryParser::getWriteback() const
{
    return writeback;
}

long MemoryParser::getAnonPages() const
{
    return anon_pages;
}

long MemoryParser::getMapped() const
{
    return mapped;
}

long MemoryParser::getShmem() const
{
    return shmem;
}