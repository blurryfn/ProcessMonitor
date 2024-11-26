
#include "ProcessMonitor.h"
#include <windows.h>
#include <tlhelp32.h>
#include <iostream>
#include <thread>
#include <algorithm>
#include "Discord.h"



std::vector<std::string> unauthorizedProcesses = { "fortmp.exe", "astro.exe", "chrome.exe"};


std::atomic<bool> monitoringActive = true;

void AttachConsoleLogging() {
    AllocConsole();
    FILE* fp;
    freopen_s(&fp, "CONOUT$", "w", stdout);
    freopen_s(&fp, "CONOUT$", "w", stderr);
    std::cout << "Logging started\n";
}


bool CaseInsensitiveCompare(const std::string& str1, const std::string& str2) {
    return std::equal(str1.begin(), str1.end(), str2.begin(),
        [](unsigned char c1, unsigned char c2) { return tolower(c1) == tolower(c2); });
}


bool TerminateUnauthorizedProcess(DWORD processID, const std::string& processName) {
    HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, processID);
    if (hProcess == NULL) {
        std::cerr << "Failed to open process: " << processName << " (Error: " << GetLastError() << ")\n";
        return false;
    }

    if (processID <= 4) {
        std::cerr << "system process: " << processName << "\n";
        CloseHandle(hProcess);
        return false;
    }

    if (TerminateProcess(hProcess, 0)) {
        std::cout << "Terminated process: " << processName << "\n";
        CloseHandle(hProcess);
        return true;
    }
    else {
        std::cerr << "Failed to terminate process: " << processName << " (Error: " << GetLastError() << ")\n";
        CloseHandle(hProcess);
        return false;
    }
}

void CloseHostApplication() {
    std::cout << "Sorry, We Have detected something,. Closing application in 15 secs \n";
    Sleep(10000);
    HANDLE hCurrentProcess = GetCurrentProcess();
    TerminateProcess(hCurrentProcess, 0);
}


bool CheckAndTerminateUnauthorizedProcesses() {
    HANDLE hProcessSnap;
    PROCESSENTRY32 pe32;
    hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    if (hProcessSnap == INVALID_HANDLE_VALUE) {
        std::cerr << "Failed to create process snapshot (Error: " << GetLastError() << ")\n";
        return false;
    }

    pe32.dwSize = sizeof(PROCESSENTRY32);

    if (!Process32First(hProcessSnap, &pe32)) {
        std::cerr << "Failed to retrieve process info (Error: " << GetLastError() << ")\n";
        CloseHandle(hProcessSnap);
        return false;
    }

    do {
        std::wstring ws(pe32.szExeFile);
        std::string processName(ws.begin(), ws.end());

        for (const auto& forbiddenProcess : unauthorizedProcesses) {
            if (CaseInsensitiveCompare(processName, forbiddenProcess)) {
                std::cout << "Unauthorized process found: " << processName << "\n";


                std::string hwid = GetHWID();
                std::string logMessage = "Unauthorized process detected: " + processName + "\nHWID: " + hwid;
                SendLogToDiscord(logMessage);

                TerminateUnauthorizedProcess(pe32.th32ProcessID, processName);
                CloseHostApplication();
                CloseHandle(hProcessSnap);
                return true;
            }
        }
    } while (Process32Next(hProcessSnap, &pe32));

    CloseHandle(hProcessSnap);
    return false;
}

void MonitorProcesses() {
    while (monitoringActive) {
        if (CheckAndTerminateUnauthorizedProcesses()) {

        }
        Sleep(100);
    }
}


