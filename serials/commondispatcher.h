#ifndef COMMONDISPATCHER_H
#define COMMONDISPATCHER_H

#include <QObject>
#include <QDBusInterface>
#include <QDBusConnection>
#include <QtDBus>
#include <QSettings>
#include <QDBusArgument>

class commondispatcher : public QObject
{
    Q_OBJECT
public:
    explicit commondispatcher(const QString &fileName,QObject *parent = 0);
    ~commondispatcher();
    void setValue(const QString &key, const QString &value);
    void setValue(const QString &key, const bool &value);
    void setValue(const QString &key, const QRect &value);
    void setValue(const QString &key, const int &value);
    QVariant value(const QString &key);
private:
    QDBusInterface *interface;
};

#endif // COMMONDISPATCHER_H
