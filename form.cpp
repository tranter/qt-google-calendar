#include "form.h"
#include "ui_form.h"
#include <QDebug>
#include <QMessageBox>
#include <QInputDialog>
#include "new_event_dialog.h"
#include "edit_calendar_dialog.h"

Form::Form(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Form)
{
    ui->setupUi(this);

    m_pOauth2 = new OAuth2(this);
    connect(m_pOauth2, SIGNAL(loginDone()), this, SLOT(loginDone()));

    connect(&m_calendarDataManager, SIGNAL(errorOccured(QString)), this, SLOT(errorOccured(QString)));
    connect(&m_calendarDataManager, SIGNAL(calendarListReady()), this, SLOT(calendarListReady()));
    connect(&m_calendarDataManager, SIGNAL(eventsReady()), this, SLOT(eventsReady()));
    connect(&m_calendarDataManager, SIGNAL(calendarListChanged()), this, SLOT(loginDone()));
    connect(&m_calendarDataManager, SIGNAL(eventChanged(QString)), this, SLOT(eventChanged(QString)));

    connect(ui->calendarsListWidget, SIGNAL(currentRowChanged(int)), this, SLOT(getEventsForCalendar(int)));
    connect(ui->eventsListWidget,    SIGNAL(currentRowChanged(int)), this, SLOT(showEventDetails(int)));

    connect(ui->newCalendarButton,    SIGNAL(clicked()), this, SLOT(newCalendar()));
    connect(ui->deleteCalendarButton, SIGNAL(clicked()), this, SLOT(deleteCalendar()));
    connect(ui->editCalendarButton,   SIGNAL(clicked()), this, SLOT(editCalendar()));

    connect(ui->newEventButton,    SIGNAL(clicked()), this, SLOT(newEvent()));
    connect(ui->deleteEventButton, SIGNAL(clicked()), this, SLOT(deleteEvent()));
    connect(ui->updateEventButton, SIGNAL(clicked()), this, SLOT(updateEvent()));
    connect(ui->moveEventButton,   SIGNAL(clicked()), this, SLOT(moveEvent()));

}

Form::~Form()
{
    delete ui;
}

void Form::startLogin(bool bForce)
{
    //Now we allow to start logging in when m_oauth2.isAuthorized(). User can log in using another Google account.
    //m_oauth2.startLogin(this, bForce); //this is a parent widget for a login dialog.
    m_pOauth2->startLogin(bForce); //this is a parent widget for a login dialog.
}


void Form::loginDone()
{
    m_strSelectedCalID = "";
    int currentCalIndex = ui->calendarsListWidget->currentRow();
    if(currentCalIndex != -1)
    {
        m_strSelectedCalID = m_calendars[currentCalIndex].toMap().value("id").toString();
    }

    ui->eventsListWidget->clear();
    ui->calendarsListWidget->clear();
    m_calendarDataManager.getCalendars(m_pOauth2->accessToken());

}

void Form::errorOccured(const QString& error)
{
    if(error == "Invalid Credentials")
    {
        m_pOauth2->startLogin(true);
    }
    else
    {
        QMessageBox::warning(this, tr("Error"), error);
    }
}

void Form::getEventsForCalendar(int currentRow)
{
    ui->eventsListWidget->clear();
    if(currentRow < 0)
        return;
    QString calID = m_calendars[currentRow].toMap().value("id").toString();

    QString accessRole = m_calendars[currentRow].toMap().value("accessRole").toString();
    setControlsAccordingToAccessRole(accessRole);

    qDebug() << "calID" << calID;
    m_calendarDataManager.getEventsForCalendar(m_pOauth2->accessToken(), calID);

}

void Form::setControlsAccordingToAccessRole(const QString& accessRole)
{
    bool canWrite = (accessRole == "owner" || accessRole == "writer");
    ui->deleteEventButton->setEnabled(canWrite);
    ui->deleteCalendarButton->setEnabled(canWrite);
    ui->moveEventButton->setEnabled(canWrite);
    ui->newEventButton->setEnabled(canWrite);
    ui->updateEventButton->setEnabled(canWrite);
    ui->editCalendarButton->setEnabled(canWrite);
}

