#include <windows.h>
#include <iostream>
#include <thread>
#include "ProcessMonitor.h"





BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    static std::thread monitoringThread;

    switch (ul_reason_for_call) {
    case DLL_PROCESS_ATTACH:
        AttachConsoleLogging();
        std::cout << "DLL Loaded Starting process monitoring\n";


        monitoringActive = true;
        monitoringThread = std::thread(MonitorProcesses);
        break;

    case DLL_PROCESS_DETACH:

        monitoringActive = false;
        if (monitoringThread.joinable()) {
            monitoringThread.join();
        }
        std::cout << "DLL Unloaded Stopping process monitoring\n";
        break;

    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
        break;
    }
    return TRUE;
}