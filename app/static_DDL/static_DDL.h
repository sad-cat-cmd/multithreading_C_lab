#ifdef STATICDDL_EXPORTS
#define STATICDDL_API __declspec(dllexport)
#else
#define STATICDDL_API __declspec(dllimport)
#endif

#ifdef __cplusplus
extern "C" {
#endif

STATICDDL_API BOOL get_client_SystemTime(HANDLE hMailSlot, SYSTEMTIME* client_st);
STATICDDL_API ULARGE_INTEGER convert_ST_in_UI(SYSTEMTIME* ST);
STATICDDL_API ULONGLONG Differt_UI_in_sec(ULARGE_INTEGER* server_UI, ULARGE_INTEGER* client_UI);

#ifdef __cplusplus
}
#endif