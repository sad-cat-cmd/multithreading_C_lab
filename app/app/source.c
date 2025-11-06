#include <Windows.h>
#include "static_DDL.h"
#include <stdio.h>
#include <sysinfoapi.h>

//#pragma comment (lib, "staticDDL.lib") 

typedef HANDLE(WINAPI* P_CreateWaitTimer)();
typedef HANDLE(WINAPI* P_MakeSlot)();
typedef BOOL(WINAPI* P_check_status_MailSlot)(HANDLE);
typedef void (WINAPI* P_printf_info)(ULONGLONG);

P_CreateWaitTimer p_CreateWaitTimer;
P_MakeSlot p_MakeSlot;
P_check_status_MailSlot p_check_status_MailSlot;
P_printf_info p_printf_info;

size_t MaxCountMsg = 30;
LARGE_INTEGER dueTime;
LONGLONG time_timer = -100000000;

int main() {
	HMODULE hDll;
	HANDLE hMailSlot, hTimer;
	SYSTEMTIME server_st, client_st;
	ULARGE_INTEGER server_ui, client_ui;
	size_t count_msg = 0;

	hDll = LoadLibrary(TEXT("dynamicDDL.dll"));
	if (!hDll) {
		printf("Error loading dll");
		return 1;
	}

	p_CreateWaitTimer = (P_CreateWaitTimer)GetProcAddress(hDll, "?CreateWaitTimer@@YAPEAXXZ");
	p_MakeSlot = (P_MakeSlot)GetProcAddress(hDll, "?MakeSlot@@YAPEAXXZ");
	p_check_status_MailSlot = (P_check_status_MailSlot)GetProcAddress(hDll, "?check_status_MailSlot@@YAHPEAX@Z");
	p_printf_info = (P_printf_info)GetProcAddress(hDll, "?print_info@@YAX_K@Z");

	hMailSlot = p_MakeSlot();
	if (hMailSlot == NULL) {
		printf("Error create MailSlot\n");
		return 1;
	}
	hTimer = p_CreateWaitTimer();
	if (hTimer == NULL) {
		printf("Error create WaitTimer\n");
		return 1;
	}
	dueTime.QuadPart = time_timer;
	if (!SetWaitableTimer(hTimer, &dueTime, 0, NULL, NULL, FALSE)) {
		printf("SetWaitableTimer failed: %d\n", GetLastError());
		return 1;
	}

	while (count_msg < MaxCountMsg) {
		if (WaitForSingleObject(hTimer, 0) == WAIT_OBJECT_0) {
			printf("!!! TIMEOUT: No messages received within 10 seconds !!!\n");
			printf("Server stopping. Received: %zu/%zu messages\n", count_msg, MaxCountMsg);
			break;
		}
		if (!p_check_status_MailSlot(hMailSlot)) {
			if (!get_client_SystemTime(hMailSlot, &client_st)) {
				GetSystemTime(&server_st);
				server_ui = convert_ST_in_UI(&server_st);
				client_ui = convert_ST_in_UI(&client_st);
				p_printf_info(Differt_UI_in_sec(&server_ui, &client_ui));
				count_msg++;
				SetWaitableTimer(hTimer, &dueTime, 0, NULL, NULL, FALSE);
			}
			else
				return 1;
		}
		else
			Sleep(20);
	}

	CloseHandle(hTimer);
	CloseHandle(hMailSlot);
	FreeLibrary(hDll);
	return 0;
}