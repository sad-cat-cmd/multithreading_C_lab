// static_DDL.cpp : Определяет экспортируемые функции для DLL.
//

#include "pch.h"
#include "framework.h"
#include "static_DDL.h"

//unsigned int tiks_in_second = 10000000;
unsigned int tiks_in_msecond = 10000;

STATICDDL_API BOOL get_client_SystemTime(HANDLE hMailSlot, SYSTEMTIME* client_st) {
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
STATICDDL_API ULARGE_INTEGER convert_ST_in_UI(SYSTEMTIME* ST) {
	FILETIME FT;
	ULARGE_INTEGER UI;
	SystemTimeToFileTime(ST, &FT);
	memcpy(&UI, &FT, sizeof(FT));
	return UI;
}
STATICDDL_API ULONGLONG Differt_UI_in_sec(ULARGE_INTEGER* server_UI, ULARGE_INTEGER* client_UI) {
	ULONGLONG diff;
	if (server_UI->QuadPart > client_UI->QuadPart)
		diff = server_UI->QuadPart - client_UI->QuadPart;
	else
		diff = client_UI->QuadPart - server_UI->QuadPart;
	return diff / (ULONGLONG)tiks_in_msecond;
}
