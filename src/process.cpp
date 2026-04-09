#include <filesystem>
#include <vector>
#include <string>
#include <cctype>

#include "process.h"

bool Processes::isPID(const std::string &str)
{
    if (str.empty())
        return false;
    for (char digit : str)
    {
        if (!std::isdigit(static_cast<unsigned char>(digit)))
            return false;
    }
    return true;
}

std::vector<std::filesystem::path> Processes::getProcessDirectories()
{
    pidPaths.clear();
    for (const auto &entry : std::filesystem::directory_iterator("/proc"))
    {
        std::string pid = entry.path().filename().string();
        if (isPID(pid))
            pidPaths.push_back(entry.path());
    }
}

void Processes::updatePIDs()
{
    for (std::filesystem::path file : pidPaths)
    {
    }
}