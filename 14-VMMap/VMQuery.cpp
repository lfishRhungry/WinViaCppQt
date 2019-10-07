#include "VMQuery.h"

// �����ṹ��������Ϣ��
typedef struct {
	SIZE_T RgnSize;
	DWORD  dwRgnStorage;
	DWORD  dwRgnBlocks;
	DWORD  dwRgnGuardBlks;
	BOOL   bRgnIsAStack;
} VMQUERY_HELP;


// ��ʾ��CPUƽ̨�ķ�������ֵ �ڵ�һ�ε���VMQuery����ʱ��ʼ��
static DWORD gs_dwAllocGran = 0;


///////////////////////////////////////////////////////////////////////////////


// ����ָ�������еĿ�����ȡ������������Ϣ
// ������VMQUERY_HELP�ṹ��Ϣ
static BOOL VMQueryHelp(
	HANDLE hProcess,
	LPCVOID pvAddress,
	VMQUERY_HELP* pVMQHelp) {

	ZeroMemory(pVMQHelp, sizeof(*pVMQHelp));

	// ��ȡ����ָ���ڴ��ַ��������ʼ��ַ
	// ����ʹ�õ�ʱVirtualQueryEx ����������ѯ�������̵ĵ�ַ��Ϣ
	MEMORY_BASIC_INFORMATION mbi;
	BOOL bOk = (
		VirtualQueryEx(hProcess, pvAddress, &mbi, sizeof(mbi)) == sizeof(mbi));

	if (!bOk)
		return(bOk);

	// �Ӹ÷����������ַ��ʼ
	// ��ֵ��ѭ��ʱʱ�����ģ�ʼ�ն���ͬһ�������ڣ�
	PVOID pvRgnBaseAddress = mbi.AllocationBase;

	// �ȴӸ������һ���鿪ʼ
	PVOID pvAddressBlk = pvRgnBaseAddress;

	// ����洢������
	pVMQHelp->dwRgnStorage = mbi.Type;

	// ��ʼѭ����
	for (;;) {
		// ��ȡ��ǰ����Ϣ
		bOk = (VirtualQueryEx(hProcess, pvAddressBlk, &mbi, sizeof(mbi))
			== sizeof(mbi));
		if (!bOk)
			break;

		// ͨ���õ�ַ�����������ַ���ж��Ƿ������ǲ�ѯ��������
		if (mbi.AllocationBase != pvRgnBaseAddress)
			break;

		// ��ʼ�������Ϣ

		pVMQHelp->dwRgnBlocks++;             // ���������ڿ�������Ϣ
		pVMQHelp->RgnSize += mbi.RegionSize; // ���Ŀ��С���뵽�����ܴ�С����

		// ����ÿ���PAGE_GUARD���� ������1
		if ((mbi.Protect & PAGE_GUARD) == PAGE_GUARD)
			pVMQHelp->dwRgnGuardBlks++;

		// ͨ�����ַ����׼ȷ�ز²������洢������
		if (pVMQHelp->dwRgnStorage == MEM_PRIVATE)
			pVMQHelp->dwRgnStorage = mbi.Type;

		// ��ȡ��һ�����ַ
		pvAddressBlk = (PVOID)((PBYTE)pvAddressBlk + mbi.RegionSize);
	}

	// ������������ �²��Ƿ�ʱ�߳�ջ
	pVMQHelp->bRgnIsAStack = (pVMQHelp->dwRgnGuardBlks > 0);

	return(TRUE);
}


///////////////////////////////////////////////////////////////////////////////


