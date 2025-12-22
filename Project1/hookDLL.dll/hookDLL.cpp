#include "pch.h"
#include "framework.h"
#include "hookDLL.h"

HHOOK hMouseHook;
HHOOK keyboard_hook;

MSLLHOOKSTRUCT* mouseInfo = NULL;
int screenWidth = 0;
int screenHeight = 0;
extern HINSTANCE dll_hInstance;

void GetMaxXY(int* x_pos, int* y_pos) {
	RECT desktopRect;
	SystemParametersInfo(SPI_GETWORKAREA, 0, &desktopRect, 0);
	*x_pos = desktopRect.right - desktopRect.left;
	*y_pos = desktopRect.bottom - desktopRect.top;
}
LRESULT CALLBACK MouseProc(int nCode, WPARAM wParam, LPARAM lParam) {
	if (nCode < 0)
		return CallNextHookEx(hMouseHook, nCode, wParam, lParam);
	
	if (wParam == WM_LBUTTONDOWN) {
		mouseInfo = (MSLLHOOKSTRUCT*)lParam;
		if (mouseInfo->pt.x > screenWidth/2 && mouseInfo->pt.y > screenHeight/2) {
			MessageBox(NULL, L"Левый клик мыши в правой нижней зоне окна", L"Mouse Hook", MB_OK | MB_ICONINFORMATION);
		}
	}

	return CallNextHookEx(hMouseHook,nCode, wParam, lParam);
}

LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (nCode >= 0)
	{
		KBDLLHOOKSTRUCT* kbStruct = (KBDLLHOOKSTRUCT*)lParam;

		if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN)
		{
			if (kbStruct->vkCode == 0x30)
			{
				if (GetAsyncKeyState(VK_LSHIFT) & 0x8000 || GetAsyncKeyState(VK_RSHIFT) & 0x8000)
				{
					MessageBox(NULL, L"Была нажата комбинация Shift + 0", L"Keyboard Hook", MB_OK | MB_ICONINFORMATION);
				}
			}
		}

	}
	return CallNextHookEx(keyboard_hook, nCode, wParam, lParam);
}

HOOKDLLDLL_API BOOL installMouseHook() {
	
	GetMaxXY(&screenWidth,&screenHeight);
	//hMouseHook = SetWindowsHookEx(WH_MOUSE_LL, MouseProc, GetModuleHandle(NULL), 0);
	keyboard_hook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc, GetModuleHandle(NULL), 0);
	if (keyboard_hook == NULL) {
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