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
#include "commonAdaptor.h"
#include "commonserver.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    serialsServer* server = new serialsServer;
    serialsAdaptor* adaptor = new serialsAdaptor(server);
    QObject::connect(server->serial,SIGNAL(readyRead()),adaptor,SLOT(Dbus_readSerial()));

    commonServer *c_server = new commonServer;
    commonAdaptor *c_adaptor = new commonAdaptor(c_server);//这句一定需要，否则DBUS总线上不会有接口
    c_adaptor = c_adaptor;//就是防止编译器出警告而已
    QDBusConnection connection = QDBusConnection::systemBus();
    connection.registerService("org.freedesktop.serials");//注册服务，conf文件夹中的三个文件中的名称
    connection.registerObject("/org/freedesktop/port", server);//注册对象
    connection.registerObject("/org/freedesktop/common", c_server);//注册对象

    return a.exec();
}
