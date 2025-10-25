#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdbool.h>
#include <Windows.h>
#include <stdlib.h>
#include <wchar.h>

TCHAR MUTEX_NAME[] = TEXT("mutex_MMF");
TCHAR lpFileShareName[] = TEXT("$MyVerySpecialFileShareName$");
BOOL status = FALSE;
wchar_t client_name[MAX_PATH];
wchar_t module_file_name[MAX_PATH];

typedef struct {
	char message[256];
	BOOL data_ready;
	BOOL status_work;
	int number_clint;
} SharedData;

void get_client_name(wchar_t * client_name) {
	wchar_t client_exe[] = L"client.exe";
	wchar_t* last_slash = wcsrchr(module_file_name, L'\\');
	if (last_slash != NULL) {
		size_t path_length = last_slash - module_file_name + 1;
		wcsncpy(client_name, module_file_name, path_length);
		client_name[path_length] = L'\0';
		wcscat(client_name, client_exe);
	}
	else {
		wcscpy(client_name, client_exe);
	}
}

int main() {
	GetModuleFileName(NULL, module_file_name, MAX_PATH);
	get_client_name(&client_name);
	wprintf(L"%s\n", module_file_name);
	wprintf(L"%s\n---------------\n", client_name);

	STARTUPINFO client_1, client_2;
	PROCESS_INFORMATION pi_1, pi_2;
	HANDLE hMapFile, hMutex;

	ZeroMemory(&client_1, sizeof(client_1));
	client_1.cb = sizeof(client_1);
	ZeroMemory(&pi_1, sizeof(pi_1));

	ZeroMemory(&client_2, sizeof(client_2));
	client_2.cb = sizeof(client_2);
	ZeroMemory(&pi_2, sizeof(pi_2));

	hMapFile = CreateFileMapping(INVALID_HANDLE_VALUE,
		NULL,
		PAGE_READWRITE,
		0,
		4096,
		lpFileShareName);
	if (hMapFile == NULL) {
		printf("Error: create FileMapping\n");
		return 1;
	}
	
	SharedData *shared_data = (SharedData*)MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, 4096);
	if (shared_data == NULL) {
		printf("Error: MapWievOfFile in main process %lu \n", GetLastError());
		return 1;
	}
	*shared_data->message = '\0';
	printf("Server is started\n");

	hMutex = CreateMutex(NULL, FALSE, MUTEX_NAME);
	
	if (hMutex == NULL)
	{
		printf("CreateMutex error: %d\n", GetLastError());
		return 1;
	}

	if (!CreateProcess(client_name, L"client.exe 1", NULL, NULL, FALSE, 0, NULL, NULL, &client_1, &pi_1)) {
		printf("Error: create Process CLIENT_1 %lu\n", GetLastError());
		return 1;
	}

	if (!CreateProcess(client_name, L"client.exe 2", NULL, NULL, FALSE, 0, NULL, NULL, &client_2, &pi_2)) {
		printf("Error: create Process CLIENT_1 %lu\n", GetLastError());
		return 1;
	}

	//WaitForSingleObject(pi_1.hProcess, INFINITE);
	//WaitForSingleObject(pi_2.hProcess, INFINITE);

	while (!status) {
		if (shared_data->status_work) {
			status = true;
			break;
		}
		if (shared_data->data_ready) {
			WaitForSingleObject(hMutex, INFINITE);
			printf("\t MESSAGE from %d client:%s\n", shared_data->number_clint, shared_data->message);
			*shared_data->message = '\0';
			shared_data->data_ready = false;
			ReleaseMutex(hMutex);
		}
	}

	CloseHandle(pi_1.hProcess);
	CloseHandle(pi_2.hProcess);
	CloseHandle(pi_1.hThread);
	CloseHandle(pi_2.hThread);

	UnmapViewOfFile(shared_data);
	CloseHandle(hMapFile);
	return 0;
}