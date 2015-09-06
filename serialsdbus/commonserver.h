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

#ifndef COMMONSERVER_H
#define COMMONSERVER_H
#include <QSettings>
#include "commonAdaptor.h"

class commonServer: public QObject
{
    Q_OBJECT
public:
    commonServer(QObject *parent = 0);
    ~commonServer();
public slots:    //一定要时公用槽，否则QMetaObject::invokeMethod不能调用该函数
    void Dbus_wSettings(const QString &path);
    QRect Dbus_wSettings_RectValue(const QString &key);
    void Dbus_wSettings_setValue(const QString &key, int x, int y, int w, int h);
    void Dbus_wSettings_setValue(const QString &key, const int &value);
    void Dbus_wSettings_setValue(const QString &key, bool value);
    void Dbus_wSettings_setValue(const QString &key, const QString &value);
    QDBusVariant Dbus_wSettings_value(const QString &key);
private:
    QSettings *wSettings;
};

#endif // COMMONSERVER_H
