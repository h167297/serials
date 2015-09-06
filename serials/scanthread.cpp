#include "scanthread.h"

ScanThread::ScanThread()
    :stopped(false)
{

}

void ScanThread::run()
{
    while(!stopped)
    {
        new_port_name.clear();
        foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
           new_port_name.append(info.portName());

        if(new_port_name!=old_port_name)
        {
            emit portname_changed(old_port_name,new_port_name);
            old_port_name = new_port_name;
        }
    }
}

void ScanThread::stop()
{
    stopped = true;
}
