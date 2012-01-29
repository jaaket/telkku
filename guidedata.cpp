#include "guidedata.h"
#include <QtWebKit>

GuideData::GuideData()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");

    QString path = QDesktopServices::storageLocation(QDesktopServices::DataLocation);

    path.append(QDir::separator()).append(".telkku");
    path = QDir::toNativeSeparators(path);

    if (!QFile::exists(path)) {
        QDir dir;
        dir.mkpath(path);
    }

    path.append(QDir::separator()).append("my.db.sqlite");
    path = QDir::toNativeSeparators(path);

    db.setDatabaseName(path);
    db.open();

    QSqlQuery query;

    if (!db.tables().contains("appdata")) {
        query.exec("CREATE TABLE appdata "
                   "(id INTEGER PRIMARY KEY, "
                   "key TEXT, "
                   "value TEXT)");
    }

    if (!db.tables().contains("channels")) {
        query.exec("CREATE TABLE channels "
                   "(id INTEGER PRIMARY KEY, "
                   "name TEXT, "
                   "lastUpdated TEXT)");
    }

    if (!db.tables().contains("programs")) {
        query.exec("CREATE TABLE programs "
                   "(id INTEGER PRIMARY KEY, "
                   "name TEXT, "
                   "channel TEXT, "
                   "startTime TEXT, "
                   "endTime TEXT, "
                   "description TEXT, "
                   "descriptionUrl TEXT)");
    }

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
    m_page = new QWebPage(this);

    connect(m_page, SIGNAL(loadFinished(bool)),
            this, SLOT(parseData()));

    m_busy = false;
}

GuideData::~GuideData()
{
    delete m_nam;
    delete m_page;
}

const QStringList& GuideData::availableChannels() const
{
    return m_availableChannels;
}

QList<TVShow> GuideData::getListing(const QString &channel) const
{
    return m_listings.value(channel);
}

QDateTime GuideData::lastUpdated(QString channel) const
{
    QSqlQuery query;

    query.prepare("SELECT lastUpdated FROM channels WHERE name=:name");
    query.bindValue(":name", channel);
    query.exec();

    QDateTime lastUpdated;

    while (query.next()) {
        lastUpdated = QDateTime::fromString(query.value(0).toString(),
                                            "yyyy-MM-dd hh:mm:ss.zzz");
    }

    return lastUpdated;
}

void GuideData::saveChannel(QString channel)
{
    QSqlDatabase db = QSqlDatabase::database();

    QString now = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz");

    db.transaction();

    QSqlQuery query;
    bool ok;

    query.prepare("SELECT * FROM channels WHERE name=:name");
    query.bindValue(":name", channel);
    ok = query.exec();
    if (!ok) qDebug() << query.lastError();

    if (query.next()) {
        query.prepare("UPDATE channels SET lastUpdated=:lastUpdated WHERE name=:name");
    } else {
        query.prepare("INSERT INTO channels (name, lastUpdated) VALUES (:name, :lastUpdated)");
    }

    query.bindValue(":name", channel);
    query.bindValue(":lastUpdated", now);
    ok = query.exec();
    if (!ok) qDebug() << query.lastError();

    query.prepare("INSERT INTO programs (name, channel, startTime, endTime, description, descriptionUrl) "
                  "VALUES (:name, :channel, :startTime, :endTime, :description, :descriptionUrl)");

    foreach (TVShow show, m_listings.value(channel)) {
        query.bindValue(":name", show.name());
        query.bindValue(":channel", channel);
        query.bindValue(":startTime",
                        show.startTime().toString("yyyy-MM-dd hh:mm:ss.zzz"));
        query.bindValue(":endTime",
                        show.endTime().toString("yyyy-MM-dd hh:mm:ss.zzz"));
        query.bindValue(":description", show.description());
        query.bindValue(":descriptionUrl", show.descriptionUrl());
        ok = query.exec();
        if (!ok) qDebug() << query.lastError();
    }

    db.commit();
}

void GuideData::saveDatabase()
{
    foreach (QString channel, m_listings.keys()) {
        saveChannel(channel);
    }
}

void GuideData::loadDatabase()
{
    m_listings.clear();

    QSqlQuery query;
    query.exec("SELECT * FROM programs");

    while (query.next()) {
        TVShow show;
        show.setName(query.value(1).toString());
        show.setStartTime(QDateTime::fromString(query.value(3).toString(),
                                                "yyyy-MM-dd hh:mm:ss.zzz"));
        show.setEndTime(QDateTime::fromString(query.value(4).toString(),
                                              "yyyy-MM-dd hh:mm:ss.zzz"));
        show.setDescription(query.value(5).toString());
        show.setDescriptionUrl(query.value(6).toString());

        QString channel = query.value(2).toString();

        if (!m_listings.keys().contains(channel)) {
            QList<TVShow> dummy;
            m_listings[channel] = dummy;
        }

        m_listings[channel].append(show);
    }

    for (int i = 0; i < m_listings.keys().size(); ++i) {
        emit channelReady(m_listings.keys().at(i));
    }
}

