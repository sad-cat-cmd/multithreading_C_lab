#include <stdio.h>
#include <Windows.h>

TCHAR MUTEX_NAME[] = TEXT("mutex_click_status");

void GetMaxXY(int* x_pos, int* y_pos) {
	RECT desktopRect;
	SystemParametersInfo(SPI_GETWORKAREA, 0, &desktopRect, 0);
	*x_pos = desktopRect.right - desktopRect.left;
	*y_pos = desktopRect.bottom - desktopRect.top;
}

typedef BOOL(*P_Install_Mouse_Hook)();
typedef BOOL(*P_Remove_Mouse_Hook)();
typedef BOOL(*P_Get_Click_Status)();
LONGLONG time_timer = -100000000;
TCHAR lpFileShareName[] = TEXT("$MyVerySpecialFileShareName$");

int screenWidth = 0;
int screenHeight = 0;

typedef struct {
	POINT pt;
	BOOL data_ready;
	BOOL status_click;
} SharedData;

BOOL click_status;

size_t count_lusky_ckick = 0;
size_t MaxCountClick = 15;
//BOOL click_status;
int main() {
	P_Install_Mouse_Hook p_installMouseHook;
	P_Remove_Mouse_Hook p_removeMouseHook;
	P_Get_Click_Status p_Get_Click_Status;
	HMODULE hDll;
	LARGE_INTEGER dueTime;
	HANDLE hTimer, hMutex, hMapFile;

	hDll = LoadLibrary(TEXT("hookDLLdll"));
	if (!hDll) {
		printf("Error loading dll\n");
		return 1;
	}
	p_installMouseHook = (P_Install_Mouse_Hook)GetProcAddress(hDll, "installMouseHook");
	p_removeMouseHook = (P_Remove_Mouse_Hook)GetProcAddress(hDll, "removeMouseHook");
	p_Get_Click_Status = (P_Get_Click_Status)GetProcAddress(hDll, "get_click_status");

	hMapFile = CreateFileMapping(INVALID_HANDLE_VALUE,
		NULL,
		PAGE_READWRITE,
		0,
		1024,
		lpFileShareName);
	if (hMapFile == NULL) {
		printf("Error: create FileMapping\n");
		return 1;
	}

	SharedData* shared_data = (SharedData*)MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, 1024);
	if (shared_data == NULL) {
		printf("Error: MapWievOfFile in main process %lu \n", GetLastError());
		return 1;
	}
	hTimer = CreateWaitableTimer(NULL, FALSE, NULL);
	if (hTimer == NULL) {
		printf("CreateWaitableTimer failed: %d\n", GetLastError());
		return 1;
	}
	dueTime.QuadPart = time_timer;
	if (!SetWaitableTimer(hTimer, &dueTime, 0, NULL, NULL, FALSE)) {
		printf("SetWaitableTimer failed: %d\n", GetLastError());
		return 1;
	}

	hMutex = CreateMutex(NULL, FALSE, MUTEX_NAME);
	if (hMutex == NULL)
	{
		printf("CreateMutex error: %d\n", GetLastError());
		return 1;
	}
	if (!p_installMouseHook())
		printf("install hook\n");
	else
		printf("failed to install hook\n");

	GetMaxXY(&screenWidth, &screenHeight);

	while (count_lusky_ckick < MaxCountClick) {
		if (WaitForSingleObject(hTimer, 0) == WAIT_OBJECT_0) {
			printf("!!! TIMEOUT: 20 sec running!!!\n");
			break;
		}
		if (shared_data->data_ready) {
			WaitForSingleObject(hMutex, INFINITE);
			if (shared_data->pt.x > screenWidth / 2 && shared_data->pt.y > screenHeight / 2) {
				printf(" --- left click at the bottom right\n");
				shared_data->data_ready = FALSE;
				//printf();
			}
			ReleaseMutex(hMutex);
		}
		else
			Sleep(20);
		/*if (p_Get_Click_Status()) {
			printf(" --- left click at the bottom right\n");
			count_lusky_ckick++;
		}
		else
			Sleep(100);*/
	}

	/*DWORD result = WaitForSingleObject(hTimer, INFINITE);

	if (result == WAIT_OBJECT_0) {
		printf("!!! TIMEOUT: 20 seconds elapsed !!!\n");
	}*/

	if (!p_removeMouseHook()) {
		printf("Uninstall hook\n");
	}
	else {
		printf("Failed uninstall hook\n");
	}
	FreeLibrary(hDll);
	CloseHandle(hTimer);
	return 0;
}