#include "calendar_data_manager.h"
#include <QJson/Parser>
#include <QJson/Serializer>

#include <QDebug>
#include <QDateTime>
#include <QApplication>

CalendarDataManager::CalendarDataManager(QObject* parent) : QObject(parent)
{
    m_pNetworkAccessManager = new QNetworkAccessManager(this);
    connect(m_pNetworkAccessManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinished(QNetworkReply*)));

}

void CalendarDataManager::replyFinished(QNetworkReply * reply)
{
    QApplication::restoreOverrideCursor();
    qDebug() << __FUNCTION__;
    QString json = reply->readAll();
    qDebug() << "Reply = " << json;
    qDebug() << "URL = " << reply->url();
    QString strUrl = reply->url().toString();

    if(json.isEmpty())
    {
        //Empty reply.
        qDebug() << "Empty answer";
        //Deletion of calendars or events
        QRegExp reg("calendars/*/events", Qt::CaseSensitive, QRegExp::Wildcard);
        if(strUrl.indexOf(reg) != -1)
        {
            emit eventChanged("");
        }
        else
        {
            emit calendarListChanged();
        }
        return;

    }

    QJson::Parser parser;
    bool ok;
    // json is a QString containing the data to convert
    QVariant result = parser.parse (json.toLatin1(), &ok);

    if(!ok)
    {
        emit errorOccured(QString("Cannot convert to QJson object: %1").arg(json));
        return;
    }

    if(result.toMap().contains("error"))
    {
        qDebug() << "ERROR occured:\n" << strUrl;
        emit errorOccured(result.toMap()["error"].toMap()["message"].toString());
        return;
    }
    if(result.toMap()["kind"].toString() == "calendar#calendarList")
    {
        m_calendars = result.toMap()["items"].toList();
        emit calendarListReady();
    }
    else if(result.toMap()["kind"].toString() == "calendar#calendar")
    {
        emit calendarListChanged();
    }
    else if(result.toMap()["kind"].toString() == "calendar#events")
    {
        m_events = result.toMap()["items"].toList();
        emit eventsReady();
    }
    else if(result.toMap()["kind"].toString() == "calendar#event")
    {
        emit eventChanged(result.toMap()["id"].toString());
    }

}

void CalendarDataManager::getCalendars(const QString& access_token)
{
    qDebug() << Q_FUNC_INFO;
    QString s = QString("https://www.googleapis.com/calendar/v3/users/me/calendarList?access_token=%1").arg(access_token);
    QApplication::setOverrideCursor(Qt::WaitCursor);
    m_pNetworkAccessManager->get(QNetworkRequest(QUrl(s)));
}

void CalendarDataManager::getEventsForCalendar(const QString& access_token, const QString & calID)
{
    qDebug() << Q_FUNC_INFO;
    QString s = QString("https://www.googleapis.com/calendar/v3/calendars/%1/events").arg(calID); //?access_token=%2").arg(calID).arg(access_token);

    QUrl url;

#if QT_VERSION >= 0x050000
    // Qt5 code
    url = QUrl::fromEncoded(QUrl::toPercentEncoding(s, "/:"));
#else
    // Qt4 code
    url.setEncodedUrl(QUrl::toPercentEncoding(s, "/:"));
#endif


    QNetworkRequest request;
    request.setUrl(url);
    request.setRawHeader("Authorization", QString("OAuth %1").arg(access_token).toLatin1());
    request.setRawHeader("X-JavaScript-User-Agent", "Google APIs Explorer");

    QApplication::setOverrideCursor(Qt::WaitCursor);
    m_pNetworkAccessManager->get(request);
}

QVariantList CalendarDataManager::getCalendars()
{
    return m_calendars;
}

QVariantList CalendarDataManager::getEvents()
{
    return m_events;
}

