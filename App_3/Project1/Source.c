#include <stdio.h>
#include <stdbool.h>
#include <Windows.h>
#include <stdlib.h>

#define COUNT_PROCESSES_OBJECT 3
wchar_t exe_path[] = L"C:\Windows\system32\cmd.exe";

struct Memory_mapped_file;
void freeing_resources_mmf(struct Memory_mapped_file*);
struct Memory_mapped_file* create_resources_mmf();
void create_multiple_process(STARTUPINFO*, PROCESS_INFORMATION*, BOOL*);
void close_multiple_process(PROCESS_INFORMATION*);

struct Memory_mapped_file
{
	HANDLE h_Map_File;
	LPVOID pDate;
};


int main() {
	STARTUPINFO _sts_info[COUNT_PROCESSES_OBJECT];
	PROCESS_INFORMATION _processes_info[COUNT_PROCESSES_OBJECT];
	BOOL _status_process[COUNT_PROCESSES_OBJECT];

	create_multiple_process(_sts_info, _processes_info, _status_process);

	for (size_t i = 0; i < COUNT_PROCESSES_OBJECT; i++) {
		if (!_status_process[i]) {
			printf("Process %zu creation failed: \n", i);
		}
	}

	struct Memory_mapped_file* _mmf = create_resources_mmf();
	if (_mmf == NULL){
		printf("Failed to create MMF resources\n");
		close_multiple_process(_processes_info);
		return 1;
	}

	freeing_resources_mmf(_mmf);
	

	for (size_t i = 0; i < COUNT_PROCESSES_OBJECT; i++) {
		CloseHandle(_processes_info[i].hProcess);
	}
	return 0;
}

void freeing_resources_mmf(struct Memory_mapped_file* _MMF) {
	if (_MMF == NULL) return;
	
	if (_MMF->pDate != NULL) {
		UnmapViewOfFile(_MMF->pDate);
	}
	UnmapViewOfFile(_MMF->pDate);
	if (_MMF->h_Map_File != NULL && _MMF->h_Map_File != INVALID_HANDLE_VALUE) {
		CloseHandle(_MMF->h_Map_File);
	}
	free(_MMF);
}
struct Memory_mapped_file* create_resources_mmf() {
	struct Memory_mapped_file* temp_MMF = malloc(sizeof(struct Memory_mapped_file));
	temp_MMF->h_Map_File = NULL;
	temp_MMF->pDate = NULL;

	temp_MMF->h_Map_File = CreateFileMapping(INVALID_HANDLE_VALUE,
		NULL,
		PAGE_READWRITE,
		0,
		4096,
		L"Global\\MyShareMemory");
	temp_MMF->pDate = MapViewOfFile(temp_MMF->h_Map_File,
		FILE_MAP_ALL_ACCESS,
		0,
		0,
		4096);

	if (temp_MMF->pDate == NULL) {
		printf("MapViewOfFile failed: %lu\n", GetLastError());
		CloseHandle(temp_MMF->h_Map_File);
		return NULL;
	}
	return temp_MMF;
}

void create_multiple_process(STARTUPINFO* _sts_info, PROCESS_INFORMATION* _process_info, BOOL* _status_process) {
	for (size_t i = 0; i < COUNT_PROCESSES_OBJECT; i++) {
		ZeroMemory(&_sts_info[i], sizeof(_sts_info[i]));
		_sts_info[i].cb = sizeof(_sts_info[i]);
		ZeroMemory(&_process_info[i], sizeof(_process_info[i]));

		_status_process[i] = CreateProcess(NULL,
										   exe_path,
			                               NULL,
										   NULL,
										   FALSE,
										   0,
										   NULL,
										   NULL,
										   &_sts_info[i],
										   &_process_info[i]);
	}
	return;
}

void close_multiple_process(PROCESS_INFORMATION* _process_info) {
	for (size_t i = 0; i < COUNT_PROCESSES_OBJECT; i++) {
		CloseHandle(_process_info[i].hProcess);
	}
}