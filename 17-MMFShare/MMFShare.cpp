#include "MMFShare.h"

MMFShare::MMFShare(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	//-------------------------界面设计------------------------------
	this->setWindowTitle("Memory-Mapped File Sharing");
	this->setFixedSize(800, 300);

	ui.buttonCreate->setText("Create mapping of Data");
	ui.buttonClose->setText("Close mapping of Data");

	ui.labelData->setText("Data:");
	ui.lineEdit->setText("Some test Data");

	ui.buttonMap->setFixedWidth(400);
	ui.buttonMap->setText("Open mapping and get Data");
	//-------------------------界面设计完毕------------------------------

	ui.buttonClose->setEnabled(false);


	// 创建按钮主逻辑
	connect(ui.buttonCreate, &QPushButton::clicked, [=]() {
		do
		{
			// 创建一个命名内存映射文件内核对象来放置lineEdit内的值 至少为一页大小
			// 传入INVALID_HANDLE_VALUE表示不以物理存储器为后备存储器
			// 以页交换文件为后备存储器
			m_hFileMap = CreateFileMapping(INVALID_HANDLE_VALUE, NULL,
				PAGE_READWRITE, 0, 4 * 1024, TEXT("MMFSharedData"));

			if (m_hFileMap == NULL) {
				QMessageBox::critical(this, "Error", "Can't map view of file.");
				break;
			}

			if (GetLastError() == ERROR_ALREADY_EXISTS) {
				chMB("Mapping already exists - not created.");
				CloseHandle(m_hFileMap);
				break;
			}

			// 到这里内存映射文件对象已经创建成功了
			// 下面就要预订和调拨
			PVOID pView = MapViewOfFile(m_hFileMap,
				FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, 0);

			if (pView == NULL) {
				QMessageBox::critical(this, "Error", "Can't map view of file.");
				break;
			}

			// 将文本内容复制到内存映射文件内
			// 这里注意要填入字符数sizeInWords
			_tcscpy_s(
				(PTSTR)pView, 
				4 * 1024 / sizeof(WCHAR), 
				reinterpret_cast<const wchar_t*>(ui.lineEdit->text().utf16()));

			// 取消映射（但是没有关闭内核对象 是为了保护内存映射文件）
			UnmapViewOfFile(pView);

			// 禁止再次创建文件映射
			ui.buttonCreate->setEnabled(false);

			// 禁止在关闭本次内存映射对象之前改动内容
			ui.lineEdit->setReadOnly(true);

			// 用户可以关闭内存映射文件对象
			ui.buttonClose->setEnabled(true);

		} while (false);

		});

	// 关闭内存映射文件对象
	connect(ui.buttonClose, &QPushButton::clicked, [=]() {
		if (CloseHandle(m_hFileMap)) {
			ui.buttonCreate->setEnabled(true);
			ui.lineEdit->setReadOnly(false);
		}
		});

	// map逻辑
	connect(ui.buttonMap, &QPushButton::clicked, [=]() {

		// 查看是否有我们已经创建好的内存映射文件对象
		HANDLE hFileMapT = OpenFileMapping(FILE_MAP_READ | FILE_MAP_WRITE,
			FALSE, TEXT("MMFSharedData"));

		if (hFileMapT == NULL) {
			QMessageBox::critical(this, "Error", "Can't open mapping.");
			return;
		}

		// 将确实存在的内存映射文件map到本进程内存空间
		PVOID pView = MapViewOfFile(hFileMapT,
			FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, 0);

		if (pView == NULL) {
			QMessageBox::critical(this, "Error", "Can't map view.");
			return;
		}

		// 将映射进来的内容copy到lineedit里面
		ui.lineEdit->setText(QString::fromWCharArray((PTSTR)pView));

		UnmapViewOfFile(pView); // 取消映射

		CloseHandle(hFileMapT); // 关闭内核对象句柄

		});
}
