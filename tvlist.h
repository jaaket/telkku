#ifndef TVLIST_H
#define TVLIST_H

#include <QObject>

class TVList : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString channel READ channel WRITE setChannel NOTIFY channelChanged)
public:
    explicit TVList(QObject *parent = 0);

    QString channel() const;
    void setChannel(const QString &channel);

private:
    QString m_channel;
};

#endif // TVLIST_H
