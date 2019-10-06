#include "ErrorShow.h"

ErrorShow::ErrorShow(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	// 设置窗口名
	this->setWindowTitle("Error Show"); // 窗口名称
	this->setFixedSize(650, 200); // 固定窗口大小
	ui.plainTextEdit->setReadOnly(true); // 关闭plaintext写功能
	ui.pushButton->setFixedWidth(200); // 设置按钮大小
	ui.lineEdit->setFixedWidth(120); // 设置写入框大小
	// 限制输入框只能输入数字
	ui.lineEdit->setValidator(new QIntValidator(this));
	// 设置输入框对齐
	ui.lineEdit->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);
	// 只有输入框中有内容才能点击查询按钮
	ui.pushButton->setEnabled(false);

	// 勾选置顶实现窗口置顶
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

	// 在输入框回车相当于按下查询键
	connect(ui.lineEdit, &QLineEdit::returnPressed, ui.pushButton, &QPushButton::click);

	/*------------------------------------------------------------------------*/

	// 点击查询按钮 查询数据
	// 核心逻辑比较简单 就直接写在lambda函数中了
	connect(ui.pushButton, &QPushButton::clicked, [=]() {
		// 拿到用户输入的错误码
		// 转换为dword十进制
		bool ok; // 转换成功检查标志
		DWORD dwError = ui.lineEdit->text().toUInt(&ok, 10);
		// 指向存储错误信息的指针
		// 由于FormatMessage函数指定了FORMAT_MESSAGE_ALLOCATE_BUFFER
		// 标志 会自动分配所需大小堆空间（使用LocalAlloc分配堆）
		// 所以这里我们就只需要一个指针而已
		HLOCAL hlocal = nullptr;
		// 设置为本地语言
		DWORD systemLocale = MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL);
		// 拿到错误信息的语言描述
		// 这里注意 指定FORMAT_MESSAGE_ALLOCATE_BUFFER时
		// 第五个参数就不是已经定义好的缓冲区地址了
		// 而是定义的空指针取地址 因为此时是修改指针本身内容
		// 而不是修改原来的指针所指缓冲区内容
		DWORD fdwOk = FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM |
			FORMAT_MESSAGE_IGNORE_INSERTS |
			FORMAT_MESSAGE_ALLOCATE_BUFFER,
			nullptr,
			dwError,
			systemLocale,
			reinterpret_cast<LPTSTR>(&hlocal),
			0,
			nullptr);
		// 没有拿到错误信息，检查是否是网络关联类错误
		if (!fdwOk) {
			// 导入网络动态库 在其中寻找错误信息
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
				FreeLibrary(hDll); // 记得释放动态库
			}
		}

		if (fdwOk && (hlocal != nullptr)) {
			ui.plainTextEdit->setPlainText(QString::fromWCharArray(static_cast<const TCHAR*>(hlocal)));
			LocalFree(hlocal); // 释放堆空间
		}
		else {
			ui.plainTextEdit->setPlainText("No text found for this error number.");
		}
		});
}
