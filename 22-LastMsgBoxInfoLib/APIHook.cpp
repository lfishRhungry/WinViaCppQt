#include "pch.h"
#include "APIHook.h"
#include "../CommonFiles/CmnHdr.h"
#include "../CommonFiles/Toolhelp.h"
#include <strsafe.h>
#include <ImageHlp.h>
#pragma comment(lib, "ImageHlp")


/////////////////////////////////////////////////////////////////////////////////


// ��ʼ��CAPIHook��������ͷ
// ��������������ඨ���г�ʼ����̬��Ա�����ķ���
// ����ֻ���ʼ����ôһ��
CAPIHook* CAPIHook::sm_pHead = NULL;

// Ĭ�����ǵ�ģ���ǲ���hook��
BOOL CAPIHook::ExcludeAPIHookMod = TRUE;


///////////////////////////////////////////////////////////////////////////////


CAPIHook::CAPIHook(PSTR pszCalleeModName, PSTR pszFuncName, PROC pfnHook) {

	/*
	 ע�� ������ִ��hook֮ǰ��Ŀ����̱����Ѿ�����������Ҫhook�ĺ���������ģ��
		  ���������ò�����hook����ԭ��ַ Ҳ�����޸���Ӧ�ĵ��������
	 һ�ֽ������ �ñ����ص�LoadLibraryϵ�к��������������ģ��ĵ��������Ƿ���
		  ����Ҫhook�ĺ��� ����� ��ô���Ǿ��õ��˱�hook����ԭ��ַ ��ͷ��֮ǰ��
		  �����������ģ�鵼�����и��� ͬʱ���Ա�hook��������ģ��ĵ��������
		  ���� ��ôһ����֮�������������ģ�鶼��������ǵ��滻������
	*/

	// ��ʼ��CAPIHook����
	// ����sm_mpHead�Ǿ�̬��Ա����
	// ������ô���൱�ڶ�̬����CAPIHook������Ӷ�����ͷ��
	m_pNext = sm_pHead;
	sm_pHead = this;

	// ���汾CAPIHook����Ҫhook����Ϣ
	// ����ʼ�Ƿ�ֹѭ��hook��һ���÷�ʽ
	// ��ÿһ��CAPIHook����ʼ��֪���Լ�������
	m_pszCalleeModName = pszCalleeModName;
	m_pszFuncName = pszFuncName;
	m_pfnHook = pfnHook;
	m_pfnOrig =
		GetProcAddressRaw(GetModuleHandleA(pszCalleeModName), m_pszFuncName);

	// ������������ڣ�������δ�����룩�ݰ�������~
	if (m_pfnOrig == NULL)
	{
#ifdef _DEBUG
		// ���ﹶ�Ӳд����������Ϣ
		wchar_t szPathname[MAX_PATH];
		GetModuleFileNameW(NULL, szPathname, _countof(szPathname));
		wchar_t sz[1024];
		StringCchPrintfW(sz, _countof(sz),
			TEXT("[%4u - %s] impossible to find %S\r\n"),
			GetCurrentProcessId(), szPathname, pszFuncName);
		OutputDebugString(sz);
#endif
		return;
	}

#ifdef _DEBUG
	// This section was used for debugging sessions when Explorer died as 
	// a folder content was requested
	// ���������������������Ե�
	// �м��漰�����龰�ǣ���ע�뵽Explorer���̣�����ExplorerҪִ�д��ļ��в���
	// 
	//static BOOL s_bFirstTime = TRUE;
	//if (s_bFirstTime)
	//{
	//   s_bFirstTime = FALSE;

	//   wchar_t szPathname[MAX_PATH];
	//   GetModuleFileNameW(NULL, szPathname, _countof(szPathname));
	//   wchar_t* pszExeFile = wcsrchr(szPathname, L'\\') + 1;
	//   OutputDebugStringW(L"Injected in ");
	//   OutputDebugStringW(pszExeFile);
	//   if (_wcsicmp(pszExeFile, L"Explorer.EXE") == 0)
	//   {
	//      DebugBreak();
	//   }
	//   OutputDebugStringW(L"\n   --> ");
	//   StringCchPrintfW(szPathname, _countof(szPathname), L"%S", pszFuncName);
	//   OutputDebugStringW(szPathname);
	//   OutputDebugStringW(L"\n");
	//}
#endif

   // ��ʼhook��~
	ReplaceIATEntryInAllMods(m_pszCalleeModName, m_pfnOrig, m_pfnHook);
}


