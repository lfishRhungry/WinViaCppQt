#include "MMFShare.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	MMFShare w;
	w.show();
	return a.exec();
}
