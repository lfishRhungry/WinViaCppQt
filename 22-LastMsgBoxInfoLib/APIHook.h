/*����ִ��IAT hook����*/
#pragma once

 class CAPIHook
{
public:
	// ������ģ����hookһ��ָ������
	CAPIHook(PSTR pszCalleeModName, PSTR pszFuncName, PROC pfnHook);

	// ������ģ����unhookһ��ָ������
	~CAPIHook();

	// ������д��PROC()������ ���ر�hook������ԭ��ַ
	operator PROC() { return(m_pfnOrig); }

	// �Ƿ�������Լ�ע���ģ��ִ��hook����
	// ����ʹ�þ�̬����ΪҪ�ھ�̬��Ա����ReplaceIATEntryInAllMods��ʹ��
	static BOOL ExcludeAPIHookMod;


public:
	// �����桤GetProcAddress
	static FARPROC WINAPI GetProcAddressRaw(HMODULE hmod, PCSTR pszProcName);

private:
	static PVOID sm_pvMaxAppAddr; // ���˽���ڴ�ռ��ַ
	static CAPIHook* sm_pHead;    // CAPIHook��������ͷ
	CAPIHook* m_pNext;            // ��CAPIHook����������ָ����һ������

	PCSTR m_pszCalleeModName;     // ����ָ��������ģ���� (ANSI)
	PCSTR m_pszFuncName;          // ������ (ANSI)
	PROC  m_pfnOrig;              // ����ԭ��ַ
	PROC  m_pfnHook;              // hook�����ĵ�ַ

private:
	// ������ģ���е�IAT�滻ָ�����ŵ�ַ
	static void WINAPI ReplaceIATEntryInAllMods(PCSTR pszCalleeModName,
		PROC pfnOrig, PROC pfnHook);

	// ��һ��ģ���е�IAT�滻ָ�����ŵ�ַ
	static void WINAPI ReplaceIATEntryInOneMod(PCSTR pszCalleeModName,
		PROC pfnOrig, PROC pfnHook, HMODULE hmodCaller);

	// ��һ��ģ���е�EAT�滻ָ�����ŵ�ַ
	static void ReplaceEATEntryInOneMod(HMODULE hmod, PCSTR pszFunctionName, PROC pfnNew);

private:
	// ��������������hook����֮����¼��ص�DLL
	static void    WINAPI FixupNewlyLoadedModule(HMODULE hmod, DWORD dwFlags);

	// ���������¼���DLL�����
	static HMODULE WINAPI LoadLibraryA(PCSTR pszModulePath);
	static HMODULE WINAPI LoadLibraryW(PCWSTR pszModulePath);
	static HMODULE WINAPI LoadLibraryExA(PCSTR pszModulePath,
		HANDLE hFile, DWORD dwFlags);
	static HMODULE WINAPI LoadLibraryExW(PCWSTR pszModulePath,
		HANDLE hFile, DWORD dwFlags);

	// ������hook����ʱ����������������ĵ�ַ
	static FARPROC WINAPI GetProcAddress(HMODULE hmod, PCSTR pszProcName);

private:
	// Ϊ��ȫ��hookָ������ ����Ҫhook���к���
	static CAPIHook sm_LoadLibraryA;
	static CAPIHook sm_LoadLibraryW;
	static CAPIHook sm_LoadLibraryExA;
	static CAPIHook sm_LoadLibraryExW;
	static CAPIHook sm_GetProcAddress;
};