///////////////////////////////////////////////////////////////////////////////


CAPIHook::~CAPIHook() {

	// ��ת��ַ �ﵽunhook��Ч��
	ReplaceIATEntryInAllMods(m_pszCalleeModName, m_pfnHook, m_pfnOrig);

	// ����CAPIHook������������Ƴ�
	CAPIHook* p = sm_pHead;
	if (p == this) {     // Removing the head node
		sm_pHead = p->m_pNext;
	}
	else {

		BOOL bFound = FALSE;

		for (; !bFound && (p->m_pNext != NULL); p = p->m_pNext) {
			if (p->m_pNext == this) {
				p->m_pNext = p->m_pNext->m_pNext;
				bFound = TRUE;
			}
		}
	}
}


///////////////////////////////////////////////////////////////////////////////


// ��������������ڵ���hook����Ľ�����������������GetProcAddress����
// ֻ������������汾���������ֶ�����ʹ��
FARPROC CAPIHook::GetProcAddressRaw(HMODULE hmod, PCSTR pszProcName) {

	return(::GetProcAddress(hmod, pszProcName));
}


///////////////////////////////////////////////////////////////////////////////


// ����ָ���ڴ��ַ����ģ��HMODULE ���������Ա������
static HMODULE ModuleFromAddress(PVOID pv) {

	MEMORY_BASIC_INFORMATION mbi;
	return((VirtualQuery(pv, &mbi, sizeof(mbi)) != 0)
		? (HMODULE)mbi.AllocationBase : NULL);
}


///////////////////////////////////////////////////////////////////////////////


void CAPIHook::ReplaceIATEntryInAllMods(PCSTR pszCalleeModName,
	PROC pfnCurrent, PROC pfnNew) {

	HMODULE hmodThisMod = ExcludeAPIHookMod
		? ModuleFromAddress(ReplaceIATEntryInAllMods) : NULL;

	// ��ȡĿ�����ģ���б����
	CToolhelp th(TH32CS_SNAPMODULE, GetCurrentProcessId());

	MODULEENTRY32 me = { sizeof(me) };
	for (BOOL bOk = th.ModuleFirst(&me); bOk; bOk = th.ModuleNext(&me)) {

		// ����hook�����Լ���ģ��(���忴ExcludeAPIHookMod����������)
		if (me.hModule != hmodThisMod) {

			// Hook this function in this module
			ReplaceIATEntryInOneMod(
				pszCalleeModName, pfnCurrent, pfnNew, me.hModule);
		}
	}
}


///////////////////////////////////////////////////////////////////////////////


// ��������ģ�鱻ж��ʱ�����Ĳ���Ԥ֪�쳣��SEH���˺���
LONG WINAPI InvalidReadExceptionFilter(PEXCEPTION_POINTERS pep) {

	// ������ζ�����EXCEPTION_EXECUTE_HANDLER
	// ��ʾ�ܹ�����
	LONG lDisposition = EXCEPTION_EXECUTE_HANDLER;


	return(lDisposition);
}


