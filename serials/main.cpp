#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
//    Linux上不管用，在main执行前就开始加载插件了
//    QCoreApplication::addLibraryPath("/usr/lib/serials");//只有这个路径存在时，调试输出才能看到。

    qDebug()<<"小提示：自动获取root权限......";
    MainWindow w;
    w.show();

    return a.exec();
}
