#include <stdio.h>
#include <Windows.h>

void print_word(char**);
size_t get_words_count(char*);
DWORD WINAPI word_thread(PVOID);


CRITICAL_SECTION _crit_section;
char _output_strign[] = "The AMD641 architecture2 is an extension of the x86 architecture. Any processor implementing the AMD64 architecture specification will also provide compatibility modes for previous descendants of the Intel 8086 architecture, including 32 - bitprocessors such as the Intel 386, Intel Pentium, and AMD K6 - 2 processor. ";
char* _ptr_string = _output_strign;
size_t _size_string;
size_t _current_word = 0;	

int main() {
	InitializeCriticalSection(&_crit_section);
	_size_string = get_words_count(_ptr_string);
	printf("COUNT WORDS: %zu \n\n", _size_string);
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
									NULL);;
	if (_thread_2 == NULL) {
		printf("Thread 2 has not been created");
		return 0;
	}

	WaitForSingleObject(_thread_1, INFINITE);
	WaitForSingleObject(_thread_2, INFINITE);

	CloseHandle(_thread_1);
	CloseHandle(_thread_2);
	DeleteCriticalSection(&_crit_section);
	_ptr_string = _output_strign;
	return 0;
}

size_t get_words_count(char* _ptr) {
	size_t _count_words = 0;
	while (*_ptr != '\0') {
		if (*_ptr == ' ') _count_words++;
		_ptr++;
	}
	_ptr_string = _output_strign;
	return _count_words + 1;
}

void print_word(char** ptr_ptr) {
	while (**ptr_ptr != '\0' && **ptr_ptr != ' ') {
		printf("%c" , **ptr_ptr);
		(*ptr_ptr)++;
	}
	printf("\n");
	(*ptr_ptr)++;
}

DWORD WINAPI word_thread(PVOID pv_Param) {
	DWORD _current_thread_id = GetCurrentThreadId();
	while (_current_word < _size_string) {
		EnterCriticalSection(&_crit_section);
		printf("Thread %d is worked (ID: %lu)\n", (int)(INT_PTR)pv_Param, _current_thread_id);
		print_word(&_ptr_string);
		_current_word++;
		LeaveCriticalSection(&_crit_section);
		Sleep(10);
	}
	return 0; 
}
