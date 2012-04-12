#ifndef FORM_H
#define FORM_H

#include <QWidget>
#include "oauth2.h"
#include "calendar_data_manager.h"

namespace Ui {
    class Form;
}

class Form : public QWidget
{
    Q_OBJECT

public:
    explicit Form(QWidget *parent = 0);
    ~Form();


    void startLogin(bool bForce); //Show login dialog even if there is gotten already access_token.

private slots:
    void loginDone();
    void errorOccured(const QString&);
    void getEventsForCalendar(int currentRow);
    void showEventDetails(int currentRow);

    void eventsReady();
    void calendarListReady();

    void newCalendar();
    void deleteCalendar();
    void editCalendar();

    void newEvent();
    void deleteEvent();
    void updateEvent();
    void moveEvent();

    void eventChanged(const QString& id);

private:
    void setControlsAccordingToAccessRole(const QString& accessRole);


private:
    Ui::Form *ui;
    OAuth2* m_pOauth2;

    QVariantList m_calendars;
    QVariantList m_events;

    CalendarDataManager m_calendarDataManager;

    QString m_strSelectedEventID;
    QString m_strSelectedCalID;
};

#endif // FORM_H
