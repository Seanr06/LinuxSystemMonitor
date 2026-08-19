#ifndef COLLECTOR_H
#define COLLECTOR_H

#include <atomic>
#include <condition_variable>
#include <memory>
#include <mutex>
#include <thread>

#include "data.h"
#include "mem.h"
#include "process.h"
#include "stat.h"
#include "systeminfo.h"

// Holds all the parsers for obtaining the monitor data
class Collector {
   private:
    StatParser stats;
    MemoryParser mem_stats;
    StaticSystemStats static_info;
    DynamicSystemStats dynamic_info;
    Processes processes;

    // Need to use std::atomic<bool> because the main thread which calls the collectors constructor
    // will mocify this variable so thread safty is required
    std::atomic<bool> running_thread{false};

    // ensures only one thread can access current data at a time
    mutable std::mutex data_mutex;
    std::shared_ptr<const StatisticsData> current_data;

    // Thread that continously collects data
    std::thread collector_thread;

   public:
    Collector();
    ~Collector();
    void run_thread();
    StatisticsData collect_data();
    std::shared_ptr<const StatisticsData> get_monitor_data() const;
};

#endif