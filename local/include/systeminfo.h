#ifndef SYSTEMINFO_H
#define SYSTEMINFO_H

#include <fstream>
#include <string>

#include "proc_paths.h"

using std::string;

// Static stats for kernel version and os-release name
class StaticSystemStats {
   private:
    string kernel_info;
    string os_info;

   public:
    StaticSystemStats();
    string get_kernel_info() const;
    string get_os_info() const;
};

// Dynamic Stats class for uptime and loadtime
class DynamicSystemStats {
   private:
    std::ifstream uptime_file;
    std::ifstream load_file;

    float last_minute_load;
    float last_five_minutes_load;
    float last_fifteen_minutes_load;

    float up_time;
    float idletime;

   public:
    DynamicSystemStats();
    void update_system_info();

    // Load avg
    float get_last_minute_load() const;
    float get_last_five_minutes_load() const;
    float get_last_fifteen_minutes_load() const;

    // uptime
    float get_uptime() const;
    float get_idletime() const;
};

#endif