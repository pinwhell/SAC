#include <Kernel/Chrono.h>
#include <Kernel/Task.h>
#include <Kernel/Log.h>
#include <Kernel/Memory.h>
#include <Kernel/Syscalls.h>
#include <Kernel/UniSTD_x86.h>
#include <Hooks.h>

bool StringEndWith(const char* str, const char* with)
{
	auto strLen = fslc_strlen(str);
	auto withLen = fslc_strlen(with);

	if (withLen > strLen)
		return false;

	return !fslc_strcmp(str + strLen - withLen, with);
}

bool CommCompare(const char* comm, const char* with)
{
	auto commLen = fslc_strlen(comm);
	auto withLen = fslc_strlen(with);
	auto shorter = commLen > withLen ? with : comm;
	auto larger = comm == shorter ? with : comm;

	// a partial comparation need to be done, becouse
	// of the fact that comm just have 16 chars

	return fslc_strstr(larger, shorter) != nullptr;
}


REGPARAMDECL(long) do_sys_open(int dfd, const char __user* filename, int flags, unsigned short mode);
long (KERN_CALL* odo_sys_open)(int dfd, const char __user* filename, int flags, unsigned short mode);
long KERN_CALL hdo_sys_open(int dfd, const char __user* filename, int flags, unsigned short mode)
{
	const auto orig = [&] {
		return odo_sys_open(dfd, filename, flags, mode);
		};

	if (!filename)
		return orig();

	// At this point Filename not nullptr!

	long fd = orig();

	if (fd < 0)
		return fd;

	// At this point, the file exist

	auto procComm = TaskCommGet(ProcessCurrentGet());
	auto filenm = MemoryFromUserString<4096>(filename);

	if (!CommCompare(procComm.c_str(), "com.example.hookme") ||
		!(StringEndWith(filenm->c_str(), ".apk") && fslc_strstr(filenm->c_str(), "com.example.hookme")))
		return fd;

	// At this point, path is the wanted package .apk
	
	auto threadComm = TaskCommGet(TaskCurrentGet());

	KLOG_PRINT("%s:%s:Open -> %s", procComm.c_str(), threadComm.c_str(), filenm->c_str());

	return fd;
}

void SACHooksInstall()
{
	HOOKTRAMP_INSTALL(do_sys_open, hdo_sys_open, &odo_sys_open);
}

void SACHooksUninstall()
{
	HOOKTRAMP_RESTORE(do_sys_open, odo_sys_open);
}