// ���ļ�����֮һ
void CAPIHook::ReplaceIATEntryInOneMod(PCSTR pszCalleeModName,
	PROC pfnCurrent, PROC pfnNew, HMODULE hmodCaller) {

	// ��ȡģ�鵼��ε�ַ
	ULONG ulSize;

	//���������Ŀ��������˵Explorer��˵������ɨ���ļ��е�ʱ���ǻ���ٵ������ж��dll��
	//���ܾͻᵼ�����������ﴫ���ImageDirectoryEntryToData�����ľ��ʧЧ��
	//Ҳ�п���������ʹ��Toolhelp�õ�ģ�����֮�󣬸�ģ���ֱ�ж���ˣ����¾��ʧЧ
	//�ܶ���֮�����б�Ҫʹ��SEH��������δ���
	PIMAGE_IMPORT_DESCRIPTOR pImportDesc = NULL;
	__try {
		// �ú�����λģ��ĵ����
		pImportDesc = (PIMAGE_IMPORT_DESCRIPTOR)ImageDirectoryEntryToData(
			hmodCaller, TRUE, IMAGE_DIRECTORY_ENTRY_IMPORT, &ulSize);
	}
	__except (InvalidReadExceptionFilter(GetExceptionInformation())) {
		// ����ʲôҲ����Ҫ����ֻ������ʱpImportDesc��NULL
	}

	if (pImportDesc == NULL)
		return;  // ��ģ���޵����


	// ����IMAGE_IMPORT_DESCRIPTOR�ṹ�е��������ҵ�����ָ��ģ���IAT
	for (; pImportDesc->Name; pImportDesc++) {
		PSTR pszModName = (PSTR)((PBYTE)hmodCaller + pImportDesc->Name); // RVAת��ȡ������
		// �ҵ���~
		if (lstrcmpiA(pszModName, pszCalleeModName) == 0) {

			// ��λ��ָ������ģ��ĵ�һ��IMAGE_THUNK_DATA�ṹ
			PIMAGE_THUNK_DATA pThunk = (PIMAGE_THUNK_DATA)
				((PBYTE)hmodCaller + pImportDesc->FirstThunk);

			// ��������IMAGE_THUNK_DATA���ҵ���ű�hook������ַ��λ��
			// ���˵���ģ���Ѿ���hook���� ���Ҳ�����
			for (; pThunk->u1.Function; pThunk++) {

				PROC* ppfn = (PROC*)&pThunk->u1.Function;

				// �Ƚ����д�ŵĵ�ַ�Ƿ�������ָ���ĺ�����ַ
				BOOL bFound = (*ppfn == pfnCurrent);
				if (bFound) {
					if (!WriteProcessMemory(GetCurrentProcess(), ppfn, &pfnNew,
						sizeof(pfnNew), NULL) && (ERROR_NOACCESS == GetLastError())) {
						// ���дʧ�ܾ��޸�Ȩ����д
						DWORD dwOldProtect;
						// ����ʹ��дʱ�����Ƿ�ֹ��Դ�ļ������޸Ŀ�����ɵ���
						if (VirtualProtect(ppfn, sizeof(pfnNew), PAGE_WRITECOPY,
							&dwOldProtect)) {

							WriteProcessMemory(GetCurrentProcess(), ppfn, &pfnNew,
								sizeof(pfnNew), NULL);
							// �ǵûָ�ԭ���ı�������
							VirtualProtect(ppfn, sizeof(pfnNew), dwOldProtect,
								&dwOldProtect);
						}
					}
					return;  // �޸���� ����~
				}
			}
		}  // ��Щ����������ͬһ��ģ������ж������� �������û���޸ĵ��Ļ� ���ﻹҪ����ѭ��Ѱ��
	}
}


///////////////////////////////////////////////////////////////////////////////


