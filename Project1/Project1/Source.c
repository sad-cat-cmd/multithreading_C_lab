#include <stdio.h>
#include <Windows.h>

typedef BOOL(*P_Install_Mouse_Hook)();
typedef BOOL(*P_Remove_Mouse_Hook)();
P_Install_Mouse_Hook p_installMouseHook;
P_Remove_Mouse_Hook p_removeMouseHook;

int main() {
	MSG msg;
	HMODULE hDll;

	hDll = LoadLibrary(TEXT("hookDLLdll"));
	if (!hDll) {
		printf("Error loading dll\n");
		return 1;
	}
	p_installMouseHook = (P_Install_Mouse_Hook)GetProcAddress(hDll, "installMouseHook");
	p_removeMouseHook = (P_Remove_Mouse_Hook)GetProcAddress(hDll, "removeMouseHook");


	if (!p_installMouseHook || !p_removeMouseHook) {
		printf("Error getting function addresses\n");
		FreeLibrary(hDll);
		return 1;
	}

	if (!p_installMouseHook())
		printf("install hook\n");
	else {
		printf("failed to install hook\n");
		FreeLibrary(hDll);
		return 1;
	}

	while (GetMessage(&msg, NULL, 0, 0)) {
		DispatchMessage(&msg); 
	}
	if (!p_removeMouseHook()) {
		printf("Uninstall hook\n");
	}
	else {
		printf("Failed uninstall hook\n");
	}
	FreeLibrary(hDll);
	return 0;
}