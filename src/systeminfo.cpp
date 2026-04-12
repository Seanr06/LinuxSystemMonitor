#include <fstream>
#include <string>
#include <sstream>

#include "proc_paths.h"
#include "systeminfo.h"

// kernel info - /proc/version
// OS Name - /etc/os-release
StaticSystemStats::StaticSystemStats()
{
    std::ifstream kernelFile(ProcPaths::kVersion);
    std::ifstream OSFile(ProcPaths::kOSRelease);
    kernelInfo = "";
    osInfo = "";

    if (kernelFile.is_open())
    {
        std::string line;
        std::string linuxWord;
        std::getline(kernelFile, line);
        std::stringstream scanner(line);

        scanner >> linuxWord >> kernelInfo;
        kernelFile.close();
    }
    if (OSFile.is_open())
    {
        std::string line;
        std::getline(OSFile, line);
        int firstQuote = line.find('"') + 1;
        int lastQuote = line.find('"', firstQuote);

        osInfo = line.substr(firstQuote, lastQuote - firstQuote);
        OSFile.close();
    }
}

std::string StaticSystemStats::getKernelInfo() const
{
    return kernelInfo;
}

std::string StaticSystemStats::getOSinfo() const
{
    return osInfo;
}
