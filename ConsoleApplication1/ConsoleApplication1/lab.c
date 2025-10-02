#include <stdio.h>
#include <windows.h>

#define COUNT_THREAD 10
HANDLE _semaphore = NULL;

DWORD WINAPI Limeted_Thread(LPVOID lp_PARAM) {
    int thread_id = *(int*)lp_PARAM;
    DWORD current_thread_id = GetCurrentThreadId();
    printf("Thread %d  (ID = %lu): awaiting\n", thread_id, current_thread_id);
    WaitForSingleObject(_semaphore, INFINITE);
    printf("Thread %d (ID = %lu): started work\n", thread_id, current_thread_id);
    for (size_t i = 0; i < 5; i++) {
        printf("Thread %d: iteration %d \n", thread_id, i);
        Sleep(1000);
    }
    printf("Thread %d (ID = %lu): stoped work\n", thread_id, current_thread_id);
    ReleaseSemaphore(_semaphore, 1, NULL);
    return 0;
}
int main() {
    _semaphore = CreateSemaphore(0,3,3,NULL);

    if (_semaphore == NULL) { printf("ERROR CREATE SEMAPHORE\n"); }

    HANDLE _threads[COUNT_THREAD];
    DWORD _thread_ids[COUNT_THREAD];
    int param = 1;

    for (size_t i = 0; i < COUNT_THREAD; i++) {
        _thread_ids[i] = i + 1;
        _threads[i] = CreateThread(NULL,
                                   0,
                                   Limeted_Thread,
                                   &_thread_ids[i],
                                   0,
                                   NULL);

        if (_threads[i] == NULL) { printf("ERROR: CREATE THREAD[i] = NULL\n"); }
    }

    

    WaitForMultipleObjects(COUNT_THREAD, _threads, TRUE, INFINITE);

    for (size_t i = 0; i < COUNT_THREAD; i++) {
        if (_threads[i]!= NULL) {CloseHandle(_threads[i]);}
    }
    CloseHandle(_semaphore);
    return 0;
}
