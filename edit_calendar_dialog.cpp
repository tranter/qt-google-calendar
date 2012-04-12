#include "edit_calendar_dialog.h"
#include "ui_edit_calendar_dialog.h"

EditCalendarDialog::EditCalendarDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EditCalendarDialog)
{
    ui->setupUi(this);
}

EditCalendarDialog::~EditCalendarDialog()
{
    delete ui;
}

void EditCalendarDialog::setCalendarDetails(const QString& summary, const QString& location,
                                            const QString& description)
{
    ui->summaryLineEdit->setText(summary);
    ui->locationLineEdit->setText(location);
    ui->descriptionTextEdit->setPlainText(description);
}

void EditCalendarDialog::getCalendarDetails(QString& summary, QString& location, QString& description)
{
    summary = ui->summaryLineEdit->text();
    location = ui->locationLineEdit->text();
    description = ui->descriptionTextEdit->toPlainText();
}
