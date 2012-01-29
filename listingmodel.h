#ifndef LISTINGMODEL_H
#define LISTINGMODEL_H

#include <QtGui>
#include <QAbstractListModel>
#include "guidedata.h"

class ListingModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(QString channel READ channel WRITE setChannel NOTIFY channelChanged)
    Q_PROPERTY(QObject *guideData READ guideData WRITE setGuideData NOTIFY guideDataChanged)
    Q_PROPERTY(bool loading READ loading NOTIFY loadingChanged)

    enum Roles {
        NameRole = Qt::UserRole + 1,
        StartTimeRole,
        EndTimeRole,
        StartTimeSecsRole,
        ChannelRole,
        LengthRole,
        DayOffsetRole
    };

public:

    explicit ListingModel(QObject *parent = 0);
    ~ListingModel();

    Q_INVOKABLE void fetchData();

    int rowCount(const QModelIndex &parent = QModelIndex()) const;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

    void setGuideData(QObject* guideData);
    GuideData* guideData() const;

    void setChannel(const QString& channel);
    QString channel() const;

    bool loading();

    double startTime() const;

    static void setDefaultGuideData(GuideData *guideData);

private:
    QString m_channel;
    GuideData* m_guideData;

    bool m_loading;

    static GuideData *defaultGuideData;

public slots:
    void notifyDownload(const QString& channel);

signals:
    void channelChanged(const QString& newChannel);
    void guideDataChanged(GuideData *newGuideData);
    void loadingChanged(bool newLoading);
};


#endif // LISTINGMODEL_H