void CalendarDataManager::newCalendar(const QString& access_token, const QString & name)
{
    QString s = QString("https://www.googleapis.com/calendar/v3/calendars?access_token=%1").arg(access_token);


    QByteArray params;
    params.append(QString("{ \"summary\": \"%1\" }").arg(name).toUtf8());
    QNetworkRequest request;
    request.setUrl(QUrl(s));
    request.setRawHeader("Content-Type", "application/json");

    QApplication::setOverrideCursor(Qt::WaitCursor);
    m_pNetworkAccessManager->post(request, params);
}

void CalendarDataManager::deleteCalendar(const QString& access_token, const QString & calID)
{
    QString s = QString("https://www.googleapis.com/calendar/v3/calendars/%1").arg(calID);

    QUrl url;
#if QT_VERSION >= 0x050000
    // Qt5 code
    url = QUrl::fromEncoded(QUrl::toPercentEncoding(s, "/:"));
#else
    // Qt4 code
    url.setEncodedUrl(QUrl::toPercentEncoding(s, "/:"));
#endif

    QNetworkRequest request;
    request.setUrl(url);
    request.setRawHeader("Authorization", QString("OAuth %1").arg(access_token).toLatin1());

    QApplication::setOverrideCursor(Qt::WaitCursor);
    m_pNetworkAccessManager->deleteResource(request);
}

void CalendarDataManager::createEvent(const QString& access_token, const QString & calID, const QString & summary,
                                      const QDate& startDate, const QDate& endDate,
                                      const QTime& startTime, const QTime& endTime)
{
    QString s = QString("https://www.googleapis.com/calendar/v3/calendars/%1/events").arg(calID);
    QUrl url;
#if QT_VERSION >= 0x050000
    // Qt5 code
    url = QUrl::fromEncoded(QUrl::toPercentEncoding(s, "/:"));
#else
    // Qt4 code
    url.setEncodedUrl(QUrl::toPercentEncoding(s, "/:"));
#endif

    QNetworkRequest request;
    request.setUrl(url);
    request.setRawHeader("Authorization", QString("OAuth %1").arg(access_token).toLatin1());
    request.setRawHeader("Content-Type", "application/json");


    QString str;
    if(startTime.isNull())
    {
        str = QString("{\n") +
                QString("\"summary\": \"%1\",\n").arg(summary) +
                QString("\"start\": { \"date\": \"%1\" },\n").arg(startDate.toString()) +
                QString("\"end\":   { \"date\": \"%1\" }\n").arg(endDate.toString()) +
                QString("}");
    }
    else
    {
        QDateTime startDT(startDate, startTime), endDT(endDate, endTime);
        QString strStart = startDT.toUTC().toString(Qt::ISODate);
        QString strEnd   = endDT.toUTC().toString(Qt::ISODate);
        if(!strStart.endsWith("Z"))
            strStart += "Z";
        if(!strEnd.endsWith("Z"))
            strEnd += "Z";
        str = QString("{\n") +
                QString("\"summary\": \"%1\",\n").arg(summary) +
                QString("\"start\": { \"dateTime\": \"%1\" },\n").arg(strStart) +
                QString("\"end\": { \"dateTime\": \"%1\" }\n").arg(strEnd) +
                QString("}");
    }

    QByteArray params = str.toUtf8();
    qDebug() << "Params to send" << params;


    QApplication::setOverrideCursor(Qt::WaitCursor);
    m_pNetworkAccessManager->post(request, params);

}

void CalendarDataManager::moveEvent(const QString& access_token, const QString & calID,
                                    const QString & eventID, const QString & destinationCalID)
{
    QString s = QString("https://www.googleapis.com/calendar/v3/calendars/%1/events/%2/move?destination=%3").arg(calID).arg(eventID).arg(destinationCalID);
    QUrl url;
#if QT_VERSION >= 0x050000
    // Qt5 code
    url = QUrl::fromEncoded(QUrl::toPercentEncoding(s, "/:"));
#else
    // Qt4 code
    url.setEncodedUrl(QUrl::toPercentEncoding(s, "/:"));
#endif

    QNetworkRequest request;
    request.setUrl(url);
    request.setRawHeader("Authorization", QString("OAuth %1").arg(access_token).toLatin1());
    request.setRawHeader("Content-Type", "application/json");

    QByteArray params;

    QApplication::setOverrideCursor(Qt::WaitCursor);
    m_pNetworkAccessManager->post(request, params);

}


