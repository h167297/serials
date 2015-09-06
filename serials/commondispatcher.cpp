#include "commondispatcher.h"
#include <QDBusMetaType>
commondispatcher::commondispatcher(const QString &fileName, QObject *parent) :
    QObject(parent)
{
    qDBusRegisterMetaType<QRect>();//在总线上注册QRect类型，使之能在总线上传递

    interface = new QDBusInterface("org.freedesktop.serials",
                                   "/org/freedesktop/common",
                                   "org.freedesktop.common",
                                   QDBusConnection::systemBus());
    interface->call("Dbus_wSettings",fileName);
}

commondispatcher::~commondispatcher()
{
    if( interface != NULL )
    {
        delete interface;
        interface = NULL;
    }
}

void commondispatcher::setValue(const QString &key, const QString &value)
{
    interface->call("Dbus_wSettings_setValue",key,value);
}

void commondispatcher::setValue(const QString &key, const bool &value)
{
    interface->call("Dbus_wSettings_setValue",key,value);
}

void commondispatcher::setValue(const QString &key, const QRect &value)
{
    int x = value.x();
    int y = value.y();
    int w = value.width();
    int h = value.height();

    interface->call("Dbus_wSettings_setValue",key,x,y,w,h);
}

void commondispatcher::setValue(const QString &key, const int &value)
{
     interface->call("Dbus_wSettings_setValue",key,value);
}

QVariant commondispatcher::value(const QString &key)
{
    QDBusReply<QDBusVariant> reply = interface->call("Dbus_wSettings_value",key);

    if( QString(reply.value().variant().typeName()) == "QDBusArgument" )
    {
        QDBusReply<QRect> reply = interface->call("Dbus_wSettings_RectValue",key);
        QVariant value(reply.value());
        return value;
    }

    return reply.value().variant();
}
