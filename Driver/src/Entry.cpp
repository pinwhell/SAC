#include <Kernel/Log.h>

#include <cstdint>

void SACPrintLoaded(uintptr_t entry)
{
	KLOG_PRINT("--- SAC Loaded --- " PTRFMT, entry);
	KLOG_PRINT("--- SAC Loaded --- " PTRFMT, entry);
	KLOG_PRINT("--- SAC Loaded --- " PTRFMT, entry);
}

void SACPrintUnloaded()
{
	KLOG_PRINT("||| SAC Unloaded |||");
	KLOG_PRINT("||| SAC Unloaded |||");
	KLOG_PRINT("||| SAC Unloaded |||");
}

bool DriverMain(uintptr_t entry)
{
	SACPrintLoaded(entry);
	return true;
}

void DriverShootdown()
{
	SACPrintUnloaded();
}