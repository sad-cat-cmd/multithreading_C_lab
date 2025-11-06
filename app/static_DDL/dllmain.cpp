#include "pch.h"

#pragma data_seg(".datashr")
DWORD dwProcessesAttached = 0;
DWORD dwAllThreadsAttached = 0;
#pragma data_seg()
#pragma comment(linker, "/SECTION:.datashr,RWS")

DWORD dwThreadsAttached = 0;

BOOL APIENTRY DllMain(HANDLE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		++dwProcessesAttached;
		++dwAllThreadsAttached;
		++dwThreadsAttached;
		break;

	case DLL_THREAD_ATTACH:
		++dwAllThreadsAttached;
		++dwThreadsAttached;
		break;

	case DLL_THREAD_DETACH:
		--dwAllThreadsAttached;
		--dwThreadsAttached;
		break;

	case DLL_PROCESS_DETACH:
		--dwProcessesAttached;
		--dwAllThreadsAttached;
		--dwThreadsAttached;
		break;
	}
	return TRUE;
}

