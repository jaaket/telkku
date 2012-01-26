#include "yledata.h"

YLEData::YLEData() :
    GuideData()
{
    m_nam = new QNetworkAccessManager();
    connect(m_nam, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(networkReply(QNetworkReply*)));

    m_availableChannels.append("tv1");
    m_availableChannels.append("tv2");
    m_availableChannels.append("mtv");
    m_availableChannels.append("nel");
    m_availableChannels.append("sub");
    m_availableChannels.append("fsd");
    m_availableChannels.append("yte");
    m_availableChannels.append("nep");
}

YLEData::~YLEData()
{
    delete m_nam;
}

void YLEData::fetchData(const QStringList &channels, const QDate &date)
{
    m_channelsToRequest.clear();
    for (int i = 0; i < channels.size(); ++i) {
        QString channel = channels.at(i);
        m_channelsToRequest.append(channel);
    }

    m_requestedChannel = m_channelsToRequest.last();
    m_requestedDate = date;

    m_nam->get(QNetworkRequest(QUrl(buildUrl(m_channelsToRequest.takeLast(), date))));
}

void YLEData::networkReply(QNetworkReply *reply)
{
    QString data(reply->readAll());

    QStringList list = data.split("<br><br>")[1].split("<!-- TNS")[0].
            replace("<!-- seuraava -->\n", "").split("<br>\n");

    QList<TVShow> channel;

    for (int i = 0; i < list.size(); ++i) {
        QRegExp rx("<b>(.+) </b> (.+)");

        if (rx.indexIn(list.at(i)) != -1 && rx.captureCount() == 2) {
            QString name = rx.capturedTexts()[2];
            QString time = rx.capturedTexts()[1];

            if (name.contains("href")) {
                QRegExp rx2("<a href=.+>(.+) <\\/a>");
                rx2.indexIn(list.at(i));
                name = rx2.capturedTexts()[1];
            }

            TVShow show;
            show.setName(name);
            show.setStartTime(QTime::fromString(time, "hh.mm"));
            show.setDescription("No description available!");
            channel.append(show);
        }
    }

    for (int i = 0; i < channel.size(); ++i) {
        QTime end;
        if (i < channel.size() - 1)
            end = channel.at(i + 1).startTime();
        else
            end = QTime(23, 59, 59, 999);

        channel[i].setEndTime(end);
    }

    m_listings[m_requestedChannel] = channel;

    emit channelReady(m_requestedChannel);

    if (!m_channelsToRequest.isEmpty()) {
        m_requestedChannel = m_channelsToRequest.last();
        m_nam->get(QNetworkRequest(QUrl(buildUrl(m_channelsToRequest.takeLast(), m_requestedDate))));
    }
}

QString YLEData::buildUrl(const QString &channel, QDate date) const
{
    return QString("http://yle.fi/ohjelmaopas/classic.php?co[]=") +
            channel + "&date=" + date.toString("yyyyMMdd") + "&l=f&verbose=no&mode=pda&span=day";
}
