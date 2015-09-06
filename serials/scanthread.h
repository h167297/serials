#ifndef SCANTHREAD_H
#define SCANTHREAD_H

#include <QObject>
#include <QThread>
#include <QtSerialPort/QtSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#ifdef Q_OS_LINUX
#include <serialsdispatcher.h>
#endif

class ScanThread :public QThread
{
    Q_OBJECT

public :
    ScanThread();

    void stop();

protected:
    void run();

signals:
    void portname_changed(const QStringList v_old_name,const QStringList v_new_name);
    void status_changed();
private:
    QStringList new_port_name;
    QStringList old_port_name;
    volatile bool stopped;
};

#endif // SCANTHREAD_H
