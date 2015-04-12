#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    QCoreApplication::addLibraryPath("/usr/lib/serials");//增加插件路径。只有这个路径存在时，调试输出才能看到。
    w.show();

    return a.exec();
}
