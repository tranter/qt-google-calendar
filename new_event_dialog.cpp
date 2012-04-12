#include "new_event_dialog.h"
#include "ui_new_event_dialog.h"
#include <QMessageBox>

NewEventDialog::NewEventDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewEventDialog)
{
    ui->setupUi(this);
    ui->startDateEdit->setDate(QDate::currentDate());
    ui->endDateEdit->setDate(QDate::currentDate());
    ui->startTimeEdit->setTime(QTime::currentTime());
    ui->endTimeEdit->setTime(QTime::currentTime());
}

NewEventDialog::~NewEventDialog()
{
    delete ui;
}

void NewEventDialog::getNewEventParams(QString& summary, QDate& startDate, QDate& endDate, QTime& startTime, QTime& endTime)
{
    summary = ui->summaryLineEdit->text();
    startDate = ui->startDateEdit->date();
    endDate   = ui->endDateEdit->date();
    if(ui->timeGroupBox->isChecked())
    {
        startTime = ui->startTimeEdit->time();
        endTime   = ui->endTimeEdit->time();
    }
    else
    {
        startTime = QTime();
        endTime = QTime();
    }
}

void NewEventDialog::accept()
{
    if(ui->summaryLineEdit->text().isEmpty())
    {
        QMessageBox::warning(this, tr("Warning"), tr("Summary field is empty"));
        return;
    }
    QDate startDate = ui->startDateEdit->date();
    QDate endDate = ui->endDateEdit->date();
    if(startDate > endDate)
    {
        QMessageBox::warning(this, tr("Warning"), tr("Start date greater than end date."));
        return;
    }
    if(ui->timeGroupBox->isChecked())
    {
        QTime startTime = ui->startTimeEdit->time();
        QTime endTime   = ui->endTimeEdit->time();
        if(startDate == endDate)
        {
            if(startTime > endTime)
            {
                QMessageBox::warning(this, tr("Warning"), tr("Start time greater than end time."));
                return;
            }
        }
    }
    QDialog::accept();
}
