#include <stdio.h>
#include <Windows.h>
#include <string.h>
#include <conio.h>
#include <sysinfoapi.h>

void ConvSysTimeToFileTime(SYSTEMTIME *server_st,SYSTEMTIME *client_st,
						   FILETIME *server_ft, FILETIME *client_ft) {
	return;
}

int main() {
	SYSTEMTIME server_st, client_st;
	GetSystemTime(&server_st);
	HANDLE hMailSlot;
	char chBuffer[256], tmp[256];
	DWORD dwBytesRead;
	GetSystemTime(&server_st);
	
	/*printf("%d.%d.%d -- %d.%d", server_sys_time.wYear,
					   server_sys_time.wMonth,
		               server_sys_time.wDay,
					   server_sys_time.wHour,
					   server_sys_time.wSecond);*/
	return 1;
}