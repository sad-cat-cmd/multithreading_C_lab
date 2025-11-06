#ifdef DYNAMICDDL_EXPORTS
#define DYNAMICDDL_API __declspec(dllexport)
#else
#define DYNAMICDDL_API __declspec(dllimport)
#endif

DYNAMICDDL_API HANDLE WINAPI MakeSlot();
DYNAMICDDL_API BOOL check_status_MailSlot(HANDLE hMailSlot);
DYNAMICDDL_API inline void print_info(ULONGLONG diff);
DYNAMICDDL_API HANDLE CreateWaitTimer();
