/******************************************************************************
Jeffrey Richter��virtualQuery��������չ�汾
******************************************************************************/
#include "../../CommonFiles/CmnHdr.h"
#include <windowsx.h>

// �Զ���һ��VMQUERY�ṹ �һ���VMQuery�������
typedef struct {
	// ������Ϣ
	PVOID  pvRgnBaseAddress; // ���������ʼ��ַ
	DWORD  dwRgnProtection;  // �ʼԤ��������ʱָ���ı�������
	SIZE_T RgnSize;          // ��Ԥ�������С���ֽڵ�λ��
	DWORD  dwRgnStorage;     // �洢�����ͣ�free image mapped private��
	DWORD  dwRgnBlocks;      // �����п������
	DWORD  dwRgnGuardBlks;   // ��������PAGE_GUARD���Կ�������λ�߳�ջԤ����
	BOOL   bRgnIsAStack;     // �������Ƿ�����߳�ջ����һ���²�ֵ��

	// ����Ϣ
	PVOID  pvBlkBaseAddress; // ����ʼ��ַ
	DWORD  dwBlkProtection;  // �鱣������
	SIZE_T BlkSize;          // ���С���ֽڵ�λ��
	DWORD  dwBlkStorage;     // ��Ĵ洢������
} VMQUERY, * PVMQUERY;


///////////////////////////////////////////////////////////////////////////////


BOOL VMQuery(HANDLE hProcess, LPCVOID pvAddress, PVMQUERY pVMQ);


//////////////////////////////// End of File //////////////////////////////////
