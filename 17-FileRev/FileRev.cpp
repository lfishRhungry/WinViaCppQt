#include "FileRev.h"

// 保存文件名称
#define FILENAME  TEXT("FileRev.dat")

FileRev::FileRev(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	// -----------------------界面设计-------------------------
	this->setFixedSize(800, 200);

	ui.labelPath->setText("Pathname:");

	ui.lineEdit->setReadOnly(true);

	ui.buttonBrowse->setText("Browse");

	ui.buttonReverse->setText("Reverse file contents");

	ui.buttonReverse->setEnabled(false);

	ui.labelChar->setText("Type of characters in file:");

	ui.labelType->setText("Unknown");

	ui.labelType->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);
	// -----------------------界面设计完毕-------------------------

	// browse文件路径
	connect(ui.buttonBrowse, &QPushButton::clicked, [=]() {
		ui.lineEdit->setText(
			QFileDialog::getOpenFileName(this, "Choose a txt file", "C:/Users/lfish/Desktop", "(*.txt)"));
		});

	connect(ui.lineEdit, &QLineEdit::textChanged, [=](const QString& text) {

		if (text.isEmpty()) {
			ui.buttonReverse->setEnabled(false);
		}
		else
		{
			ui.buttonReverse->setEnabled(true);
		}
		});

	// reverse主逻辑
	connect(ui.buttonReverse, &QPushButton::clicked, [=]() {

		const wchar_t* wstrPathName = reinterpret_cast<const wchar_t*>(ui.lineEdit->text().utf16());
		
		// 为了将成果保存在文件同级目录 获取文件所在目录 并改变程序工作目录
		PTCHAR psPath[_MAX_DIR];
		_wsplitpath(wstrPathName, nullptr, (wchar_t*)psPath, nullptr, nullptr);
		SetCurrentDirectory((LPCWSTR)psPath);

		// 先复制一份文件 使用复制 文件来操作 以免损坏源文件
		if (!CopyFile(wstrPathName, FILENAME, FALSE)) {
			QMessageBox::critical(this, "Error", "New file could not be created.");
			return;
		}

		BOOL bIsTextUnicode;
		if (FileReverse(FILENAME, &bIsTextUnicode)) {
			// 更新编码类型
			ui.labelType->setText(QString::fromWCharArray(bIsTextUnicode ? TEXT("Unicode") : TEXT("ANSI")));

			// 打开记事本查看我们的成果
			STARTUPINFO si = { sizeof(si) };
			PROCESS_INFORMATION pi;
			TCHAR sz[] = TEXT("Notepad ") FILENAME; // 设置好命令行
			if (CreateProcess(NULL, sz,
				NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {

				CloseHandle(pi.hThread);
				CloseHandle(pi.hProcess);
			}
		}
		});
	
}

// 反转文件的函数
BOOL FileRev::FileReverse(PCTSTR pszPathname, PBOOL pbIsTextUnicode) {

	*pbIsTextUnicode = FALSE;  // 假设时Unicode

	// 打开文件
	HANDLE hFile = CreateFile(pszPathname, GENERIC_WRITE | GENERIC_READ, 0,
		NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (hFile == INVALID_HANDLE_VALUE) {
		QMessageBox::critical(this, "Error", "File could not be opened");
		return(FALSE);
	}

	// 获取文件大小（假设文件时可以被完全映射的）
	DWORD dwFileSize = GetFileSize(hFile, NULL);

	// 创建文件映射对象 其大小设置为比文件大一个零字节 以便以零结尾
	// 因为我们不知道文件时ANSI还是Unicode 就放一个wchar大小的字节
	// 注意 这里仅仅是创建了一个文件映射内核对象
	HANDLE hFileMap = CreateFileMapping(hFile, NULL, PAGE_READWRITE,
		0, dwFileSize + sizeof(WCHAR), NULL);

	if (hFileMap == NULL) {
		QMessageBox::critical(this, "Error", "File map could not be opened.");
		CloseHandle(hFile);
		return(FALSE);
	}

	// 为该文件映射内核对象预订并调拨空间 并获取首地址
	PVOID pvFile = MapViewOfFile(hFileMap, FILE_MAP_WRITE, 0, 0, 0);

	if (pvFile == NULL) {
		QMessageBox::critical(this, "Error", "Could not map view of file.");
		CloseHandle(hFileMap);
		CloseHandle(hFile);
		return(FALSE);
	}

	// 测试是ANSI还是Unicode
	int iUnicodeTestFlags = -1;
	*pbIsTextUnicode = IsTextUnicode(pvFile, dwFileSize, &iUnicodeTestFlags);

	if (!*pbIsTextUnicode) {
		// 文件时ANSI

		// 在最后放入零
		PSTR pchANSI = (PSTR)pvFile;
		pchANSI[dwFileSize / sizeof(CHAR)] = 0;

		// 文件逆转(想不到还有这样的API)
		_strrev(pchANSI);

		// 但是要把所有被无脑逆转成"\n\r"的地方转会"\r\n"
		pchANSI = strstr(pchANSI, "\n\r");

		while (pchANSI != NULL) {
			*pchANSI++ = '\r';   // 先等于后加加 懂了吧
			*pchANSI++ = '\n';
			pchANSI = strstr(pchANSI, "\n\r"); // 下一个
		}

	}
	else {
		// Unicode的情况

		// 结尾置零
		PWSTR pchUnicode = (PWSTR)pvFile;
		pchUnicode[dwFileSize / sizeof(WCHAR)] = 0;

		// 判断是否是带BOM(byte-order-mark)的Unicode
		if ((iUnicodeTestFlags & IS_TEXT_UNICODE_SIGNATURE) != 0) {
			// 是就跳过
			pchUnicode++;
		}

		// 反转
		_wcsrev(pchUnicode);

		// 和上面一样的操作
		pchUnicode = wcsstr(pchUnicode, L"\n\r");

		while (pchUnicode != NULL) {
			*pchUnicode++ = L'\r';
			*pchUnicode++ = L'\n';
			pchUnicode = wcsstr(pchUnicode, L"\n\r");
		}
	}

	// 反转完毕 对该文件映射的修改就是对源文件的修改
	// 处理后事
	UnmapViewOfFile(pvFile);
	CloseHandle(hFileMap);

	// 由于映射文件的时候多映射了一个存放结尾零字节的大小
	// 这里把它截断
	SetFilePointer(hFile, dwFileSize, NULL, FILE_BEGIN);
	SetEndOfFile(hFile);
	CloseHandle(hFile);

	return(TRUE);
}
