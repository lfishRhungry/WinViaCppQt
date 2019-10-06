#include "VMStat.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	VMStat w;
	w.show();
	return a.exec();
}
