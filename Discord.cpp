// DiscordLogger.cpp
#include "Discord.h"
#include <windows.h>
#include <string>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <C:\Users\charleslb10000\source\repos\ProcessMonitor\cpr\curl\curl.h>
#include <algorithm>
#include <windows.h> 
#include <psapi.h> 


std::string GetHWID() {
    std::string hwid;
    char buffer[128];

    FILE* pipe = _popen("wmic cpu get ProcessorId", "r");
    if (pipe) {
        while (fgets(buffer, sizeof(buffer), pipe)) {
            if (strlen(buffer) > 0) {
                hwid += buffer;
            }
        }
        _pclose(pipe);
    }

   
    pipe = _popen("wmic diskdrive get SerialNumber", "r");
    if (pipe) {
        while (fgets(buffer, sizeof(buffer), pipe)) {
            if (strlen(buffer) > 0) {
                hwid += buffer;
            }
        }
        _pclose(pipe);
    }

    
    hwid.erase(remove_if(hwid.begin(), hwid.end(), isspace), hwid.end());
    return hwid;
}




std::string GetComputerName()
{
    char computerName[MAX_COMPUTERNAME_LENGTH + 1];
    DWORD size = sizeof(computerName);
    if (GetComputerNameA(computerName, &size))
    {
        return std::string(computerName);
    }
    return "Unknown"; 
}



std::string GetProcessName()
{
    DWORD processID = GetCurrentProcessId();
    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processID);

    if (hProcess)
    {
        char processName[MAX_PATH];
        HMODULE hModule;
        if (EnumProcessModules(hProcess, &hModule, sizeof(hModule), NULL))
        {
            GetModuleFileNameExA(hProcess, hModule, processName, sizeof(processName) / sizeof(char));
            CloseHandle(hProcess);
            return std::string(processName);
        }
        CloseHandle(hProcess);
    }
    return "Fortnite.exe"; 
}

std::string webhookUrl = "put your webhook here";

void SendLogToDiscord(const std::string& message) {
    CURL* curl;
    CURLcode res;

    curl_global_init(CURL_GLOBAL_ALL);
    curl = curl_easy_init();

    if (curl) {
        std::string hwid = GetHWID();        
        
        std::string jsonPayload =
            "{\"username\": \"ProcessMonitor\", "
            "\"content\": \"ProcessMonitor Report\", "
            "\"embeds\": [{"
            "\"title\": \"Hello\", "
            "\"description\": \"Our ProcessMonitor has detected a user cheating.\", "
            "\"fields\": ["
            "{"
            "\"name\": \"HWID\", "
            "\"value\": \"" + hwid + "\", "
            "\"inline\": false"
            "},"
            "{"
            "\"name\": \"Computer Name\", "
            "\"value\": \"" + GetComputerName() + "\", "
            "\"inline\": false"
            "},"
            "{"
            "\"name\": \"Process Name\", "
            "\"value\": \"" + GetProcessName() + "\", "
            "\"inline\": false"
            "}"
            "]"
            "}]"
            "}";
            "}]}";

        curl_easy_setopt(curl, CURLOPT_URL, webhookUrl.c_str());
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, jsonPayload.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, curl_slist_append(NULL, "Content-Type: application/json"));

        res = curl_easy_perform(curl);

        if (res != CURLE_OK) {
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << "\n";
        }

        curl_easy_cleanup(curl);
    }
    curl_global_cleanup();
}

void CheckAndTerminateUnauthorizedProcesses() {
   
    std::cout << "Checking for unauthorized processes...\n";

    
    std::string logMessage = "Unauthorized process detected";
    SendLogToDiscord(logMessage);
}
