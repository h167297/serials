/*
 * Copyright (C) 2014 ~ 2015 HANGZHOU DIANZI UNIVERSITY(HDU).
 * Author: hening
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "serialsServer.h"

serialsServer::serialsServer(QObject *parent) :
    QObject(parent)
{
   serial = new QSerialPort;
}

serialsServer::~serialsServer()
{
    if(serial!=NULL)
        delete serial;
    serial = NULL;
}

void serialsServer::Dbus_serial_clear()
{
    serial->clear();
}

void serialsServer::Dbus_serial_close()
{
    serial->close();
    exit(0);
}

bool serialsServer::Dbus_serial_isOpen()
{
    return serial->isOpen();
}

bool serialsServer::Dbus_serial_open()
{
    if(serial->open(QIODevice::ReadWrite))
    {
        return true;
    }
    return false;
}

int serialsServer::Dbus_serial_pinoutSignals()
{
    return (int)serial->pinoutSignals();
}

void serialsServer::Dbus_serial_setBaudRate(int baudrate)
{
    serial->setBaudRate(baudrate);
}

void serialsServer::Dbus_serial_setDataBits(int databit)
{
    serial->setDataBits(static_cast<QSerialPort::DataBits>(databit));
}

bool serialsServer::Dbus_serial_setDataTerminalReady(bool set)
{
    return serial->setDataTerminalReady(set);
}

void serialsServer::Dbus_serial_setFlowControl(int flowcontrol)
{
    serial->setFlowControl(static_cast<QSerialPort::FlowControl>(flowcontrol));
}

void serialsServer::Dbus_serial_setParity(int parity)
{
    serial->setParity(static_cast<QSerialPort::Parity>(parity));
}

void serialsServer::Dbus_serial_setPortName(const QString &portname)
{
    serial->setPortName(portname);
}

bool serialsServer::Dbus_serial_setRequestToSend(bool set)
{
    return serial->setRequestToSend(set);
}

void serialsServer::Dbus_serial_setStopBits(int stopbits)
{
    serial->setStopBits(static_cast<QSerialPort::StopBits>(stopbits));
}

void serialsServer::Dbus_serial_write(const QByteArray &data)
{
    serial->write(data);
}

void serialsServer::Dbus_serial_write(int data, int len)
{
    char temp[1]={char(data)};
    serial->write(temp,len);
}

QByteArray serialsServer::Dbus_serial_readAll()
{
    return serial->readAll();
}
