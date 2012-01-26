#include "guidedata.h"

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

}

QList<TVShow> GuideData::getListing(const QString &channel) const
{
    return m_listings.value(channel);
}

const QStringList& GuideData::availableChannels() const
{
    return m_availableChannels;
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
        query.bindValue(":startTime", show.startTime().toString("hh:mm"));
        query.bindValue(":endTime", show.endTime().toString("hh:mm"));
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
        show.setStartTime(QTime::fromString(query.value(3).toString(), "hh:mm"));
        show.setEndTime(QTime::fromString(query.value(4).toString(), "hh:mm"));
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
        qDebug() << "Requested channel" << channel << "doesn't exist in database!";
        return;
    }

    query.prepare("SELECT * FROM programs WHERE channel=:channel");
    query.bindValue(":channel", channel);
    query.exec();

    while (query.next()) {
        TVShow show;
        show.setName(query.value(1).toString());
        show.setStartTime(QTime::fromString(query.value(3).toString(), "hh:mm"));
        show.setEndTime(QTime::fromString(query.value(4).toString(), "hh:mm"));
        show.setDescription(query.value(5).toString());
        show.setDescriptionUrl(query.value(6).toString());

        QString channel = query.value(2).toString();

        m_listings[channel].append(show);
    }

    emit channelReady(channel);
}
