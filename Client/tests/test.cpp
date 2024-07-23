#include <cstdint>
#include <errno.h>
#include <stdio.h>
#include <SAC/SAC.h>
#include <sys/ioctl.h>

template<ESACCmd cmd, typename TArg>
int SACCmdInvoke(TArg& arg)
{
	return ioctl((int)SACFD, (int)cmd, (uintptr_t)arg);
}

int main()
{
	if (SACCmdInvoke<ESACCmd::PACKAGE_ADD>("com.example.hookme"))
		perror("SACCmdInvoke(ESACCmd::PACKAGE_ADD)");

	getc(stdin);

	if (SACCmdInvoke<ESACCmd::PACKAGE_HOLD_RELEASE>("com.example.hookme"))
		perror("SACCmdInvoke(ESACCmd::PACKAGE_HOLD_RELEASE)");
}