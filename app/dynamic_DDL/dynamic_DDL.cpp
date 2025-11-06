#include "pch.h"
#include "framework.h"
#include "dynamic_DDL.h"


LPCWSTR SlotName = TEXT("\\\\.\\mailslot\\network_timesync");

DYNAMICDDL_API HANDLE WINAPI MakeSlot() {
    HANDLE hMailSlot = CreateMailslot(SlotName, 0, MAILSLOT_WAIT_FOREVER, (LPSECURITY_ATTRIBUTES)NULL);
    if (hMailSlot == INVALID_HANDLE_VALUE) {
        printf("CreateMailSlot failed with %d\n", GetLastError());
        return NULL;
    }
    return hMailSlot;
}

DYNAMICDDL_API BOOL check_status_MailSlot(HANDLE hMailSlot) {
    BOOL return_code;
    DWORD next_size, msg_number;
    return_code = GetMailslotInfo(hMailSlot, NULL, &next_size, &msg_number, NULL);
    if (return_code == FALSE) {
        printf("error GETMAILSLOTINFO\n");
        return TRUE;
    }
    if (next_size == MAILSLOT_NO_MESSAGE) {
        //printf("No messages \n");
        return TRUE;
    }
    return FALSE;
}

DYNAMICDDL_API inline void print_info(ULONGLONG diff) {
    SYSTEMTIME server_LT;
    GetLocalTime(&server_LT);
    printf("%02d:%02d:%02d --- time difference of two pcs: %llu \n",
        server_LT.wHour, server_LT.wMinute, server_LT.wSecond, diff);
}

DYNAMICDDL_API HANDLE CreateWaitTimer() {
    HANDLE hTimer;
    hTimer = CreateWaitableTimer(NULL, FALSE, NULL);
    if (hTimer == INVALID_HANDLE_VALUE) {
        printf("Create WaitTimer failed with %d\n", GetLastError());
        return NULL;
    }
    return hTimer;
}