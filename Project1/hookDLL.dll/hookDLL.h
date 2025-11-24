#ifdef HOOKDLLDLL_EXPORTS
#define HOOKDLLDLL_API __declspec(dllexport)
#else
#define HOOKDLLDLL_API __declspec(dllimport)
#endif

extern "C" {
	HOOKDLLDLL_API BOOL installMouseHook();
	HOOKDLLDLL_API BOOL removeMouseHook();
}
