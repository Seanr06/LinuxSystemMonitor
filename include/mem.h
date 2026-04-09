#ifndef MEM_H
#define MEM_H

#include <fstream>
#include <string>
#include <sstream>

class MemoryParser
{
private:
    std::ifstream file;

    long mem_total;
    long mem_available;

    long cached;
    long buffers;

    long swap_total;
    long swap_free;

    long active;
    long inactive;

    long active_anon;
    long inactive_anon;
    long active_file;
    long inactive_file;

    long slab;
    long sreclaimable;
    long sunreclaim;

    long dirty;
    long writeback;

    long anon_pages;
    long mapped;
    long shmem;

public:
    MemoryParser();

    void computeMemoryStatistics();

    // Total / Available / Used
    long getMemTotal() const;
    long getMemAvailable() const;
    long getMemUsed() const;

    // Cache / Buffers
    long getCached() const;
    long getBuffers() const;

    // Swap
    long getSwapTotal() const;
    long getSwapFree() const;
    long getSwapUsed() const;

    // Activity
    long getActive() const;
    long getInactive() const;

    // Anon / File breakdown
    long getActiveAnon() const;
    long getInactiveAnon() const;
    long getActiveFile() const;
    long getInactiveFile() const;

    // Kernel (Slab)
    long getSlab() const;
    long getSReclaimable() const;
    long getSUnreclaim() const;

    // Disk write state
    long getDirty() const;
    long getWriteback() const;

    // Process-related memory
    long getAnonPages() const;
    long getMapped() const;
    long getShmem() const;
};

#endif