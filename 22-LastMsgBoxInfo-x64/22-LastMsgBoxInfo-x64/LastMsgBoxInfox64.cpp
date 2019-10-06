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
	//---------------------------界面设计-------------------------------
	this->setMinimumSize(800, 400);
	// 注意啊！！！！
	// 这里的窗口标题一定要和LastMsgBoxInfoLib里面发送消息的名字对应
	// 否则程序收不到WM_COPYDATA 就无法更新信息了
	this->setWindowTitle("Last MessageBox Info");

	ui.plainTextEdit->setReadOnly(true);
	ui.plainTextEdit->setPlainText("Waiting for a Message Box to be dismissed");
	ui.plainTextEdit->setWordWrapMode(QTextOption::NoWrap);  // 添加横向滚轮
	//---------------------------界面设计完毕-------------------------------

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
