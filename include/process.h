#include <filesystem>
#include <vector>
#include <string>

class Processes
{
private:
    std::vector<std::filesystem::path> pidPaths;
    std::vector<PID> pids;

public:
    bool isPID(const std::string &str);
    std::vector<std::filesystem::path> getProcessDirectories();
    void updatePIDs();
};

class PID
{
private:
    ParseProcessStat stats;
    ParseProccessStatus status;
    ParseProcessRun run;
    ParsefdDirectory fd;
    ParseProcessesIO io;
};

class ParseProcessStat
{
};

class ParseProccessStatus
{
};

class ParseProcessRun
{
};

class ParsefdDirectory
{
};

class ParseProcessesIO
{
};
