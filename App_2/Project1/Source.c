#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <Windows.h>

void print_word(char**);
DWORD WINAPI word_thread(PVOID);

CRITICAL_SECTION crit_section;
char output_strign[] = "The AMD641 architecture2 is an extension of the x86 architecture. Any processor implementing the AMD64 architecture specification will also provide compatibility modes for previous descendants of the Intel 8086 architecture, including 32 - bitprocessors such as the Intel 386, Intel Pentium, and AMD K6 - 2 processor. ";
char* ptr_string = output_strign;

int main() {
	InitializeCriticalSection(&crit_section);
	HANDLE _thread_1 = CreateThread(NULL,
									0,
									word_thread,
									(LPVOID)1,
									0,
									NULL);
	if (_thread_1 == NULL) {
		printf("Thread 1 has not been created");
		return 0; 
	}
	
	HANDLE _thread_2 = CreateThread(NULL,
									0,
									word_thread,
									(LPVOID)2,
									0,
									NULL);
	if (_thread_2 == NULL) {
		printf("Thread 2 has not been created");
		return 0;
	}

	WaitForSingleObject(_thread_1, INFINITE);
	WaitForSingleObject(_thread_2, INFINITE);

	CloseHandle(_thread_1);
	CloseHandle(_thread_2);
	DeleteCriticalSection(&crit_section);
	return 0;
}

void print_word(char** ptr_ptr) {
	char temp[20];
	size_t index = 0;
	char* prev_ptr = *ptr_ptr;
	while (**ptr_ptr != '\0' && **ptr_ptr != ' ') {
		(*ptr_ptr)++;
		index++;
	}
	const int i = index;
	temp[i] = '\0';
	strncpy(temp, prev_ptr, index);
	printf("%s\n",temp);
	(*ptr_ptr)++;
}

DWORD WINAPI word_thread(PVOID pv_Param) {
	DWORD _current_thread_id = GetCurrentThreadId();
	while (*ptr_string != '\0') {
		EnterCriticalSection(&crit_section);
		printf("Thread %d is worked (ID: %lu)\n", (int)(INT_PTR)pv_Param, _current_thread_id);
		print_word(&ptr_string);
		LeaveCriticalSection(&crit_section);
		Sleep(10);
	}
	return 0; 
}
