#pragma once

#include <etl/string.h>

struct AppState {
	AppState(const char* fullPackageName);

	void Reset();
	void Hold();
	void ReleaseHold();
	volatile bool AlreadyProcessed();
	volatile bool Disabled();

	volatile bool mbDisabled;
	volatile bool mbStartupProcessed;
	volatile bool mbWantHold;
	volatile bool mbHolding;
	etl::string<32> mFullPackageName;
};