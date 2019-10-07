#include "LastMsgBoxInfo.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	LastMsgBoxInfo w;
	w.show();
	return a.exec();
}
