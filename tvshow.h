#ifndef TVSHOW_H
#define TVSHOW_H

class QString;
class QDateTime;

class TVShow
{
public:
    TVShow();

    void setName(QString);
    void setDescription(QString);
    void setStartTime(QDateTime);
    void setEndTime(QDateTime);
    void setDescriptionUrl(QString);

    int id() const;
    QString name() const;
    QString description() const;
    QDateTime startTime() const;
    QDateTime endTime() const;
    double duration() const;
    QString descriptionUrl() const;

private:
    int m_id;
    QString m_name;
    QString m_description;
    QDateTime m_startTime;
    QDateTime m_endTime;
    QString m_descriptionUrl;

    static int s_idCounter;
};

#endif // TVSHOW_H