// ���������������� ����û���õ� ֻ���ṩ�˴�����ʱ��������Ŀ����õ��Ĺ���
void CAPIHook::ReplaceEATEntryInOneMod(HMODULE hmod, PCSTR pszFunctionName,
	PROC pfnNew) {

	// ��ȡ��ģ�鵼�����ַ
	ULONG ulSize;

	// ���޸�IAT�����һ�� ��Ϊ��ȡģ������ͨ�������ȡ�õ������β�����ԭ�Ӳ���
	// ����Ҫ���ǵ�ģ����ʧЧ�Ŀ���
	PIMAGE_EXPORT_DIRECTORY pExportDir = NULL;
	__try {
		pExportDir = (PIMAGE_EXPORT_DIRECTORY)ImageDirectoryEntryToData(
			hmod, TRUE, IMAGE_DIRECTORY_ENTRY_EXPORT, &ulSize);
	}
	__except (InvalidReadExceptionFilter(GetExceptionInformation())) {}

	if (pExportDir == NULL)
		return;  // û�е�������Ѿ�ģ���ѱ�ж��

	PDWORD pdwNamesRvas = (PDWORD)((PBYTE)hmod + pExportDir->AddressOfNames);
	PWORD pdwNameOrdinals = (PWORD)
		((PBYTE)hmod + pExportDir->AddressOfNameOrdinals);
	PDWORD pdwFunctionAddresses = (PDWORD)
		((PBYTE)hmod + pExportDir->AddressOfFunctions);

	// ����ģ�麯��������
	for (DWORD n = 0; n < pExportDir->NumberOfNames; n++) {
		PSTR pszFuncName = (PSTR)((PBYTE)hmod + pdwNamesRvas[n]);

		// ����ָ��������continue
		if (lstrcmpiA(pszFuncName, pszFunctionName) != 0) continue;

		WORD ordinal = pdwNameOrdinals[n];

		// ��ȡԭ������ַ���ڵ�ַ
		PROC* ppfn = (PROC*)&pdwFunctionAddresses[ordinal];

		// ���滻������ַת��ΪRVA
		pfnNew = (PROC)((PBYTE)pfnNew - (PBYTE)hmod);

		// �޸�֮
		if (!WriteProcessMemory(GetCurrentProcess(), ppfn, &pfnNew,
			sizeof(pfnNew), NULL) && (ERROR_NOACCESS == GetLastError())) {
			DWORD dwOldProtect;
			if (VirtualProtect(ppfn, sizeof(pfnNew), PAGE_WRITECOPY,
				&dwOldProtect)) {

				WriteProcessMemory(GetCurrentProcess(), ppfn, &pfnNew,
					sizeof(pfnNew), NULL);
				VirtualProtect(ppfn, sizeof(pfnNew), dwOldProtect, &dwOldProtect);
			}
		}
		break;  // �޸ĳɹ� ����
	}
}


///////////////////////////////////////////////////////////////////////////////
// ��LoadLibraryϵ�к�����GeetProcAddress����hook�� ����һ�������⼸��������
// ��������Ҫhook�ĺ��� һ�о���������
// ���ﶼ�Ǿ�̬��Ա���� ����ÿ��ʵ����һ��CAPIHook���󶼻������⼸������������޵���
// ����ÿ��ע��ɹ�ʱ ��ֻ���ʼ����ôһ��LoadLibraryϵ�к�����GetProcAddress����
// ȷ�����ÿһ��ָ��hook�ĺ�����֮һ���׵�һ�����б���hook����

CAPIHook CAPIHook::sm_LoadLibraryA((PSTR)"Kernel32.dll", (PSTR)"LoadLibraryA",
(PROC)CAPIHook::LoadLibraryA);

CAPIHook CAPIHook::sm_LoadLibraryW("Kernel32.dll", "LoadLibraryW",
(PROC)CAPIHook::LoadLibraryW);

CAPIHook CAPIHook::sm_LoadLibraryExA("Kernel32.dll", "LoadLibraryExA",
(PROC)CAPIHook::LoadLibraryExA);

CAPIHook CAPIHook::sm_LoadLibraryExW("Kernel32.dll", "LoadLibraryExW",
(PROC)CAPIHook::LoadLibraryExW);

CAPIHook CAPIHook::sm_GetProcAddress("Kernel32.dll", "GetProcAddress",
(PROC)CAPIHook::GetProcAddress);


///////////////////////////////////////////////////////////////////////////////


