#include <string>
#include <vector>
#include <atomic>


void AttachConsoleLogging();
std::string GetHWID();
void SendLogToDiscord(const std::string& message);
bool CheckAndTerminateUnauthorizedProcesses();
void MonitorProcesses();


extern std::vector<std::string> unauthorizedProcesses;
extern std::atomic<bool> monitoringActive;


