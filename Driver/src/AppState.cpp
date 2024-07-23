#include <Kernel/Task.h>
#include <SAC/Drv/AppState.h>

AppState::AppState(const char* fullPackageName)
	: mFullPackageName(fullPackageName)
{
	Reset();
}

void AppState::Reset()
{
	mbStartupProcessed = false;
	mbWantHold = true;
	mbDisabled = false;
}

volatile bool AppState::AlreadyProcessed()
{
	return mbStartupProcessed;
}

volatile bool AppState::Disabled()
{
	return mbDisabled;
}

void AppState::Hold()
{
	mbHolding = true;

	while (mbWantHold)
		TaskCurrentYield();

	mbHolding = false;
}

void AppState::ReleaseHold()
{
	mbWantHold = false;
}