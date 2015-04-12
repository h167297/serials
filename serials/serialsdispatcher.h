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

#ifndef DEMODISPATCHER_H
#define DEMODISPATCHER_H

#include <QObject>
#include <QDBusInterface>
#include <QDBusConnection>
#include <QtDBus>
#include <QtSerialPort/QtSerialPort>

class serialsDispatcher : public QObject
{
    Q_OBJECT
public:
    explicit serialsDispatcher(QObject *parent = 0);
    ~serialsDispatcher();
    bool serial_open();
    void serial_clear();
    void serial_setPortName(const QString &portname);
    void serial_setBaudRate(int &baudrate);
    void serial_setDataBits(const int &databit);
    void serial_setParity(const int &parity);
    void serial_setStopBits(const int &stopbits);
    void serial_setFlowControl(const int &flowcontrol);
    void serial_close();
    void serial_write(int data);
    void serial_write(const QString &data);
    QByteArray serial_readAll();
public:
    QDBusInterface *iface;
};

#endif // DEMODISPATCHER_H
