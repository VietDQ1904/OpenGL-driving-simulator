#include <windows.h>
#include <iostream>
#include <psapi.h>
#pragma comment(lib, "psapi.lib")

// Note: This only works on Windows systems.
#ifndef SYSTEMS_INFO
#define SYSTEMS_INFO

inline double getRAMUsageMB()
{
   PROCESS_MEMORY_COUNTERS_EX pmc;
   GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS *)&pmc, sizeof(pmc));
   return pmc.WorkingSetSize / 1024.0 / 1024.0; // MB
}

inline double getTotalRAMMB()
{
   MEMORYSTATUSEX statex;
   statex.dwLength = sizeof(statex);
   GlobalMemoryStatusEx(&statex);
   return statex.ullTotalPhys / 1024.0 / 1024.0; // MB
}

inline double getProcessCPUUsage()
{
   static ULONGLONG lastSysTime = 0, lastUserTime = 0, lastTime = 0;
   static int numProcessors = 0;

   if (numProcessors == 0)
   {
      SYSTEM_INFO sysInfo;
      GetSystemInfo(&sysInfo);
      numProcessors = (int)sysInfo.dwNumberOfProcessors;
   }

   FILETIME ftSys, ftUser, ftCreation, ftExit;
   if (!GetProcessTimes(GetCurrentProcess(), &ftCreation, &ftExit, &ftSys, &ftUser))
   {
      return 0.0;
   }

   ULARGE_INTEGER sys, user;
   sys.LowPart = ftSys.dwLowDateTime;
   sys.HighPart = ftSys.dwHighDateTime;
   user.LowPart = ftUser.dwLowDateTime;
   user.HighPart = ftUser.dwHighDateTime;

   ULONGLONG nowSys = sys.QuadPart;
   ULONGLONG nowUser = user.QuadPart;

   FILETIME ftNow;
   GetSystemTimeAsFileTime(&ftNow);
   ULARGE_INTEGER now;
   now.LowPart = ftNow.dwLowDateTime;
   now.HighPart = ftNow.dwHighDateTime;

   if (lastTime == 0)
   {
      lastSysTime = nowSys;
      lastUserTime = nowUser;
      lastTime = now.QuadPart;
      return 0.0;
   }

   ULONGLONG sysDiff = nowSys - lastSysTime;
   ULONGLONG userDiff = nowUser - lastUserTime;
   ULONGLONG timeDiff = now.QuadPart - lastTime;

   lastSysTime = nowSys;
   lastUserTime = nowUser;
   lastTime = now.QuadPart;

   double cpuUsage = (100.0 * (sysDiff + userDiff) / timeDiff) / numProcessors;
   return cpuUsage; // %
}

#endif