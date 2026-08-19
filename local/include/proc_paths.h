#ifndef PROC_PATHS_H
#define PROC_PATHS_H

#include <string>

// Use inline to tell other files to share these proc paths
namespace ProcPaths {
    inline constexpr const char* kProcStat = "/proc/stat";
    inline constexpr const char* kMemInfo = "/proc/meminfo";
    inline constexpr const char* kDiskStats = "/proc/diskstats";
    inline constexpr const char* kVersion = "/proc/version";
    inline constexpr const char* kOSRelease = "/etc/os-release";
    inline constexpr const char* kUptime = "/proc/uptime";
    inline constexpr const char* kLoadAVG = "/proc/loadavg";
}  // namespace ProcPaths

#endif