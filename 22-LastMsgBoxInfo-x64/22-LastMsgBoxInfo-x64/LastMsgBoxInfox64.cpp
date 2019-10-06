#include "LastMsgBoxInfox64.h"
#include "../../CommonFiles/CmnHdr.h"
#include <windowsx.h>
#include <tchar.h>
#include "../../22-LastMsgBoxInfoLib/22-LastMsgBoxInfoLib/22-LastMsgBoxInfoLib.h"
#pragma comment(lib, "22-LastMsgBoxInfoLib.lib")

LastMsgBoxInfox64::LastMsgBoxInfox64(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	//---------------------------�������-------------------------------
	this->setMinimumSize(800, 400);
	// ע�Ⱑ��������
	// ����Ĵ��ڱ���һ��Ҫ��LastMsgBoxInfoLib���淢����Ϣ�����ֶ�Ӧ
	// ��������ղ���WM_COPYDATA ���޷�������Ϣ��
	this->setWindowTitle("Last MessageBox Info");

	ui.plainTextEdit->setReadOnly(true);
	ui.plainTextEdit->setPlainText("Waiting for a Message Box to be dismissed");
	ui.plainTextEdit->setWordWrapMode(QTextOption::NoWrap);  // ��Ӻ������
	//---------------------------����������-------------------------------

	DWORD dwThreadId = 0;
	LastMsgBoxInfo_HookAllApps(TRUE, dwThreadId);
}

LastMsgBoxInfox64::~LastMsgBoxInfox64() {
	LastMsgBoxInfo_HookAllApps(FALSE, 0);
}

bool LastMsgBoxInfox64::nativeEvent(const QByteArray& eventType, void* message, long* result) {

	MSG* msg = (MSG*)message;
	if (msg->message == WM_COPYDATA) {
		PCOPYDATASTRUCT pcds = reinterpret_cast<PCOPYDATASTRUCT>(msg->lParam);
		ui.plainTextEdit->appendPlainText(QString::fromWCharArray((PCWSTR)pcds->lpData));
	}

	return false;
}
