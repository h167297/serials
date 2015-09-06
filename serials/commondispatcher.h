#ifndef COMMONDISPATCHER_H
#define COMMONDISPATCHER_H

#include <QObject>

class commondispatcher : public QObject
{
    Q_OBJECT
public:
    explicit commondispatcher(QObject *parent = 0);

signals:

public slots:

};

#endif // COMMONDISPATCHER_H
