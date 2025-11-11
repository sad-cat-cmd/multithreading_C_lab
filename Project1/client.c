#include <stdio.h>
#include <Windows.h>
#include <string.h>
#include <conio.h>
#include <sysinfoapi.h>

unsigned int tiks_in_second = 10000000;
LPCTSTR SlotName = TEXT("\\\\DESKTOP-UQN8AN0\\mailslot\\network_timesync");
size_t MaxCountMsg = 30;

int main() {
	SYSTEMTIME client_st;
	HANDLE hMailSlot;
	DWORD bytesWritten;
	BOOL success;
	size_t count_msg = 0;
	
	hMailSlot = CreateFile(SlotName,
		GENERIC_WRITE,
		FILE_SHARE_READ,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);
	
	if (hMailSlot == INVALID_HANDLE_VALUE) {
		printf("ERROR: Opening with mailslot %d\n", GetLastError());
		return 1;
	}

	while (count_msg < MaxCountMsg) {
		GetSystemTime(&client_st);
		success = WriteFile(
			hMailSlot,
			&client_st,
			sizeof(SYSTEMTIME),
			&bytesWritten,
			NULL);
		if (success && bytesWritten == sizeof(SYSTEMTIME)) {
			printf("SystemTime is sent\n");
			count_msg++;
		}
		else printf("SystemTime is not sent\n");

		Sleep(1000);
	}

	CloseHandle(hMailSlot);
	return 1;
}