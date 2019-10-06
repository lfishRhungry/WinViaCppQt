#include "AppInst.h"
///////////////////////////////////////////////////////////////////////////////


// ���߱��������˱������ڹ����ڴ���Թ����б�����ʵ������
#pragma data_seg("Shared")
volatile LONG g_lApplicationInstances = 0; // �������ʵ������ �����߳�ͬ�� ʹ��volatile
#pragma data_seg()

// �������������˹����ڴ������ΪRWSȨ��
#pragma comment(linker, "/Section:Shared,RWS")


///////////////////////////////////////////////////////////////////////////////

AppInst::AppInst(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	// -----------------------�������-----------------------------
	this->setFixedSize(600, 100);

	ui.labelContent->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);
	ui.labelNum->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);
	ui.labelContent->setText("Number of instances running:");
	ui.labelNum->setText("");

	// -----------------------����������-----------------------------

	// ����Ϣע��
	g_uMsgAppInstCountUpdate =
		RegisterWindowMessage(TEXT("MsgAppInstCountUpdate"));

	// ʹ��ԭ�ӷ�ʽ��count��1
	InterlockedExchangeAdd(&g_lApplicationInstances, 1);

	//�����1֮��ʵ�������ú� �������ܲ����Լ����͵Ĺ㲥��Ϣ
	ui.labelNum->setText(QString("%1").arg(g_lApplicationInstances));

	// �㲥��Ϣ
	PostMessage(HWND_BROADCAST, g_uMsgAppInstCountUpdate, 0, 0);

}

AppInst::~AppInst() {

	// ʹ��ԭ�ӷ�ʽ��count��1
	InterlockedExchangeAdd(&g_lApplicationInstances, -1);

	// �㲥��Ϣ
	PostMessage(HWND_BROADCAST, g_uMsgAppInstCountUpdate, 0, 0);
}

// ��дnativeEvent�����Դ���windows������Ϣ
bool AppInst::nativeEvent(const QByteArray& eventType, void* message, long* result) {

	MSG* msg = (MSG*)message;
	if (msg->message == g_uMsgAppInstCountUpdate) {
		ui.labelNum->setText(QString("%1").arg(g_lApplicationInstances));
	}

	return false;
}
