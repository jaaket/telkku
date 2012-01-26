#ifndef TVSHOW_H
#define TVSHOW_H

class QString;
class QTime;

class TVShow
{
public:
    TVShow();
    TVShow(const QString& name, const QTime& time, double duration);

    void setName(QString);
    void setDescription(QString);
    void setStartTime(QTime);
    void setEndTime(QTime);
    void setDescriptionUrl(QString);

    int id() const;
    QString name() const;
    QString description() const;
    QTime startTime() const;
    QTime endTime() const;
    double duration() const;
    QString descriptionUrl() const;

private:
    int m_id;
    QString m_name;
    QString m_description;
    QTime m_startTime;
    QTime m_endTime;
    QString m_descriptionUrl;

    static int s_idCounter;
};

#endif // TVSHOW_H
