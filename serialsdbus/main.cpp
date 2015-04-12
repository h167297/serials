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

#include "serialsAdaptor.h"
#include "serialsServer.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    serialsServer* server = new serialsServer;
    serialsAdaptor* adaptor = new serialsAdaptor(server);
    QObject::connect(server->serial,SIGNAL(readyRead()),adaptor,SLOT(Dbus_readSerial()));
    QDBusConnection connection = QDBusConnection::systemBus();
    connection.registerService("org.freedesktop.serials");
    connection.registerObject("/", server);
    return a.exec();
}
