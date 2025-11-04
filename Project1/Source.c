#include <stdio.h>
#include <Windows.h>
#include <string.h>
#include <conio.h>
#include <sysinfoapi.h>

unsigned int tiks_in_second = 10000000;
LPCWSTR SlotName = TEXT("\\\\.\\mailslot\\network_timesync");
//LPCWSTR SlotName = TEXT("\\\\.\\mailslot\\network_timesync");
size_t MaxCountMsg = 30;
LARGE_INTEGER dueTime;
LONGLONG time_timer = -100000000;

BOOL check_status_MailSlot(HANDLE hMailSlot) {
	BOOL return_code;
	DWORD next_size, msg_number;
	return_code = GetMailslotInfo(hMailSlot, NULL, &next_size, &msg_number, NULL);
	if (return_code == FALSE) {
		printf("error GETMAILSLOTINFO\n");
		return TRUE;
	}
	if (next_size == MAILSLOT_NO_MESSAGE) {
		//printf("No messages \n");
		return TRUE;
	}
	return FALSE;
}

HANDLE WINAPI MakeSlot() {
	HANDLE hMailSlot = CreateMailslot(SlotName, 0, MAILSLOT_WAIT_FOREVER, (LPSECURITY_ATTRIBUTES) NULL);
	if (hMailSlot == INVALID_HANDLE_VALUE) {
		printf("CreateMailSlot failed with %d\n", GetLastError());
		return NULL;
	}
	return hMailSlot;
}

//HANDLE WINAPI MakeSlot() {
//	SECURITY_DESCRIPTOR sd;
//	InitializeSecurityDescriptor(&sd, SECURITY_DESCRIPTOR_REVISION);
//	SetSecurityDescriptorDacl(&sd, TRUE, NULL, FALSE);
//
//	SECURITY_ATTRIBUTES sa;
//	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
//	sa.lpSecurityDescriptor = &sd;
//	sa.bInheritHandle = FALSE;
//
//	HANDLE hMailSlot = CreateMailslot(SlotName,
//		0,
//		MAILSLOT_WAIT_FOREVER,
//		&sa);
//
//	if (hMailSlot == INVALID_HANDLE_VALUE) {
//		printf("CreateMailslot failed with error: %d\n", GetLastError());
//		return NULL;
//	}
//
//	printf("Server: Mailslot created successfully\n");
//	printf("Server: Waiting for connections at: %ls\n", SlotName);
//	return hMailSlot;
//}

BOOL get_client_SystemTime(HANDLE hMailSlot, SYSTEMTIME *client_st) {	
	DWORD bytes_read;
	BOOL success;
	success = ReadFile(hMailSlot, client_st, sizeof(SYSTEMTIME), &bytes_read, NULL);

	if (!success) {
		printf("ReadFile failed: %d\n", GetLastError());
		return TRUE;
	}
	if (bytes_read != sizeof(SYSTEMTIME)) {
		printf("Invalid data size: received %d bytes, expected %zu\n",
			bytes_read, sizeof(SYSTEMTIME));
		return TRUE;
	}
	return FALSE;
}

inline ULARGE_INTEGER convert_ST_in_UI(SYSTEMTIME *ST) {
	FILETIME FT;
	ULARGE_INTEGER UI;
	SystemTimeToFileTime(ST, &FT);
	memcpy(&UI, &FT, sizeof(FT));
	return UI;
}

inline ULONGLONG Differt_UI_in_sec(ULARGE_INTEGER *server_UI, ULARGE_INTEGER *client_UI) {
	ULONGLONG diff;
	if (server_UI->QuadPart > client_UI->QuadPart) 
		diff = server_UI->QuadPart - client_UI->QuadPart;
	else 
		diff = client_UI->QuadPart - server_UI->QuadPart;
	return diff / (ULONGLONG)tiks_in_second;
}

inline void print_info(ULONGLONG diff) {
	SYSTEMTIME server_LT;
	GetLocalTime(&server_LT);
	printf("%02d:%02d:%02d --- time difference of two pcs: %llu \n",server_LT.wHour, server_LT.wMinute,server_LT.wSecond, diff);
}

int main() {
	HANDLE hMailSlot, hTimer;
	hMailSlot = MakeSlot();
	if (hMailSlot == NULL) {
		printf("Error create MailSlot");
		return 1;
	}
	hTimer = CreateWaitableTimer(NULL,FALSE,NULL);
	if (hTimer == NULL) {
		printf("CreateWaitableTimer failed: %d\n", GetLastError());
		return 1;
	}
	dueTime.QuadPart = time_timer;
	if (!SetWaitableTimer(hTimer, &dueTime,0, NULL, NULL, FALSE)) {
		printf("SetWaitableTimer failed: %d\n", GetLastError());
		return 1;
	}
	SYSTEMTIME server_st, client_st;
	ULARGE_INTEGER server_ui, client_ui;
	BOOL status_work = TRUE;
	size_t count_msg = 0;

	while (count_msg < MaxCountMsg) {
		/*if (WaitForSingleObject(hTimer, 0) == WAIT_OBJECT_0) {
			printf("!!! TIMEOUT: No messages received within 10 seconds !!!\n");
			printf("Server stopping. Received: %zu/%zu messages\n", count_msg, MaxCountMsg);
			break;
		}*/
		if (!check_status_MailSlot(hMailSlot)) {
			if (!get_client_SystemTime(hMailSlot, &client_st)) {
				GetSystemTime(&server_st);
				server_ui = convert_ST_in_UI(&server_st);
				client_ui = convert_ST_in_UI(&client_st);
				print_info(Differt_UI_in_sec(&server_ui, &client_ui));
				count_msg++;
			}
			else
				return 1;
		}
		else
			Sleep(20);
	}

	CloseHandle(hTimer);
	CloseHandle(hMailSlot);
	return 0;
}