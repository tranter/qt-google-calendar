#ifndef CALENDAR_DATA_MANAGER_H
#define CALENDAR_DATA_MANAGER_H

#include <QObject>
#include <QVariantList>
#include <QStringList>

#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QDate>
#include <QTime>

class CalendarDataManager : public QObject
{
    Q_OBJECT
public:
    explicit CalendarDataManager(QObject *parent = 0);

    //Methods to get information from Google Calendar service.
    void getCalendars(const QString& access_token);
    void getEventsForCalendar(const QString& access_token, const QString & calID);
    void newCalendar(const QString& access_token, const QString & name);
    void deleteCalendar(const QString& access_token, const QString & calID);
    void createEvent(const QString& access_token, const QString & calID, const QString & summary,
                     const QDate& startDate, const QDate& endDate,
                     const QTime& startTime, const QTime& endTime);
    void deleteEvent(const QString& access_token, const QString & calID, const QString & eventID);
    void updateEvent(const QString& access_token, const QString & calID,
                     const QString & eventID, const QVariant & eventData);

    void moveEvent(const QString& access_token, const QString & calID,
                   const QString & eventID, const QString & destinationCalID);

    void updateCalendar(const QString& access_token, const QString & calID,
                        const QString& summary, const QString& location,
                        const QString& description);

    //Methods to get retrieved Calendars and Events.
    QVariantList getCalendars();
    QVariantList getEvents();

signals:
    void errorOccured(const QString&);
    void calendarListReady();
    void eventsReady();
    void calendarListChanged();
    void eventChanged(const QString& id);

private slots:
    void replyFinished(QNetworkReply*);

private:
    void getTimeShift(int& hours_shift, int& minutes_shift);


private:
    QNetworkAccessManager* m_pNetworkAccessManager;

    QVariantList m_calendars;
    QVariantList m_events;
};

#endif // CALENDAR_DATA_MANAGER_H
