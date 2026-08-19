#include "systeminfo.h"

#include <sstream>

#include "proc_paths.h"

// kernel info - /proc/version
// OS Name - /etc/os-release

// Constructor initializing fields in StaticSystemStats by parsing once
StaticSystemStats::StaticSystemStats() {
    // Initialize and open /proc/version and etc/os-release
    std::ifstream kernel_file(ProcPaths::kVersion);
    std::ifstream os_file(ProcPaths::kOSRelease);
    kernel_info = "";
    os_info = "";

    // Use a scanner to grab "linux" -> "version" -> then the actual kernel version
    if (kernel_file.is_open()) {
        std::string line;
        std::string linux_word;
        std::string version_word;
        std::getline(kernel_file, line);
        std::stringstream scanner(line);

        scanner >> linux_word >> version_word >> kernel_info;
        kernel_file.close();
    }
    // Obtain the PRETTY_NAME which is surrounded by quotes in the first line
    if (os_file.is_open()) {
        std::string line;
        std::getline(os_file, line);
        int first_quote = line.find('"') + 1;
        int last_quote = line.find('"', first_quote);

        os_info = line.substr(first_quote, last_quote - first_quote);
        os_file.close();
    }
}

// Getter methods for kernel and os info
std::string StaticSystemStats::get_kernel_info() const { return kernel_info; }

std::string StaticSystemStats::get_os_info() const { return os_info; }

// Constructor initializing fields in DynamicSystemStats that change
DynamicSystemStats::DynamicSystemStats() {
    last_minute_load = 0;
    last_five_minutes_load = 0;
    last_fifteen_minutes_load = 0;
    up_time = 0;
    idletime = 0;

    update_system_info();
}

void DynamicSystemStats::update_system_info() {
    // open /proc/uptime and and parse to obtain uptime and idletime
    uptime_file.open(ProcPaths::kUptime);
    if (uptime_file.is_open()) {
        std::string line;
        std::getline(uptime_file, line);
        std::stringstream scanner(line);
        scanner >> up_time >> idletime;
        uptime_file.close();
    }

    // open /proc/uptime and and parse to obtain load times
    load_file.open(ProcPaths::kLoadAVG);
    if (load_file.is_open()) {
        std::string line;
        std::getline(load_file, line);
        std::stringstream scanner(line);
        scanner >> last_minute_load >> last_five_minutes_load >> last_fifteen_minutes_load;
        load_file.close();
    }
}

// Getter methods for all DynamicSystemStats fields
float DynamicSystemStats::get_last_minute_load() const { return last_minute_load; }

float DynamicSystemStats::get_last_five_minutes_load() const { return last_five_minutes_load; }

float DynamicSystemStats::get_last_fifteen_minutes_load() const {
    return last_fifteen_minutes_load;
}

float DynamicSystemStats::get_uptime() const { return up_time; }

float DynamicSystemStats::get_idletime() const { return idletime; }