#include "listingmodel.h"

GuideData* ListingModel::defaultGuideData = NULL;

ListingModel::ListingModel(QObject *parent) :
    QAbstractListModel(parent)

{
     QHash<int, QByteArray> roles;
     roles[ChannelRole] = "channel";
     roles[NameRole] = "name";
     roles[StartTimeRole] = "startTime";
     roles[EndTimeRole] = "endTime";
     roles[StartTimeSecsRole] = "startTimeSecs";
     roles[LengthRole] = "length";
     setRoleNames(roles);

     m_guideData = ListingModel::defaultGuideData;
     if (m_guideData) {
         connect(m_guideData, SIGNAL(channelReady(QString)),
                 this, SLOT(notifyDownload(QString)));
         emit guideDataChanged(m_guideData);
     }

     m_loading = true;
}

ListingModel::~ListingModel()
{

}

void ListingModel::setChannel(const QString &channel)
{
    if (channel != m_channel) {
        m_channel = channel;
        emit channelChanged(channel);
    }
}

QString ListingModel::channel() const
{
    return m_channel;
}

void ListingModel::fetchData()
{
    if (m_guideData) {
        m_loading = true;

        if (m_guideData->lastUpdated(m_channel).date() == QDate::currentDate())
            m_guideData->loadChannel(m_channel);
        else
            m_guideData->fetchData(QStringList(m_channel));
    }
}

int ListingModel::rowCount(const QModelIndex & /* parent */) const
{
    if (m_guideData)
        return m_guideData->getListing(m_channel).size();
    return 0;
}

void ListingModel::setDefaultGuideData(GuideData *guideData)
{
    ListingModel::defaultGuideData = guideData;
}

void ListingModel::notifyDownload(const QString &channel)
{
    if (channel == m_channel) {
        m_loading = false;
        emit loadingChanged(m_loading);
        reset();
    }
}

QVariant ListingModel::data(const QModelIndex &index, int role) const
{
    if (!m_guideData)
        return QVariant();

    if (!index.isValid())
        return QVariant();

    if (index.row() >= m_guideData->getListing(m_channel).size())
        return QVariant();

    if (role == NameRole)
        return m_guideData->getListing(m_channel).at(index.row()).name();
    else if (role == ChannelRole)
        return m_channel;
    else if (role == StartTimeSecsRole) {
        QTime start = m_guideData->getListing(m_channel).at(index.row()).startTime();
        return (double)QTime(0, 0).secsTo(start) / 3600.0;
    }
    else if (role == StartTimeRole)
        return m_guideData->getListing(m_channel).at(index.row()).startTime();
    else if (role == EndTimeRole)
        return m_guideData->getListing(m_channel).at(index.row()).endTime();
    else if (role == LengthRole)
        return m_guideData->getListing(m_channel).at(index.row()).duration();
    else
        return QVariant();
}

void ListingModel::setGuideData(QObject* guideData)
{
    if ((GuideData*)guideData != m_guideData) {
        m_guideData = (GuideData*)guideData;
        connect(m_guideData, SIGNAL(channelReady(QString)),
                this, SLOT(notifyDownload(QString)));
        emit guideDataChanged(m_guideData);
    }
}

GuideData* ListingModel::guideData() const
{
    return m_guideData;
}

bool ListingModel::loading()
{
    return m_loading;
}
