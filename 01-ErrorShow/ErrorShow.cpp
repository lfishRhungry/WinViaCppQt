#include "ErrorShow.h"

ErrorShow::ErrorShow(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	// ���ô�����
	this->setWindowTitle("Error Show"); // ��������
	this->setFixedSize(650, 200); // �̶����ڴ�С
	ui.plainTextEdit->setReadOnly(true); // �ر�plaintextд����
	ui.pushButton->setFixedWidth(200); // ���ð�ť��С
	ui.lineEdit->setFixedWidth(120); // ����д����С
	// ���������ֻ����������
	ui.lineEdit->setValidator(new QIntValidator(this));
	// ������������
	ui.lineEdit->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);
	// ֻ��������������ݲ��ܵ����ѯ��ť
	ui.pushButton->setEnabled(false);

	// ��ѡ�ö�ʵ�ִ����ö�
	connect(ui.checkBox, &QCheckBox::stateChanged, [=](int state) {
		if (state) {
			this->setWindowFlags(Qt::WindowStaysOnTopHint);
			this->show();
		}
		else {
			this->setWindowFlags(nullptr);
			this->show();
		}
		});

	connect(ui.lineEdit, &QLineEdit::textChanged, [=](const QString& text) {
		if (text.isEmpty())
			ui.pushButton->setEnabled(false);
		else
			ui.pushButton->setEnabled(true);
		});

	// �������س��൱�ڰ��²�ѯ��
	connect(ui.lineEdit, &QLineEdit::returnPressed, ui.pushButton, &QPushButton::click);

	/*------------------------------------------------------------------------*/

	// �����ѯ��ť ��ѯ����
	// �����߼��Ƚϼ� ��ֱ��д��lambda��������
	connect(ui.pushButton, &QPushButton::clicked, [=]() {
		// �õ��û�����Ĵ�����
		// ת��Ϊdwordʮ����
		bool ok; // ת���ɹ�����־
		DWORD dwError = ui.lineEdit->text().toUInt(&ok, 10);
		// ָ��洢������Ϣ��ָ��
		// ����FormatMessage����ָ����FORMAT_MESSAGE_ALLOCATE_BUFFER
		// ��־ ���Զ����������С�ѿռ䣨ʹ��LocalAlloc����ѣ�
		// �����������Ǿ�ֻ��Ҫһ��ָ�����
		HLOCAL hlocal = nullptr;
		// ����Ϊ��������
		DWORD systemLocale = MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL);
		// �õ�������Ϣ����������
		// ����ע�� ָ��FORMAT_MESSAGE_ALLOCATE_BUFFERʱ
		// ����������Ͳ����Ѿ�����õĻ�������ַ��
		// ���Ƕ���Ŀ�ָ��ȡ��ַ ��Ϊ��ʱ���޸�ָ�뱾������
		// �������޸�ԭ����ָ����ָ����������
		DWORD fdwOk = FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM |
			FORMAT_MESSAGE_IGNORE_INSERTS |
			FORMAT_MESSAGE_ALLOCATE_BUFFER,
			nullptr,
			dwError,
			systemLocale,
			reinterpret_cast<LPTSTR>(&hlocal),
			0,
			nullptr);
		// û���õ�������Ϣ������Ƿ���������������
		if (!fdwOk) {
			// �������綯̬�� ������Ѱ�Ҵ�����Ϣ
			HMODULE hDll = LoadLibraryEx(TEXT("netmsg.dll"), nullptr, DONT_RESOLVE_DLL_REFERENCES);
			if (hDll != nullptr) {
				fdwOk = FormatMessage(FORMAT_MESSAGE_FROM_HMODULE |
					FORMAT_MESSAGE_IGNORE_INSERTS |
					FORMAT_MESSAGE_ALLOCATE_BUFFER,
					hDll,
					dwError,
					systemLocale,
					reinterpret_cast<LPTSTR>(&hlocal),
					0,
					nullptr);
				FreeLibrary(hDll); // �ǵ��ͷŶ�̬��
			}
		}

		if (fdwOk && (hlocal != nullptr)) {
			ui.plainTextEdit->setPlainText(QString::fromWCharArray(static_cast<const TCHAR*>(hlocal)));
			LocalFree(hlocal); // �ͷŶѿռ�
		}
		else {
			ui.plainTextEdit->setPlainText("No text found for this error number.");
		}
		});
}
