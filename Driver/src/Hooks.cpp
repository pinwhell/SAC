#include <Kernel/Chrono.h>
#include <Kernel/Task.h>
#include <Kernel/Log.h>
#include <Kernel/Memory.h>
#include <Kernel/Syscalls.h>
#include <Kernel/UniSTD_x86.h>
#include <Kernel/PtRegs.h>
#include <SAC/Drv/Hooks.h>
#include <SAC/Drv/SAC.h>
#include <SAC/SAC.h>

bool StringEndWith(const char* str, const char* with)
{
	auto strLen = fslc_strlen(str);
	auto withLen = fslc_strlen(with);

	if (withLen > strLen)
		return false;

	return !fslc_strcmp(str + strLen - withLen, with);
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

	auto appComm = TaskCommGet(ProcessCurrentGet());

	AppState* appState = SAC::Instance().AppStateFromComm(appComm.c_str());

	if(!appState)
		return fd;

	// Got app state for current app with appComm & app hasnt been processed yet

	auto filenm = MemoryFromUserString<4096>(filename);

	if (!StringEndWith(filenm->c_str(), ".apk") ||
		!fslc_strstr(filenm->c_str(), appState->mFullPackageName.c_str()) ||
		appState->AlreadyProcessed() ||
		appState->Disabled())
		return fd;

	// At this point, path is the wanted package .apk
	// and the startup hasnt been processed yet & is not disabled yet

	auto threadComm = TaskCommGet(TaskCurrentGet());

	appState->mbStartupProcessed = true;
	KLOG_PRINT("%s:%s:Open -> %s", appState->mFullPackageName.c_str(), threadComm.c_str(), filenm->c_str());
	appState->Hold();

	return fd;
}

REGPARAMDECL(void) __noreturn do_exit(long code);
void (__noreturn KERN_CALL* odo_exit)(long code);
void KERN_CALL hdo_exit(long code)
{
	Task proc = ProcessCurrentGet();
	Task thread = TaskCurrentGet();

	if (proc != thread)
		return odo_exit(code);

	auto appComm = TaskCommGet(proc);
	AppState* appState = SAC::Instance().AppStateFromComm(appComm.c_str());

	if(!appState)
		return odo_exit(code);		

	// At this point, the wanted Process is closing

	KLOG_PRINT("%s:Close", appState->mFullPackageName.c_str());
	appState->Reset();

	odo_exit(code);
}

int (asmlinkage* oioctl)(pt_regs* args);
asmlinkage int hioctl(pt_regs* args)
{
	if (SACFD == args->bx)
		return SACHandleCmd((ESACCmd)args->cx, (void*)args->dx);

	return oioctl(args);
}

void SACHooksInstall()
{
	static int(asmlinkage*oioctlStub)(unsigned int fd, unsigned int cmd, unsigned long arg);

	HOOKTRAMP_INSTALL(do_sys_open, hdo_sys_open, &odo_sys_open);
	HOOKTRAMP_INSTALL(do_exit, hdo_exit, &odo_exit);

	SYSCALL_REPLACE(__NR_ioctl, +[]  (unsigned int fd, unsigned int cmd, unsigned long arg) asmlinkage {
		pt_regs regs{};

		regs.bx = fd;
		regs.cx = cmd;
		regs.dx = arg;

		return hioctl(&regs);
		}, &oioctlStub);

	oioctl = [](pt_regs* args) asmlinkage {
		return oioctlStub(args->bx, args->cx, args->dx);
		};				  
}		  

void SACHooksUninstall()
{
	SYSCALL_RESTORE(__NR_ioctl);

	HOOKTRAMP_RESTORE(do_exit, odo_exit);
	HOOKTRAMP_RESTORE(do_sys_open, odo_sys_open);
}