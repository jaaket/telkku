#ifndef GuideData_H
#define GuideData_H

#include <QtGui>
#include <QtSql>
#include "tvshow.h"

class QNetworkAccessManager;
class QNetworkReply;

class GuideData : public QObject
{
    Q_OBJECT
public:
    GuideData();
    ~GuideData();

    void fetchData(const QStringList &channels,
                   const QDate &date = QDate::currentDate());

    const QStringList& availableChannels() const;

    QList<TVShow> getListing(const QString &channel) const;

    QDateTime lastUpdated(QString channel) const;

    void saveChannel(QString channel);
    void saveDatabase();
    void loadChannel(QString channel);
    void loadDatabase();

protected:
    QStringList m_availableChannels;
    QHash<QString, int> m_channelNumbers;
    QHash<QString, QList<TVShow> > m_listings;

    bool m_busy;

    QDate m_requestedDate;
    QStringList m_channelsToRequest;

    QNetworkAccessManager *m_nam;

    QString buildUrl(const QString &channel, QDate date) const;

signals:
    void startedFetch(QString channel);
    void channelReady(QString channel);

private slots:
    void parseData(QNetworkReply *reply);
};

#endif // GuideData_H
