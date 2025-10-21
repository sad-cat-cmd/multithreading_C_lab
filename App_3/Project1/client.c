#define _CRT_SECURE_NO_WARNINGS
#include <Windows.h>
#include <stdio.h>

TCHAR MUTEX_NAME[] = TEXT("mutex_MMF");
TCHAR lpFileShareName[] = TEXT("$MyVerySpecialFileShareName$");

int main(int argc, char* argv[]) {
	if (argc < 2) {
		printf("The ID_client is not specified\n");
		return 1;
	}
	int client_id = atoi(argv[1]);
	HANDLE hMapFile = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, lpFileShareName);
	if (hMapFile == NULL) {
		printf("Client %d: ERROR OPEN FILE:\n", client_id);
		return 1;
	}

	char* shared_data = (char*)MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, 4096);
	if (shared_data == NULL) {
		printf("Error: MapViewOfFile\n");
		CloseHandle(hMapFile);
		return 1;
	}

	HANDLE hMutex = OpenMutex(MUTEX_ALL_ACCESS, FALSE, MUTEX_NAME);
	if (hMutex == NULL) {
		printf("Error: Client %d is not open mutex", client_id);
		return 1;
	}
	
	WaitForSingleObject(hMutex, INFINITE);

	int current_length = strlen(shared_data);

	for (int i = 0; i < 3; i++) {
		char message[256];
		sprintf_s(message, sizeof(message), "Message %d from client %d\n", i + 1, client_id);

		strcpy_s(shared_data + current_length, 4096 - current_length, message);
		current_length += strlen(message);

		printf("Client %d wrote: %s", client_id, message);
		Sleep(500);
	}

	ReleaseMutex(hMutex);
	UnmapViewOfFile(shared_data);
	CloseHandle(hMapFile);
}