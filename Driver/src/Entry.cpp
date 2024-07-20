#include <Kernel/Log.h>
#include <cstdint>
#include <Hooks.h>

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
	SACHooksInstall();
	return true;
}

void DriverShootdown()
{
	SACHooksUninstall();
	SACPrintUnloaded();
}