#pragma once

#define SACFD 0xF1AAAAF1ul

enum class ESACCmd : unsigned int {
	PACKAGE_ADD,
	PACKAGE_STATE,
	PACKAGE_REMOVE,
	PACKAGE_HOLD_RELEASE
};