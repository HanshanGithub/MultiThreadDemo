#include "MultiThreadDemo.h"
#include <QtWidgets/QApplication>

int main(int argc, char* argv[])
{
	QApplication a(argc, argv);
	ThreadDemo::MultiThreadDemo w;
	w.show();
	return a.exec();
}