// ��ͷϷ
BOOL VMQuery(HANDLE hProcess, LPCVOID pvAddress, PVMQUERY pVMQ) {

	if (gs_dwAllocGran == 0) {
		// �ڵ�һ�ε��ú���ʱ���÷�������
		SYSTEM_INFO sinf;
		GetSystemInfo(&sinf);
		gs_dwAllocGran = sinf.dwAllocationGranularity;
	}

	ZeroMemory(pVMQ, sizeof(*pVMQ));

	// ��ȡ�����ַ��MEMORY_BASIC_INFORMATION��Ϣ
	MEMORY_BASIC_INFORMATION mbi;
	BOOL bOk = (VirtualQueryEx(hProcess, pvAddress, &mbi, sizeof(mbi))
		== sizeof(mbi));

	if (!bOk)
		return(bOk);

	// ׼������ ��ʼ������ǵ�VMQUERY�ṹ��Ϣ

	// ���� ������Ϣ ֮�������������Ϣ
	// �жϴ洢����
	switch (mbi.State) {
	case MEM_FREE:       // δԤ��
		pVMQ->pvBlkBaseAddress = NULL;
		pVMQ->BlkSize = 0;
		pVMQ->dwBlkProtection = 0;
		pVMQ->dwBlkStorage = MEM_FREE;
		break;

	case MEM_RESERVE:    // Ԥ������δ����
		pVMQ->pvBlkBaseAddress = mbi.BaseAddress;
		pVMQ->BlkSize = mbi.RegionSize;

		// ����δ������ �䱣����������Чֵ
		// ������������������ı�������
		pVMQ->dwBlkProtection = mbi.AllocationProtect;
		pVMQ->dwBlkStorage = MEM_RESERVE;
		break;

	case MEM_COMMIT:     // �ѵ���
		pVMQ->pvBlkBaseAddress = mbi.BaseAddress;
		pVMQ->BlkSize = mbi.RegionSize;
		pVMQ->dwBlkProtection = mbi.Protect;
		pVMQ->dwBlkStorage = mbi.Type;
		break;

	default:
		DebugBreak();
		break;
	}

	// ���ڿ�ʼ���������Ϣ
	// �������涨��ĸ�������
	VMQUERY_HELP VMQHelp;
	switch (mbi.State) {
	case MEM_FREE:       // δԤ������ʽ
		pVMQ->pvRgnBaseAddress = mbi.BaseAddress;
		pVMQ->dwRgnProtection = mbi.AllocationProtect;
		pVMQ->RgnSize = mbi.RegionSize;
		pVMQ->dwRgnStorage = MEM_FREE;
		pVMQ->dwRgnBlocks = 0;
		pVMQ->dwRgnGuardBlks = 0;
		pVMQ->bRgnIsAStack = FALSE;
		break;

	case MEM_RESERVE:    // ��Ԥ��
		pVMQ->pvRgnBaseAddress = mbi.AllocationBase;
		pVMQ->dwRgnProtection = mbi.AllocationProtect;

		// ���������ڿ�����ȡ����������Ϣ
		// ���������ν��VirtualQueryϵ�к�����ȡ�Ļ������ǿ���Ϣ
		VMQueryHelp(hProcess, pvAddress, &VMQHelp);

		pVMQ->RgnSize = VMQHelp.RgnSize;
		pVMQ->dwRgnStorage = VMQHelp.dwRgnStorage;
		pVMQ->dwRgnBlocks = VMQHelp.dwRgnBlocks;
		pVMQ->dwRgnGuardBlks = VMQHelp.dwRgnGuardBlks;
		pVMQ->bRgnIsAStack = VMQHelp.bRgnIsAStack;
		break;

	case MEM_COMMIT:     // ��Ԥ��
		pVMQ->pvRgnBaseAddress = mbi.AllocationBase;
		pVMQ->dwRgnProtection = mbi.AllocationProtect;

		// ���������ڿ�����ȡ����������Ϣ
		VMQueryHelp(hProcess, pvAddress, &VMQHelp);

		pVMQ->RgnSize = VMQHelp.RgnSize;
		pVMQ->dwRgnStorage = VMQHelp.dwRgnStorage;
		pVMQ->dwRgnBlocks = VMQHelp.dwRgnBlocks;
		pVMQ->dwRgnGuardBlks = VMQHelp.dwRgnGuardBlks;
		pVMQ->bRgnIsAStack = VMQHelp.bRgnIsAStack;
		break;

	default:
		DebugBreak();
		break;
	}

	return(bOk);
}
