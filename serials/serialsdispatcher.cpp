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

#include "serialsdispatcher.h"

serialsDispatcher::serialsDispatcher(QObject *parent) :
    QObject(parent)
{
    iface = new QDBusInterface("org.freedesktop.serials",
                               "/org/freedesktop/port",
                               "org.freedesktop.port",
                               QDBusConnection::systemBus());
}

serialsDispatcher::~serialsDispatcher() {
    if (iface != NULL) {
        delete iface;
        iface = NULL;
    }
}

bool serialsDispatcher::open(){
    QDBusReply<bool> reply = iface->call("Dbus_serial_open");
    return reply.value();
}

void serialsDispatcher::clear()
{
    iface->call("Dbus_serial_clear");
}

void serialsDispatcher::setPortName(const QString &portname)
{
    iface->call("Dbus_serial_setPortName",portname);
}

void serialsDispatcher::setBaudRate(const int &baudrate)
{
    iface->call("Dbus_serial_setBaudRate",baudrate);
}

void serialsDispatcher::setDataBits(const int &databit)
{
    iface->call("Dbus_serial_setDataBits",databit);
}

void serialsDispatcher::setParity(const int &parity)
{
    iface->call("Dbus_serial_setParity",parity);
}

void serialsDispatcher::setStopBits(const int &stopbits)
{
    iface->call("Dbus_serial_setStopBits",stopbits);
}

void serialsDispatcher::setFlowControl(const int &flowcontrol)
{
    iface->call("Dbus_serial_setFlowControl",flowcontrol);
}

void serialsDispatcher::close()
{
    iface->call("Dbus_serial_close");
}

void serialsDispatcher::write(const int &data, const int &len)
{
    iface->call("Dbus_serial_write",data,len);
}

void serialsDispatcher::write(const QByteArray &data)
{
    iface->call("Dbus_serial_write",data);
}

QByteArray serialsDispatcher::readAll()
{
    QDBusReply<QByteArray> reply = iface->call("Dbus_serial_readAll");
    return reply.value();
}

bool serialsDispatcher::isOpen()
{
    QDBusReply<bool> reply = iface->call("Dbus_serial_isOpen");
    return reply.value();
}

bool serialsDispatcher::setDataTerminalReady(bool set)
{
    QDBusReply<bool> reply = iface->call("Dbus_serial_setDataTerminalReady",set);
    return reply.value();
}

bool serialsDispatcher::setRequestToSend(bool set)
{
    QDBusReply<bool> reply = iface->call("Dbus_serial_setRequestToSend",set);
    return reply.value();
}

QSerialPort::PinoutSignals serialsDispatcher::pinoutSignals()
{
    QDBusReply<int> reply = iface->call("Dbus_serial_pinoutSignals");
    return (QSerialPort::PinoutSignals)reply.value();
}

