#include "descriptionfetcher.h"

#include <QtNetwork/QtNetwork>
#include "guidedata.h"

DescriptionFetcher::DescriptionFetcher(QObject *parent) :
    QObject(parent)
{
    m_nam = new QNetworkAccessManager;
    connect(m_nam, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(handleReply(QNetworkReply*)));
}

DescriptionFetcher::~DescriptionFetcher()
{
    delete m_nam;
}

void DescriptionFetcher::fetchDescription(QString channel, int program)
{
    QNetworkRequest request;
    request.setUrl(QUrl(m_guideData->getListing(channel).at(program).descriptionUrl()));
    m_description = "Loading description";
    emit descriptionChanged(m_description);
    m_nam->get(request);
}

void DescriptionFetcher::handleReply(QNetworkReply *reply)
{
    QString text = reply->readAll();

    QString matchText = "<p class=\"programDescription\">";
    int a = text.indexOf(matchText) + matchText.length();
    int b = text.indexOf("</p>", a);

    QString description = QString::fromUtf8(text.mid(a, b - a).toStdString().c_str());

    m_description = description;

    reply->deleteLater();

    emit descriptionChanged(description);
}
