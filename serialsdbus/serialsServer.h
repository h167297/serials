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

#ifndef SERIALSSERVER_H
#define SERIALSSERVER_H

#include <QtSerialPort/QSerialPort>
#include <QObject>
#include "serialsAdaptor.h"

class serialsServer : public QObject
{
    Q_OBJECT
public:
    explicit serialsServer(QObject *parent = 0);
     ~serialsServer();
public slots:
    void Dbus_serial_clear();
    void Dbus_serial_close();
    bool Dbus_serial_open();
    void Dbus_serial_setBaudRate(int baudrate);
    void Dbus_serial_setDataBits(int databit);
    void Dbus_serial_setFlowControl(int flowcontrol);
    void Dbus_serial_setParity(int parity);
    void Dbus_serial_setPortName(const QString &portname);
    void Dbus_serial_setStopBits(int stopbits);
    void Dbus_serial_write(const QString &data);
    void Dbus_serial_write(int data);
    QByteArray Dbus_serial_readAll();
public:
    QSerialPort *serial;
};

#endif // SERIALSSERVER_H
