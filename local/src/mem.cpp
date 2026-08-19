#include "mem.h"

#include "proc_paths.h"

using std::string;

MemoryParser::MemoryParser() {
    mem_total = 0;
    mem_free = 0;
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

void MemoryParser::compute_memory_statistics() {
    // Parse all of /proc/mem and the lines that contain mem stat strings
    file.open(ProcPaths::kMemInfo);
    string line;
    if (file.is_open()) {
        while (std::getline(file, line)) {
            std::stringstream scanner(line);
            std::string label;
            long value;
            scanner >> label >> value;
            if (label == "MemTotal:") {
                mem_total = value;
            } else if (label == "MemFree:") {
                mem_free = value;
            } else if (label == "MemAvailable:") {
                mem_available = value;
            } else if (label == "Cached:") {
                cached = value;
            } else if (label == "Buffers:") {
                buffers = value;
            } else if (label == "SwapTotal:") {
                swap_total = value;
            } else if (label == "SwapFree:") {
                swap_free = value;
            } else if (label == "SwapCached:") {
            } else if (label == "Active:") {
                active = value;
            } else if (label == "Inactive:") {
                inactive = value;
            } else if (label == "Active(anon):") {
                active_anon = value;
            } else if (label == "Inactive(anon):") {
                inactive_anon = value;
            } else if (label == "Active(file):") {
                active_file = value;
            } else if (label == "Inactive(file):") {
                inactive_file = value;
            } else if (label == "Slab:") {
                slab = value;
            } else if (label == "SReclaimable:") {
                sreclaimable = value;
            } else if (label == "SUnreclaim:") {
                sunreclaim = value;
            } else if (label == "Dirty:") {
                dirty = value;
            } else if (label == "Writeback:") {
                writeback = value;
            } else if (label == "AnonPages:") {
                anon_pages = value;
            } else if (label == "Mapped:") {
                mapped = value;
            } else if (label == "Shmem:") {
                shmem = value;
            }
        }
    }
    file.close();
}

// Getter methods returning all the fields
long MemoryParser::get_mem_total() const { return mem_total; }

long MemoryParser::get_mem_free() const { return mem_free; }

long MemoryParser::get_mem_available() const { return mem_available; }

long MemoryParser::get_mem_used() const { return mem_total - mem_available; }

long MemoryParser::get_cached() const { return cached; }

long MemoryParser::get_buffers() const { return buffers; }

long MemoryParser::get_swap_total() const { return swap_total; }

long MemoryParser::get_swap_free() const { return swap_free; }

long MemoryParser::get_swap_used() const { return swap_total - swap_free; }

long MemoryParser::get_active() const { return active; }

long MemoryParser::get_inactive() const { return inactive; }

long MemoryParser::get_active_anon() const { return active_anon; }

long MemoryParser::get_inactive_anon() const { return inactive_anon; }

long MemoryParser::get_active_file() const { return active_file; }

long MemoryParser::get_inactive_file() const { return inactive_file; }

long MemoryParser::get_slab() const { return slab; }

long MemoryParser::get_sreclaimable() const { return sreclaimable; }

long MemoryParser::get_sunreclaim() const { return sunreclaim; }

long MemoryParser::get_dirty() const { return dirty; }

long MemoryParser::get_writeback() const { return writeback; }

long MemoryParser::get_anon_pages() const { return anon_pages; }

long MemoryParser::get_mapped() const { return mapped; }

long MemoryParser::get_shmem() const { return shmem; }