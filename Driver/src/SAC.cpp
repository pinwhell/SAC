#include <Kernel/Task.h>
#include <Kernel/Log.h>
#include <Kernel/Memory.h>
#include <SAC/Drv/SAC.h>

SAC::SAC()
	: mPackages(new etl::unordered_set<etl::string<32>, 128>())
	, mAppStates(new etl::unordered_map<etl::string<32>, etl::unique_ptr<AppState>, 128>())
{}

SAC::~SAC()
{
	for (auto& pkg : *mPackages)
	{
		AppState* state = AppStateFromComm(pkg.c_str());
		if (!state) continue;
		state->mbDisabled = true;
		if (!state->mbHolding) continue;
		state->ReleaseHold();
		KLOG_PRINT("SAC Releasing %s", state->mFullPackageName.c_str());
		while (state->mbHolding) 
			TaskCurrentYield();
	}
}

const char* SAC::PackageFromComm(const char* comm)
{
	for (const auto& package : *mPackages)
		if (TaskCommCompare(comm, package.c_str()))
			return package.c_str();

	return nullptr;
}

AppState* SAC::AppStateFromComm(const char* comm)
{
	const char* package = PackageFromComm(comm);

	if (!package)
		return nullptr;

	auto& appStates = *mAppStates;

	if (appStates.find(package) == appStates.end())
		return nullptr;

	return appStates[package].get();
}

void SAC::Register(const char* packageName)
{
	mPackages->insert(packageName);
	(*mAppStates)[packageName] = etl::unique_ptr<AppState>(
		new AppState(
			packageName
		)
	);
}

void SAC::Remove(const char* packageName)
{
	AppState* state = AppStateFromComm(packageName);

	if (!state)
		return;

	state->Reset();
	state->mbDisabled = true;
}

SAC& SAC::Instance()
{
	static SAC sac;
	return sac;
}

int SACHandleCmd(ESACCmd cmd, void* arg)
{
	auto procComm = TaskCommGet(ProcessCurrentGet());
	auto threadComm = TaskCommGet(TaskCurrentGet());

	switch (cmd)
	{
	case ESACCmd::PACKAGE_ADD:
	{
		auto pkgName = MemoryFromUserString<4096>((const char __user*)arg);
		SAC::Instance().Register(pkgName->c_str());
		KLOG_PRINT("%s:%s:Registered %s", procComm.c_str(), threadComm.c_str(), pkgName->c_str());
		break;
	}

	case ESACCmd::PACKAGE_HOLD_RELEASE:
	{
		auto pkgName = MemoryFromUserString<4096>((const char __user*)arg);
		AppState* app = SAC::Instance().AppStateFromComm(pkgName->c_str());
		if (!app) return -ESRCH;
		if (!app->mbWantHold) return -EALREADY;
		if (!app->mbHolding) return -EAGAIN;
		app->ReleaseHold();
		break;
	}

	case ESACCmd::PACKAGE_REMOVE:
	{
		auto pkgName = MemoryFromUserString<4096>((const char __user*)arg);
		SAC::Instance().Remove(pkgName->c_str());
		KLOG_PRINT("%s:%s:Removed %s", procComm.c_str(), threadComm.c_str(), pkgName->c_str());
		break;
	}
	default:
		return -ENOTTY;
	}

	return 0;
}