void GuideData::loadChannel(QString channel)
{
    m_listings[channel] = QList<TVShow>();

    QSqlQuery query;

    query.prepare("SELECT * FROM channels WHERE name=:name");
    query.bindValue(":name", channel);
    query.exec();

    if (!query.next()) {
        qDebug() << "Requested channel" << channel <<
                    "doesn't exist in database!";
        return;
    }

    query.prepare("SELECT * FROM programs WHERE channel=:channel");
    query.bindValue(":channel", channel);
    query.exec();

    while (query.next()) {
        TVShow show;
        show.setName(query.value(1).toString());
        show.setStartTime(QDateTime::fromString(query.value(3).toString(),
                                                "yyyy-MM-dd hh:mm:ss.zzz"));
        show.setEndTime(QDateTime::fromString(query.value(4).toString(),
                                              "yyyy-MM-dd hh:mm:ss.zzz"));
        show.setDescription(query.value(5).toString());
        show.setDescriptionUrl(query.value(6).toString());

        QString channel = query.value(2).toString();

        m_listings[channel].append(show);
    }

    emit channelReady(channel);
}

void GuideData::fetchData(const QStringList &channels, const QDate &date)
{
    for (int i = 0; i < channels.size(); ++i) {
        QString channel = channels.at(i);
        m_channelsToRequest.append(channel);
    }

    m_requestedDate = date;

    if (!m_busy) {
        m_page->mainFrame()->load(QUrl(buildUrl(m_channelsToRequest.first(),
                                                m_requestedDate)));
        m_busy = true;
    }
}

QString GuideData::buildUrl(const QString &channel, QDate date) const
{
    QString url = QString("http://m.telkku.com/channel/show/") +
            QString::number(m_channelNumbers.value(channel)) + "/" +
            date.toString("yyyyMMdd");
    return url;
}

void GuideData::parseData()
{
    QString requestedChannel = m_channelsToRequest.first();

    QWebElement document = m_page->mainFrame()->documentElement();
    QWebElementCollection elements = document.findAll("table.programTable tr");

    m_listings[requestedChannel] = QList<TVShow>();

    for (int i = 0; i < elements.count(); ++i) {
        QWebElement timeElement = elements.at(i).findFirst("td.progTime");
        QWebElement nameElement = elements.at(i).findFirst("td.progName");

        TVShow show;

        QDateTime startTime = QDateTime::currentDateTime();
        startTime.setTime(QTime::fromString(timeElement.toPlainText(),
                                            "hh.mm"));

        show.setStartTime(startTime);
        show.setName(nameElement.findFirst("a").toPlainText());

        QString url = nameElement.findFirst("a").attribute("href");
        show.setDescriptionUrl(url);

        m_listings[requestedChannel].append(show);
    }

    bool nextDay = false;

    for (int i = 0; i < m_listings[requestedChannel].size(); ++i) {
        if (i < m_listings[requestedChannel].size() - 1) {
            QDateTime endTime = m_listings[requestedChannel].at(i + 1).startTime();
            m_listings[requestedChannel][i].setEndTime(endTime);
        } else {
            QDateTime endTime = m_listings[requestedChannel].at(i).startTime();
            m_listings[requestedChannel][i].setEndTime(endTime.addSecs(3600));
        }

        // Handle programs starting or ending after midnight
        if (i < 2) continue;

        if (nextDay || m_listings[requestedChannel].at(i).endTime().time() <
                m_listings[requestedChannel].at(i - 1).startTime().time()) {
            m_listings[requestedChannel][i].setEndTime(m_listings[requestedChannel].at(i).endTime().addDays(1));

            if (nextDay || m_listings[requestedChannel].at(i).startTime().time() <
                    m_listings[requestedChannel].at(i - 1).startTime().time()) {
                m_listings[requestedChannel][i].setStartTime(m_listings[requestedChannel].at(i).startTime().addDays(1));
            }

            nextDay = true;
        }
    }

    // Handle the case where the first program starts before midnight
    if (m_listings[requestedChannel].first().startTime().time() >
            m_listings[requestedChannel].first().endTime().time()) {
        m_listings[requestedChannel].first().setStartTime(m_listings[requestedChannel].first().startTime().addDays(-1));
    }

    emit channelReady(requestedChannel);

    saveChannel(requestedChannel);

    m_channelsToRequest.removeFirst();

    if (!m_channelsToRequest.isEmpty()) {
        m_page->mainFrame()->load(QUrl(buildUrl(m_channelsToRequest.first(),
                                                m_requestedDate)));
    } else {
        m_busy = false;
    }
}
