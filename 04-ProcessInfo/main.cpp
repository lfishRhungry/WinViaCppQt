#include "ProcessInfo.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	ProcessInfo w;
	w.show();
	return a.exec();
}
