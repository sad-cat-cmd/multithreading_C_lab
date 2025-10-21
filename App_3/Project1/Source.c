#include <stdio.h>
#include <stdbool.h>
#include <Windows.h>
#include <stdlib.h>

TCHAR MUTEX_NAME[] = TEXT("mutex_MMF");
TCHAR lpFileShareName[] = TEXT("$MyVerySpecialFileShareName$");

int main() {
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
	
	char* shared_data = (char*)MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, 4096);
	if (shared_data == NULL) {
		printf("Error: MapWievOfFile in main process %lu \n", GetLastError());
		return 1;
	}
	shared_data[0] = '\0';
	printf("Server is started\n");

	hMutex = CreateMutex(NULL, FALSE, MUTEX_NAME);
	
	if (hMutex == NULL)
	{
		printf("CreateMutex error: %d\n", GetLastError());
		return 1;
	}

	if (CreateProcess(L"C:\\Users\\win_\\Desktop\\multithreading_C_lab\\App_3\\Project1\\client.exe", L"client.exe 1", NULL, NULL, FALSE, 0, NULL, NULL, &client_1, &pi_1)) {
		printf("Client 1 is started\n");
	}
	else {
		printf("Error: create Process CLIENT_1 %lu\n", GetLastError());
		return 1;
	}

	if (CreateProcess(L"C:\\Users\\win_\\Desktop\\multithreading_C_lab\\App_3\\Project1\\client.exe", L"client.exe 2", NULL, NULL, FALSE, 0, NULL, NULL, &client_2, &pi_2)) {
		printf("Client 2 is started\n");
	}
	else {
		printf("Error: create Process CLIENT_1 %lu\n", GetLastError());
		return 1;
	}

	WaitForSingleObject(pi_1.hProcess, INFINITE);
	WaitForSingleObject(pi_2.hProcess, INFINITE);

	WaitForSingleObject(hMutex, INFINITE);

	if (strlen(shared_data) > 0) {
		printf("All messages from clients:\n%s", shared_data);
		shared_data[0] = '\0';
	}
	else {
		printf("No messages received\n");
	}

	ReleaseMutex(hMutex);

	CloseHandle(pi_1.hProcess);
	CloseHandle(pi_2.hProcess);
	CloseHandle(pi_1.hThread);
	CloseHandle(pi_2.hThread);

	UnmapViewOfFile(shared_data);
	CloseHandle(hMapFile);
	return 0;
}