void CalendarDataManager::deleteEvent(const QString& access_token, const QString & calID, const QString & eventID)
{
    QString s = QString("https://www.googleapis.com/calendar/v3/calendars/%1/events/%2").arg(calID).arg(eventID);

    QUrl url;
    url = QUrl::fromEncoded(QUrl::toPercentEncoding(s, "/:"));

    QNetworkRequest request;
    request.setUrl(url);
    request.setRawHeader("Authorization", QString("OAuth %1").arg(access_token).toLatin1());

    QApplication::setOverrideCursor(Qt::WaitCursor);
    m_pNetworkAccessManager->deleteResource(request);
}

void CalendarDataManager::getTimeShift(int& hours_shift, int& minutes_shift)
{
    //Adding UTC time shift!!!
    QDateTime curr = QDateTime::currentDateTime();
    QDateTime utc   = curr.toUTC();
    QDateTime local = curr.toLocalTime();

    QDateTime u1(utc.date(), utc.time(), Qt::LocalTime);
    QDateTime l1(local.date(), local.time(), Qt::LocalTime);

    //int fromLocalToUtc = l1.secsTo(u1);
    int fromUtcToLocal = u1.secsTo(l1);

    //qDebug() << "u1" << u1;
    //qDebug() << "l1" << l1;

    //qDebug() << "fromLocalToUtc" << fromLocalToUtc;
    //qDebug() << "fromUtcToLocal" << fromUtcToLocal;

    hours_shift = fromUtcToLocal/3600;
    minutes_shift = (fromUtcToLocal%3600)/60;
}

void CalendarDataManager::updateEvent(const QString& access_token, const QString & calID,
                 const QString & eventID, const QVariant & eventData)
{
  QString s = QString("https://www.googleapis.com/calendar/v3/calendars/%1/events/%2").arg(calID).arg(eventID);
  QUrl url;
#if QT_VERSION >= 0x050000
    // Qt5 code
    url = QUrl::fromEncoded(QUrl::toPercentEncoding(s, "/:"));
#else
    // Qt4 code
    url.setEncodedUrl(QUrl::toPercentEncoding(s, "/:"));
#endif

  QNetworkRequest request;
  request.setUrl(url);
  request.setRawHeader("Authorization", QString("OAuth %1").arg(access_token).toLatin1());
  request.setRawHeader("Content-Type", "application/json");


  QJson::Serializer serializer;
  QByteArray params = serializer.serialize(eventData);
  qDebug() << "Params to send" << params;

  QApplication::setOverrideCursor(Qt::WaitCursor);
  m_pNetworkAccessManager->put(request, params);
}

void CalendarDataManager::updateCalendar(const QString& access_token, const QString & calID,
                                         const QString& summary, const QString& location,
                                         const QString& description)
{
    QString s = QString("https://www.googleapis.com/calendar/v3/calendars/%1").arg(calID);
    QUrl url;
#if QT_VERSION >= 0x050000
    // Qt5 code
    url = QUrl::fromEncoded(QUrl::toPercentEncoding(s, "/:"));
#else
    // Qt4 code
    url.setEncodedUrl(QUrl::toPercentEncoding(s, "/:"));
#endif

    QNetworkRequest request;
    request.setUrl(url);
    request.setRawHeader("Authorization", QString("OAuth %1").arg(access_token).toLatin1());
    request.setRawHeader("Content-Type", "application/json");

    QString str;
    str = QString("{\n") +
            QString("\"summary\": \"%1\",\n").arg(summary) +
            QString("\"location\": \"%1\",\n").arg(location) +
            QString("\"description\": \"%1\"\n").arg(description) +
            QString("}");

    QByteArray params = str.toUtf8();
    qDebug() << "Params to send" << params;


    QApplication::setOverrideCursor(Qt::WaitCursor);
    m_pNetworkAccessManager->put(request, params);

}

