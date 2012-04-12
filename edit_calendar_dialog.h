#ifndef EDIT_CALENDAR_DIALOG_H
#define EDIT_CALENDAR_DIALOG_H

#include <QDialog>

namespace Ui {
    class EditCalendarDialog;
}

class EditCalendarDialog : public QDialog
{
    Q_OBJECT

public:
    explicit EditCalendarDialog(QWidget *parent = 0);
    ~EditCalendarDialog();

    void setCalendarDetails(const QString& summary, const QString& location, const QString& description);
    void getCalendarDetails(QString& summary, QString& location, QString& description);


private:
    Ui::EditCalendarDialog *ui;
};

#endif // EDIT_CALENDAR_DIALOG_H
