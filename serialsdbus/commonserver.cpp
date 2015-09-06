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

#include "commonserver.h"

commonServer::commonServer(QObject *parent):
    QObject(parent)
{
}

commonServer::~commonServer()
{
    if(wSettings != NULL)
    {
        delete wSettings;
        wSettings = NULL;
    }
}

void commonServer::Dbus_wSettings(const QString &path)
{
    wSettings = new QSettings(path,QSettings::IniFormat);
}

QRect commonServer::Dbus_wSettings_RectValue(const QString &key)
{
    return (wSettings->value(key).toRect());
}

void commonServer::Dbus_wSettings_setValue(const QString &key, int x, int y, int w, int h)
{
    QRect value(x,y,w,h);
    wSettings->setValue(key,value);
}

void commonServer::Dbus_wSettings_setValue(const QString &key, const int &value)
{
    wSettings->setValue(key,value);
}

void commonServer::Dbus_wSettings_setValue(const QString &key, bool value)
{
    wSettings->setValue(key,value);
}

void commonServer::Dbus_wSettings_setValue(const QString &key, const QString &value)
{
    wSettings->setValue(key,value);
}

QDBusVariant commonServer::Dbus_wSettings_value(const QString &key)
{
    QDBusVariant variant(wSettings->value(key));
    return variant;
}
