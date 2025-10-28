#define _CRT_SECURE_NO_WARNINGS
#include <Windows.h>
#include <stdio.h>
#include <stdbool.h>

TCHAR MUTEX_NAME[] = TEXT("mutex_MMF");
TCHAR lpFileShareName[] = TEXT("$MyVerySpecialFileShareName$");

BOOL status = false;
typedef struct {
	char message[256];
	BOOL data_ready;
	BOOL status_work;
	int number_clint;
} SharedData;

void write_message(char *ptr_message, int id_client) {
	printf("Enter messege for client %d: ", id_client);
	scanf("%s", ptr_message);
	printf("Messenge is send from client %d \n", id_client);
	return;
}

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

	SharedData* shared_data = (SharedData*)MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, 4096);
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

	int current_length = strlen(shared_data->message);
	char tmp_message[256];
	while (!status)
	{
		if (shared_data->status_work) {
			status = true;
			break;
		}
		if (!shared_data->data_ready) {
			write_message(tmp_message, client_id);
			WaitForSingleObject(hMutex, INFINITE);
			if (!shared_data->data_ready) {
				strcpy(shared_data->message, tmp_message);
				if (shared_data->message[0] == '0') {
					shared_data->status_work = true;
					ReleaseMutex(hMutex);
					break;
				}
				shared_data->data_ready = true;
				shared_data->number_clint = client_id;
			}
			else 
				Sleep(100);
			ReleaseMutex(hMutex);
		}
	}
	UnmapViewOfFile(shared_data);
	CloseHandle(hMapFile);
}