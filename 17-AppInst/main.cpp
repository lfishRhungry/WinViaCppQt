#include "AppInst.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	AppInst w;
	w.show();
	return a.exec();
}
