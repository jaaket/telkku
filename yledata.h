#ifndef YLEDATA_H
#define YLEDATA_H

#include <QtGui>
#include <QtNetwork>
#include "guidedata.h"

class YLEData : public GuideData
{
    Q_OBJECT

public:
    YLEData();
    ~YLEData();

    void fetchData(const QStringList &channels, const QDate &date = QDate::currentDate());

private:
    QNetworkAccessManager *m_nam;

    QString m_requestedChannel;
    QDate m_requestedDate;
    QStringList m_channelsToRequest;

    QString buildUrl(const QString &channel, QDate date) const;

private slots:
    void networkReply(QNetworkReply *reply);

};

#endif // YLEDATA_H
