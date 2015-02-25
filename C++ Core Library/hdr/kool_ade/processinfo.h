/****************************************************************
**
**	processinfo.h	- 
**
**	Copyright 2001 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.5 $
**
****************************************************************/

#if !defined( IN_PROCESSINFO_H )
#define IN_PROCESSINFO_H

// This is needed to get the number of clock ticks per second
// in a platform independent manner (almost)...
#ifdef WIN32
#include <time.h>
#define KA_CLOCKS_PER_SEC CLOCKS_PER_SEC
#else
#include <unistd.h> // For sysconf.
#define KA_CLOCKS_PER_SEC ( ( double ) sysconf( _SC_CLK_TCK ) )
#endif

// Structure for GetProcessMemoryInfo()
// Very similar to the one defined in psapi.h

typedef struct GS_PROCESS_MEMORY_COUNTERS {     
	unsigned long PageFaultCount;     
	unsigned long PeakWorkingSetSize;     
	unsigned long WorkingSetSize;     
	unsigned long QuotaPeakPagedPoolUsage;     
	unsigned long QuotaPagedPoolUsage;     
	unsigned long QuotaPeakNonPagedPoolUsage;     
	unsigned long QuotaNonPagedPoolUsage;     
	unsigned long PagefileUsage;     
	unsigned long PeakPagefileUsage; 
	} GS_PROCESS_MEMORY_COUNTERS; 

DLLEXPORT int ProcessMemoryInfoGet( GS_PROCESS_MEMORY_COUNTERS *Counters );

DLLEXPORT unsigned long ProcessWorkingSetSizeGet();

DLLEXPORT double ProcessCpuTimeGet();

DLLEXPORT unsigned long ProcessIdGet();

#endif
