#ifndef TELKKUDATA_H
#define TELKKUDATA_H

#include <QtGui>
#include <QtWebKit/QtWebKit>
#include "guidedata.h"

class TelkkuData : public GuideData
{
    Q_OBJECT

public:
    TelkkuData();

    void fetchData(const QStringList &channels, const QDate &date = QDate::currentDate());
    QString descriptionUrl(QString channel, int program) const;

private:
    QString m_requestedChannel;
    QDate m_requestedDate;
    QStringList m_channelsToRequest;

    QStringList m_downloadedChannels;

    QString buildUrl(const QString &channel, QDate date) const;

    QHash<QString, int> m_channelNumbers;

    QWebPage m_page;
    QWebPage m_descriptionPage;

    QNetworkAccessManager *m_nam;

private slots:
    void parseData();
};

#endif // TELKKUDATA_H
