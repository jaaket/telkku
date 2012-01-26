#include "descriptionfetcher.h"

#include <QtNetwork/QtNetwork>
#include <QtWebKit>
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
    request.setUrl(QUrl(m_guideData->descriptionUrl(channel, program)));
    m_description = "Loading description";
    emit descriptionChanged(m_description);
    m_nam->get(request);
}

void DescriptionFetcher::handleReply(QNetworkReply *reply)
{
    QWebPage page;
    page.mainFrame()->setHtml(reply->readAll());
    QWebElement document = page.mainFrame()->documentElement();
    QWebElement element = document.findFirst("p.programDescription");

    QString description = QString::fromUtf8(element.toPlainText().toStdString().c_str());

    m_description = description;

    reply->deleteLater();

    emit descriptionChanged(description);
}