// ������Ҫ�Ǹ������LoadLibrary������GetProcAddress������ʹ�õ�
void CAPIHook::FixupNewlyLoadedModule(HMODULE hmod, DWORD dwFlags) {

	// ���������ģ�� �Ͱ����ģ�������ǵĺ���hook��
	if ((hmod != NULL) &&
		// ����hook�������Լ���ģ��
		(hmod != ModuleFromAddress(FixupNewlyLoadedModule)) &&
		// �⼸������ģʽ�Ϳ��Բ��ø��� ��Ϊ�ǲ���ִ�е����뷽ʽ
		((dwFlags & LOAD_LIBRARY_AS_DATAFILE) == 0) &&
		((dwFlags & LOAD_LIBRARY_AS_DATAFILE_EXCLUSIVE) == 0) &&
		((dwFlags & LOAD_LIBRARY_AS_IMAGE_RESOURCE) == 0)
		) {

		// �������ǹ����CAPIHook���� ��ÿ�������ٴζ�����ģ�����hook
		for (CAPIHook* p = sm_pHead; p != NULL; p = p->m_pNext) {
			if (p->m_pfnOrig != NULL) {
				ReplaceIATEntryInAllMods(p->m_pszCalleeModName,
					p->m_pfnOrig, p->m_pfnHook);
			}
			else {
#ifdef _DEBUG
				// ˵ʵ�ڵ�Ӧ�ò�����ִ�е��������
				wchar_t szPathname[MAX_PATH];
				GetModuleFileNameW(NULL, szPathname, _countof(szPathname));
				wchar_t sz[1024];
				StringCchPrintfW(sz, _countof(sz),
					TEXT("[%4u - %s] impossible to find %S\r\n"),
					GetCurrentProcessId(), szPathname, p->m_pszCalleeModName);
				OutputDebugString(sz);
#endif
			}
		}
	}
}


///////////////////////////////////////////////////////////////////////////////


HMODULE WINAPI CAPIHook::LoadLibraryA(PCSTR pszModulePath) {

	// ˵ʵ�����ﻹͦ���~
	HMODULE hmod = ::LoadLibraryA(pszModulePath);
	FixupNewlyLoadedModule(hmod, 0);
	return(hmod);
}


///////////////////////////////////////////////////////////////////////////////


HMODULE WINAPI CAPIHook::LoadLibraryW(PCWSTR pszModulePath) {

	HMODULE hmod = ::LoadLibraryW(pszModulePath);
	FixupNewlyLoadedModule(hmod, 0);
	return(hmod);
}


///////////////////////////////////////////////////////////////////////////////


HMODULE WINAPI CAPIHook::LoadLibraryExA(PCSTR pszModulePath,
	HANDLE hFile, DWORD dwFlags) {

	HMODULE hmod = ::LoadLibraryExA(pszModulePath, hFile, dwFlags);
	FixupNewlyLoadedModule(hmod, dwFlags);
	return(hmod);
}


///////////////////////////////////////////////////////////////////////////////


HMODULE WINAPI CAPIHook::LoadLibraryExW(PCWSTR pszModulePath,
	HANDLE hFile, DWORD dwFlags) {

	// ԭ�����dwFlags��������ѽ
	HMODULE hmod = ::LoadLibraryExW(pszModulePath, hFile, dwFlags);
	FixupNewlyLoadedModule(hmod, dwFlags);
	return(hmod);
}


///////////////////////////////////////////////////////////////////////////////


FARPROC WINAPI CAPIHook::GetProcAddress(HMODULE hmod, PCSTR pszProcName) {

	// �õ��桤GetProcAddress������ַ
	FARPROC pfn = GetProcAddressRaw(hmod, pszProcName);

	// ����Ҫ����һ������CAPIHook��������û��Ҫ�������������hook
	// ����� ��˳�Ʒ��ض�Ӧ��������� �������
	CAPIHook* p = sm_pHead;
	for (; (pfn != NULL) && (p != NULL); p = p->m_pNext) {

		if (pfn == p->m_pfnOrig) {

			pfn = p->m_pfnHook;
			break;
		}
	}

	return(pfn);
}


//////////////////////////////// End of File //////////////////////////////////

