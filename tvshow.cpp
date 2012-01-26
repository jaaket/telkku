#include <QtGui>
#include "tvshow.h"

int TVShow::s_idCounter = 0;

TVShow::TVShow()
{
    m_id = s_idCounter++;
}

void TVShow::setName(QString name)
{
    m_name = name;
}

void TVShow::setDescription(QString description)
{
    m_description = description;
}

void TVShow::setStartTime(QTime startTime)
{
    m_startTime = startTime;
}

void TVShow::setEndTime(QTime endTime)
{
    m_endTime = endTime;
}

void TVShow::setDescriptionUrl(QString descriptionUrl)
{
    m_descriptionUrl = descriptionUrl;
}

int TVShow::id() const
{
    return m_id;
}

QString TVShow::name() const
{
    return m_name;
}

QString TVShow::description() const
{
    return m_description;
}

QTime TVShow::startTime() const
{
    return m_startTime;
}

QTime TVShow::endTime() const
{
    return m_endTime;
}

double TVShow::duration() const
{
    return (double)m_startTime.secsTo(m_endTime) / 3600.0;
}

QString TVShow::descriptionUrl() const
{
    return m_descriptionUrl;
}

