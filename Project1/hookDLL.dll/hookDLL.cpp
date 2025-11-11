#include "pch.h"
#include "framework.h"
#include "hookDLL.h"

TCHAR lpFileShareName[] = TEXT("$MyVerySpecialFileShareName$");
TCHAR MUTEX_NAME[] = TEXT("mutex_click_status");
BOOL click_Status = FALSE;
HANDLE hMutex;

HHOOK hMouseHook;
MOUSEHOOKSTRUCT* mouseInfo = NULL;
int screenWidth = 0;
int screenHeight = 0;
extern HINSTANCE dll_hInstance;
typedef struct {
	POINT pt;
	BOOL data_ready;
	BOOL status_click;
} SharedData;
SharedData* shared_data;
void GetMaxXY(int* x_pos, int* y_pos) {
	RECT desktopRect;
	SystemParametersInfo(SPI_GETWORKAREA, 0, &desktopRect, 0);
	*x_pos = desktopRect.right - desktopRect.left;
	*y_pos = desktopRect.bottom - desktopRect.top;
}
LRESULT CALLBACK MouseProc(int nCode, WPARAM wParam, LPARAM lParam) {
	LRESULT result = CallNextHookEx(hMouseHook, nCode, wParam, lParam);
	if (nCode < 0)
		return CallNextHookEx(hMouseHook, nCode, wParam, lParam);

	if (wParam == WM_LBUTTONDOWN) {
		//MOUSEHOOKSTRUCT* mouseInfo = (MOUSEHOOKSTRUCT*)lParam;
		MSLLHOOKSTRUCT* mouseInfo = (MSLLHOOKSTRUCT*)lParam;
		printf("DLL: Mouse click detected at %d, %d\n", mouseInfo->pt.x, mouseInfo->pt.y);
		//printf("Left click\n");
		// ПРАВИЛЬНАЯ структура для WH_MOUSE
		WaitForSingleObject(hMutex, INFINITE);
		shared_data->pt = mouseInfo->pt;
		shared_data->data_ready = TRUE;
		//MOUSEHOOKSTRUCT* mouseInfo = (MOUSEHOOKSTRUCT*)lParam;
		ReleaseMutex(hMutex);
		/*printf("DLL: Mouse click at %d, %d | Threshold: >%d, >%d\n",
			mouseInfo->pt.x, mouseInfo->pt.y,
			screenWidth / 2, screenHeight / 2);*/
		
		/*BOOL inRightHalf = (mouseInfo->pt.x > screenWidth / 2);
		BOOL inBottomHalf = (mouseInfo->pt.y > screenHeight / 2);*/

		/*if (inRightHalf && inBottomHalf) {
			printf("DLL: SUCCESS - Click in right-bottom area!\n");
			click_Status = TRUE;
		}*/
		/*else {
			printf("DLL: Click outside target area\n");
		}*/
	}
	return result;
	//return CallNextHookEx(hMouseHook, nCode, wParam, lParam);
}

HOOKDLLDLL_API BOOL installMouseHook() {
	hMutex = OpenMutex(MUTEX_ALL_ACCESS,FALSE, MUTEX_NAME);
	if (hMutex == NULL) {
		printf("Error: DDL is not open mutex\n");
		return 1;
	}
	HANDLE hMapFile = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, lpFileShareName);

	if (hMapFile == NULL) {
		printf("Client: ERROR OPEN FILE:\n");
		return 1;
	}

	shared_data = (SharedData*)MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, 1024);
	if (shared_data == NULL) {
		printf("Error: MapViewOfFile\n");
		CloseHandle(hMapFile);
		return 1;
	}
	GetMaxXY(&screenWidth,&screenHeight);
	//hMouseHook = SetWindowsHookEx(WH_MOUSE,MouseProc, dll_hInstance, 0);
	hMouseHook = SetWindowsHookEx(WH_MOUSE_LL, MouseProc, GetModuleHandle(NULL), 0);
	if (hMouseHook == NULL) {
		printf("Error: SetWindowsHookEx\n");
		return TRUE;
	}
	return FALSE;
}
HOOKDLLDLL_API BOOL removeMouseHook() {
	if (!UnhookWindowsHookEx(hMouseHook)) {
		printf("Error: UnhookWindowsHookEx\n");
		return TRUE;
	}
	return FALSE;
}

HOOKDLLDLL_API BOOL get_click_status() {
	if (click_Status) {
		click_Status = FALSE;
		return TRUE;
	}
	return FALSE;
}