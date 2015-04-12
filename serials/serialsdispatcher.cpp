/*
 * Copyright (C) 2013 ~ 2014 National University of Defense Technology(NUDT) & Kylin Ltd.
 * Author: Kobe Lee
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
                               "/",
                               "org.freedesktop.serials",
                               QDBusConnection::systemBus());
}

serialsDispatcher::~serialsDispatcher() {
    if (iface != NULL) {
        delete iface;
        iface = NULL;
    }
}

bool serialsDispatcher::serial_open(){
    QDBusReply<bool> reply = iface->call("Dbus_serial_open");
    return reply.value();
}

void serialsDispatcher::serial_clear()
{
    iface->call("Dbus_serial_clear");
}

void serialsDispatcher::serial_setPortName(const QString &portname)
{
    iface->call("Dbus_serial_setPortName",portname);
}

void serialsDispatcher::serial_setBaudRate(int &baudrate)
{
    iface->call("Dbus_serial_setBaudRate",baudrate);
}

void serialsDispatcher::serial_setDataBits(const int &databit)
{
    iface->call("Dbus_serial_setDataBits",databit);
}

void serialsDispatcher::serial_setParity(const int &parity)
{
    iface->call("Dbus_serial_setParity",parity);
}

void serialsDispatcher::serial_setStopBits(const int &stopbits)
{
    iface->call("Dbus_serial_setStopBits",stopbits);
}

void serialsDispatcher::serial_setFlowControl(const int &flowcontrol)
{
    iface->call("Dbus_serial_setFlowControl",flowcontrol);
}

void serialsDispatcher::serial_close()
{
    iface->call("Dbus_serial_close");
}

void serialsDispatcher::serial_write(int data)
{
    iface->call("Dbus_serial_write",data);
}

void serialsDispatcher::serial_write(const QString &data)
{
    iface->call("Dbus_serial_write",data);
}

QByteArray serialsDispatcher::serial_readAll()
{
    QDBusReply<QByteArray> reply = iface->call("Dbus_serial_readAll");
    return reply.value();
}

