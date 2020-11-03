#include "VisionDemo.h"
#include <QtWidgets/QApplication>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    VisionDemo w;
    w.show();
    return a.exec();
}
