/*用于在此模块中操控CAPIHook类
  来hook messagebox函数*/
#ifdef MY22LASTMSGBOXINFOLIB_EXPORTS
#define MY22LASTMSGBOXINFOLIB_API __declspec(dllexport)
#else
#define MY22LASTMSGBOXINFOLIB_API __declspec(dllimport)
#endif

MY22LASTMSGBOXINFOLIB_API BOOL WINAPI LastMsgBoxInfo_HookAllApps(BOOL bInstall,
	DWORD dwThreadId);
