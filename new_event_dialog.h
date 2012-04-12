#ifndef NEW_EVENT_DIALOG_H
#define NEW_EVENT_DIALOG_H

#include <QDialog>
#include <QDate>
#include <QTime>

namespace Ui {
    class NewEventDialog;
}

class NewEventDialog : public QDialog
{
    Q_OBJECT

public:
    explicit NewEventDialog(QWidget *parent = 0);
    ~NewEventDialog();

    void getNewEventParams(QString& summary, QDate& startDate, QDate& endDate, QTime& startTime, QTime& endTime);

    void accept();

private:
    Ui::NewEventDialog *ui;
};

#endif // NEW_EVENT_DIALOG_H