void Form::showEventDetails(int currentRow)
{
    if(currentRow<0)
    {
        ui->frame->setEnabled(false);
        return;
    }
    ui->frame->setEnabled(true);

    QVariantMap event = m_events[currentRow].toMap();
    QString location = event.value("location").toString();
    QString summary = event.value("summary").toString();

    QDateTime startDT, endDT;
    bool bTime = false;
    QVariantMap mp = event.value("start").toMap();
    bTime = mp.contains("dateTime");
    if(mp.contains("dateTime"))
    {
        startDT = QDateTime::fromString(mp.value("dateTime").toString(), Qt::ISODate);
    }
    else if(mp.contains("date"))
    {
        startDT = QDateTime::fromString(mp.value("date").toString(), Qt::ISODate);
    }
    mp = event.value("end").toMap();
    if(mp.contains("dateTime"))
    {
        endDT = QDateTime::fromString(mp.value("dateTime").toString(), Qt::ISODate);
    }
    else if(mp.contains("date"))
    {
        endDT = QDateTime::fromString(mp.value("date").toString(), Qt::ISODate);
    }
    startDT = startDT.toLocalTime();
    endDT = endDT.toLocalTime();

    ui->summaryLineEdit->setText(summary);
    ui->startDateEdit->setDate(startDT.date());
    ui->endDateEdit->setDate(endDT.date());
    ui->startTimeEdit->setTime(startDT.time());
    ui->endTimeEdit->setTime(endDT.time());
    ui->timeGroupBox->setChecked(bTime);

    ui->locationLineEdit->setText(location);

}

void Form::eventsReady()
{
    m_events = m_calendarDataManager.getEvents();
    QStringList lst;
    int select_index = -1;
    for(int i = 0; i < m_events.count(); ++i)
    {
        lst << m_events[i].toMap()["summary"].toString();
        if(m_events[i].toMap()["id"].toString() == m_strSelectedEventID)
        {
            select_index = i;
        }
    }
    ui->eventsListWidget->addItems(lst);
    if(select_index != -1)
    {
        qDebug() << "setCurrentRow" << select_index;
        ui->eventsListWidget->setCurrentRow(select_index);
    }
}

void Form::calendarListReady()
{
    m_calendars = m_calendarDataManager.getCalendars();
    QStringList lst;
    int select_index = -1;
    for(int i = 0; i < m_calendars.count(); ++i)
    {
        lst << m_calendars[i].toMap()["summary"].toString();
        if(m_calendars[i].toMap()["id"].toString() == m_strSelectedCalID)
        {
            select_index = i;
        }
    }
    ui->calendarsListWidget->addItems(lst);
    if(select_index != -1)
    {
        ui->calendarsListWidget->setCurrentRow(select_index);
    }
}

void Form::newCalendar()
{
    qDebug() << __FUNCTION__;
    QString strName = QInputDialog::getText(this, "New Calendar", "Enter the title:");
    if(strName.isEmpty())
    {
        return;
    }
    m_calendarDataManager.newCalendar(m_pOauth2->accessToken(), strName);
}

void Form::deleteCalendar()
{
    qDebug() << __FUNCTION__;
    int currentRow = ui->calendarsListWidget->currentRow();
    if(currentRow < 0)
    {
        QMessageBox::warning(this, tr("Warning"), tr("No selected calendar."));
        return;
    }
    QString calID = m_calendars[currentRow].toMap().value("id").toString();
    ui->eventsListWidget->clear();
    m_calendarDataManager.deleteCalendar(m_pOauth2->accessToken(), calID);
}


void Form::eventChanged(const QString& id)
{
    int currentCalIndex = ui->calendarsListWidget->currentRow();

    if (currentCalIndex<0)
    {
        QMessageBox::warning(this, tr("Warning"), tr("Do not know wich calendar to reload."));
        return;
    }

    m_strSelectedEventID = id;
    getEventsForCalendar(currentCalIndex);
}



void Form::newEvent()
{
    qDebug() << Q_FUNC_INFO;
    int currentCalIndex = ui->calendarsListWidget->currentRow();

    if (currentCalIndex<0)
    {
        QMessageBox::warning(this, tr("Warning"), tr("No selected calendar."));
        return;
    }

    NewEventDialog dlg(this);
    int res = dlg.exec();
    if(res != QDialog::Accepted)
    {
        return;
    }
    QString summary;
    QDate startDate, endDate;
    QTime startTime, endTime;

    dlg.getNewEventParams(summary, startDate, endDate, startTime, endTime);

    QString calID = m_calendars[currentCalIndex].toMap().value("id").toString();

    m_calendarDataManager.createEvent(m_pOauth2->accessToken(), calID, summary, startDate, endDate, startTime, endTime);
}

void Form::deleteEvent()
{
    qDebug() << Q_FUNC_INFO;
    int currentCalIndex = ui->calendarsListWidget->currentRow();

    if (currentCalIndex<0)
    {
        QMessageBox::warning(this, tr("Warning"), tr("No selected calendar."));
        return;
    }

    int currentRow = ui->eventsListWidget->currentRow();
    if(currentRow < 0)
    {
        QMessageBox::warning(this, tr("Warning"), tr("No selected event."));
        return;
    }
    QString calID = m_calendars[currentCalIndex].toMap().value("id").toString();

    QString eventID = m_events[currentRow].toMap().value("id").toString();
    m_calendarDataManager.deleteEvent(m_pOauth2->accessToken(), calID, eventID);
}

