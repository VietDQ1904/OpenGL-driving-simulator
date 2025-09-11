#include <windows.h>
#include <psapi.h>
#pragma comment(lib, "psapi.lib")

#ifndef SYSTEMS_INFO
#define SYSTEM_INFO

inline double GetRAMUsageMB()
{
   PROCESS_MEMORY_COUNTERS_EX pmc;
   GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS *)&pmc, sizeof(pmc));
   return pmc.WorkingSetSize / 1024.0 / 1024.0; // MB
}

inline double GetTotalRAMMB()
{
   MEMORYSTATUSEX statex;
   statex.dwLength = sizeof(statex);
   GlobalMemoryStatusEx(&statex);
   return statex.ullTotalPhys / 1024.0 / 1024.0; // MB
}

#endif