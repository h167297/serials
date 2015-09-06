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
    bool open();
    void clear();
    void setPortName(const QString &portname);
    void setBaudRate(const int &baudrate);
    void setDataBits(const int &databit);
    void setParity(const int &parity);
    void setStopBits(const int &stopbits);
    void setFlowControl(const int &flowcontrol);
    void close();
    void write(const int &data, const int &len);
    void write(const QByteArray &data);
    QByteArray readAll();
    bool isOpen();
    bool setDataTerminalReady(bool set);
    bool setRequestToSend(bool set);
    QSerialPort::PinoutSignals pinoutSignals();
public:
    QDBusInterface *iface;
};

#endif // DEMODISPATCHER_H
