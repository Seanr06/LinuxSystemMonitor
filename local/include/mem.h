#ifndef MEM_H
#define MEM_H

#include <fstream>
#include <sstream>
#include <string>

class MemoryParser {
   private:
    std::ifstream file;

    long mem_total;
    long mem_free;
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

    void compute_memory_statistics();

    // Total / Free / Available / Used
    long get_mem_total() const;
    long get_mem_free() const;
    long get_mem_available() const;
    long get_mem_used() const;

    // Cache / Buffers
    long get_cached() const;
    long get_buffers() const;

    // Swap
    long get_swap_total() const;
    long get_swap_free() const;
    long get_swap_used() const;

    // Activity
    long get_active() const;
    long get_inactive() const;

    // Anon / File breakdown
    long get_active_anon() const;
    long get_inactive_anon() const;
    long get_active_file() const;
    long get_inactive_file() const;

    // Kernel (Slab)
    long get_slab() const;
    long get_sreclaimable() const;
    long get_sunreclaim() const;

    // Disk write state
    long get_dirty() const;
    long get_writeback() const;

    // Process-related memory
    long get_anon_pages() const;
    long get_mapped() const;
    long get_shmem() const;
};

#endif