void Form::updateEvent()
{
    qDebug() << Q_FUNC_INFO;
    int currentCalIndex = ui->calendarsListWidget->currentRow();

    if (currentCalIndex<0)
    {
        QMessageBox::warning(this, tr("Warning"), tr("No selected calendar."));
        return;
    }

    int currentRow = ui->eventsListWidget->currentRow();
    if(currentRow < 0)
    {
        QMessageBox::warning(this, tr("Warning"), tr("No selected event."));
        return;
    }
    QString calID = m_calendars[currentCalIndex].toMap().value("id").toString();
    qDebug() << "calID" << calID;

    QVariantMap eventData = m_events[currentRow].toMap();
    QString eventID = eventData.value("id").toString();
    eventData["summary"] = ui->summaryLineEdit->text();
    QVariantMap startData, endData;
    if(ui->timeGroupBox->isChecked())
    {
        QDateTime startDT(ui->startDateEdit->date(), ui->startTimeEdit->time());
        QDateTime endDT(ui->endDateEdit->date(), ui->endTimeEdit->time());
#if QT_VERSION >= 0x040800
        startData["dateTime"] = startDT.toUTC().toString(Qt::ISODate);
        endData["dateTime"] = endDT.toUTC().toString(Qt::ISODate);
#else
        startData["dateTime"] = startDT.toUTC().toString(Qt::ISODate)+"Z";
        endData["dateTime"] = endDT.toUTC().toString(Qt::ISODate)+"Z";
#endif
    }
    else
    {
        startData["date"] = ui->startDateEdit->date().toString(Qt::ISODate);
        endData["date"] = ui->endDateEdit->date().toString(Qt::ISODate);
    }
    eventData["start"] = startData;
    eventData["end"] = endData;
    eventData["location"] = ui->locationLineEdit->text();
    qDebug() << "eventData" << eventData;
    m_calendarDataManager.updateEvent(m_pOauth2->accessToken(), calID, eventID, eventData);
}

void Form::moveEvent()
{
    qDebug() << __FUNCTION__;
    int currentCalIndex = ui->calendarsListWidget->currentRow();

    if (currentCalIndex<0)
    {
        QMessageBox::warning(this, tr("Warning"), tr("No selected calendar."));
        return;
    }

    int currentRow = ui->eventsListWidget->currentRow();
    if(currentRow < 0)
    {
        QMessageBox::warning(this, tr("Warning"), tr("No selected event."));
        return;
    }
    QString calID = m_calendars[currentCalIndex].toMap().value("id").toString();
    QString eventID = m_events[currentRow].toMap().value("id").toString();

    QStringList calendarNames, calendarIds;
    for(int i = 0; i < m_calendars.count(); ++i)
    {
        //Only calendars where I have an accessRole as "owner" or "writer"
        if(m_calendars[i].toMap()["id"].toString() != calID &&
                (m_calendars[i].toMap()["accessRole"].toString() == "owner" ||
                 m_calendars[i].toMap()["accessRole"].toString() == "writer"))
        {
            calendarNames << m_calendars[i].toMap()["summary"].toString();
            calendarIds   << m_calendars[i].toMap()["id"].toString();
        }
    }
    bool bOk = false;
    QString strCal = QInputDialog::getItem(this, tr("Move Event"), tr("Select Destination Calendar"), calendarNames, 0, false, &bOk);
    if(!bOk)
    {
        return;
    }
    int index = calendarNames.indexOf(strCal);
    if(index == -1)
    {
        return;
    }
    QString destinationCalID = calendarIds[index];
    qDebug() << "Move to calendar" << strCal << destinationCalID;

    m_calendarDataManager.moveEvent(m_pOauth2->accessToken(), calID, eventID, destinationCalID);
}

void Form::editCalendar()
{
    int currentCalIndex = ui->calendarsListWidget->currentRow();

    if (currentCalIndex<0)
    {
        QMessageBox::warning(this, tr("Warning"), tr("No selected calendar."));
        return;
    }
    QString calID = m_calendars[currentCalIndex].toMap().value("id").toString();

    QString summary  = m_calendars[currentCalIndex].toMap().value("summary").toString();
    QString location = m_calendars[currentCalIndex].toMap().value("location").toString();
    QString description = m_calendars[currentCalIndex].toMap().value("description").toString();

    EditCalendarDialog dlg(this);
    dlg.setCalendarDetails(summary, location, description);
    int res = dlg.exec();
    if(res != QDialog::Accepted)
    {
        return;
    }
    dlg.getCalendarDetails(summary, location, description);
    qDebug() << "New calendar details" << summary << location << description;
    m_calendarDataManager.updateCalendar(m_pOauth2->accessToken(), calID, summary, location, description);
}

