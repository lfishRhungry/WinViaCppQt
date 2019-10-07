#include "AppInst.h"
///////////////////////////////////////////////////////////////////////////////


// 告诉编译器将此变量放在共享内存段以供所有本程序实例共享
#pragma data_seg("Shared")
volatile LONG g_lApplicationInstances = 0; // 代表程序实例数量 由于线程同步 使用volatile
#pragma data_seg()

// 告诉链接器将此共享内存段设置为RWS权限
#pragma comment(linker, "/Section:Shared,RWS")


///////////////////////////////////////////////////////////////////////////////

AppInst::AppInst(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	// -----------------------界面设计-----------------------------
	this->setFixedSize(600, 100);

	ui.labelContent->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);
	ui.labelNum->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);
	ui.labelContent->setText("Number of instances running:");
	ui.labelNum->setText("");

	// -----------------------界面设计完毕-----------------------------

	// 将消息注册
	g_uMsgAppInstCountUpdate =
		RegisterWindowMessage(TEXT("MsgAppInstCountUpdate"));

	// 使用原子方式将count加1
	InterlockedExchangeAdd(&g_lApplicationInstances, 1);

	//这里加1之后本实例先设置好 否则会接受不到自己发送的广播消息
	ui.labelNum->setText(QString("%1").arg(g_lApplicationInstances));

	// 广播消息
	PostMessage(HWND_BROADCAST, g_uMsgAppInstCountUpdate, 0, 0);

}

AppInst::~AppInst() {

	// 使用原子方式将count减1
	InterlockedExchangeAdd(&g_lApplicationInstances, -1);

	// 广播消息
	PostMessage(HWND_BROADCAST, g_uMsgAppInstCountUpdate, 0, 0);
}

// 重写nativeEvent函数以处理windows窗口消息
bool AppInst::nativeEvent(const QByteArray& eventType, void* message, long* result) {

	MSG* msg = (MSG*)message;
	if (msg->message == g_uMsgAppInstCountUpdate) {
		ui.labelNum->setText(QString("%1").arg(g_lApplicationInstances));
	}

	return false;
}
