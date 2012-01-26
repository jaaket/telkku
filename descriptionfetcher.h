#ifndef DESCRIPTIONFETCHER_H
#define DESCRIPTIONFETCHER_H

#include <QObject>

class QNetworkAccessManager;
class QNetworkReply;
class GuideData;

class DescriptionFetcher : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString description READ description NOTIFY descriptionChanged)

public:
    explicit DescriptionFetcher(QObject *parent = 0);
    ~DescriptionFetcher();

    void setGuideData(GuideData *guideData) { m_guideData = guideData; }

    Q_INVOKABLE void fetchDescription(QString channel, int program);

    QString description() const { return m_description; }

private:
    QNetworkAccessManager *m_nam;
    QString m_description;
    GuideData *m_guideData;

signals:
    void descriptionChanged(QString);

public slots:
    void handleReply(QNetworkReply*);

};

#endif // DESCRIPTIONFETCHER_H
