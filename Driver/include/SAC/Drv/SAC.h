#pragma once

#include <etl/memory.h>
#include <etl/string.h>
#include <etl/unordered_set.h>
#include <etl/unordered_map.h>
#include <SAC/Drv/AppState.h>
#include <SAC/SAC.h>

class SAC {
public:
	SAC();
	~SAC();

	const char* PackageFromComm(const char* comm);
	AppState* AppStateFromComm(const char* comm);
	void Register(const char* packageName);
	void Remove(const char* packageName);
	static SAC& Instance();

	etl::unique_ptr<etl::unordered_set<etl::string<32>, 128>> mPackages;
	etl::unique_ptr<etl::unordered_map<etl::string<32>, etl::unique_ptr<AppState>, 128>> mAppStates;
};

int SACHandleCmd(ESACCmd cmd, void* arg);