#include "telkkudata.h"

TelkkuData::TelkkuData()
{
    m_availableChannels << "TV1"<< "TV2"<< "MTV3"<< "Nelonen"<< "Sub"<<
    "FST5"<< "JIM"<< "Nelonen Pro 2"<< "TV5"<< "YLE Teema"<<
    "Liv"<< "SuomiTV"<< "MTV3 MAX"<< "MTV3 Leffa"<< "MTV3 Fakta"<<
    "MTV3 Juniori"<< "AVA"<< "MTV3 Sarja"<< "MTV3 Scifi"<< "Eurosport"<<
    "Eurosport 2"<< "Extreme Sports"<< "ESPN America"<< "Nelonen Pro 1"<< "URHOtv"<<
    "ESPN Classic"<< "CANAL+ Emotion"<< "CANAL+ First"<< "CANAL+ Hits"<< "CANAL+ Action"<<
    "CANAL+ Series"<< "CANAL+ SF-kanalen"<< "CANAL+ URHEILU"<< "CANAL+ SPORT2"<<
    "CANAL+ SPORT HD"<< "CANAL+ FILM HD"<< "TV1000"<< "TV1000 Action"<< "TV1000 Nordic"<<
    "TV1000 Family"<< "TV1000 Classic"<< "TV1000 Drama"<< "Viasat Sport 1"<< "Viasat Golf"<<
    "Viasat Hockey"<< "Viasat Explorer"<< "Viasat Nature/Crime"<< "Viasat History"<<
    "Discovery Channel"<< "Animal Planet"<< "National Geographic"<< "Discovery World"<< "Discovery Science"<<
    "TLC"<< "Cartoon Network"<< "Disney Channel"<< "Disney XD"<< "Disney Junior"<<
    "Nickelodeon"<< "MTV Finland"<< "MTV ROCKS"<< "MTV Dance"<< "VH1"<<
    "VH1 Classic"<< "Al Jazeera"<< "BBC World"<< "Bloomberg Television"<< "CNBC"<<
    "CNN"<< "Sky News"<< "EuroNews"<< "BBC Lifestyle"<< "BBC Entertainment"<<
    "BBC Knowledge"<< "Star!"<< "Travel Channel"<< "Silver"<< "Deutsche Welle"<<
    "TCM"<< "TV Finland"<< "TV7"<< "Nelonen Kino"<< "The History Channel"<<
    "Showtime"<< "Viron ETV"<< "TV5 Monde"<< "Fashion TV"<< "Playboy TV"<<
    "SVT 1"<< "SVT 2"<< "TV3"<< "TV4"<< "Kanal5"<<
    "SVT24"<< "SVT World"<< "TV6"<< "Barnkanalen"<< "RTL"<<
    "Investigation Discovery"<< "Animal Planet HD"<< "SVT24"<< "The History Channel HD"<< "MTV HD LIVE"<<
    "Silver HD"<< "BBC HD"<< "Discovery HD Showcase "<< "MTV3 Komedia"<<
    "CANAL+ Aitio"<< "Nelonen Perhe"<< "Nelonen Maailma"<< "The Voice"<<
    "TNT7"<< "CANAL+ Family"<< "Motors TV ";

    for (int i = 0; i < m_availableChannels.size(); ++i) {
        m_channelNumbers[m_availableChannels.at(i)] = i + 1;
    }

    m_nam = new QNetworkAccessManager(this);

    connect(&m_page, SIGNAL(loadFinished(bool)),
            this, SLOT(parseData()));

    m_busy = false;
}

void TelkkuData::fetchData(const QStringList &channels, const QDate &date)
{
    for (int i = 0; i < channels.size(); ++i) {
        QString channel = channels.at(i);
        m_channelsToRequest.append(channel);
    }

    m_requestedDate = date;

    if (!m_busy) {
        m_page.mainFrame()->load(QUrl(buildUrl(m_channelsToRequest.first(), m_requestedDate)));
        m_busy = true;
    }
}

QString TelkkuData::buildUrl(const QString &channel, QDate date) const
{
    QString url = QString("http://m.telkku.com/channel/show/") +
            QString::number(m_channelNumbers.value(channel)) + "/" +
            date.toString("yyyyMMdd");
    return url;
}

void TelkkuData::parseData()
{
    QString requestedChannel = m_channelsToRequest.first();

    QWebElement document = m_page.mainFrame()->documentElement();
    QWebElementCollection elements = document.findAll("table.programTable tr");

    m_listings[requestedChannel] = QList<TVShow>();

    for (int i = 0; i < elements.count(); ++i) {
        QWebElement timeElement = elements.at(i).findFirst("td.progTime");
        QWebElement nameElement = elements.at(i).findFirst("td.progName");

        TVShow show;
        show.setStartTime(QTime::fromString(timeElement.toPlainText(), "hh.mm"));
        show.setName(nameElement.findFirst("a").toPlainText());

        QString url = nameElement.findFirst("a").attribute("href");
        show.setDescriptionUrl(url);

        m_listings[requestedChannel].append(show);
    }

    for (int i = 0; i < m_listings[requestedChannel].size(); ++i) {
        QTime end;
        if (i < m_listings[requestedChannel].size() - 1)
            end = m_listings[requestedChannel].at(i + 1).startTime();
        else
            end = m_listings[requestedChannel].at(i).startTime().addSecs(3600);

        m_listings[requestedChannel][i].setEndTime(end);
    }

    emit channelReady(requestedChannel);

    saveChannel(requestedChannel);

    m_channelsToRequest.removeFirst();

    if (!m_channelsToRequest.isEmpty()) {
        m_page.mainFrame()->load(QUrl(buildUrl(m_channelsToRequest.first(), m_requestedDate)));
    } else {
        m_busy = false;
    }
}

QString TelkkuData::descriptionUrl(QString channel, int program) const
{
    return m_listings.value(channel).at(program).descriptionUrl();
}
