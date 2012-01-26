#ifndef GuideData_H
#define GuideData_H

#include <QtGui>
#include <QtSql>
#include "tvshow.h"

class GuideData : public QObject
{
    Q_OBJECT
public:
    GuideData();

    virtual void fetchData(const QStringList &channels,
                           const QDate &date = QDate::currentDate()) = 0;

    QList<TVShow> getListing(const QString &channel) const;

    const QStringList& availableChannels() const;

    virtual QString descriptionUrl(QString channel, int program) const = 0;

    QDateTime lastUpdated(QString channel) const;

    void saveChannel(QString channel);
    void saveDatabase();
    void loadChannel(QString channel);
    void loadDatabase();

protected:
    QStringList m_availableChannels;
    QHash<QString, QList<TVShow> > m_listings;

    bool m_busy;

signals:
    void startedFetch(QString channel);
    void channelReady(QString channel);
};

#endif // GuideData_H
