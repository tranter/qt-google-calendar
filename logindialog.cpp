#include "logindialog.h"
#include "ui_logindialog.h"

#include <QDebug>
#include <QWebView>

LoginDialog::LoginDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoginDialog)
{
    ui->setupUi(this);
    connect(ui->webView, SIGNAL(urlChanged(QUrl)), this, SLOT(urlChanged(QUrl)));
    connect(ui->webView, SIGNAL(loadStarted()), this, SLOT(loadStarted()));
    connect(ui->webView, SIGNAL(loadFinished(bool)), this, SLOT(loadFinished(bool)));
}

LoginDialog::~LoginDialog()
{
    delete ui;
}

void LoginDialog::loadStarted()
{
    QApplication::setOverrideCursor(Qt::WaitCursor);
    qDebug() << "loadStarted";
}

void LoginDialog::loadFinished(bool b)
{
    QApplication::restoreOverrideCursor();
    qDebug() << "loadFinished with" << b;
}

void LoginDialog::urlChanged(const QUrl &url)
{
    qDebug() << "URL =" << url;
    QString str = url.toString();
    if(str.indexOf("access_token") != -1)
    {
        QStringList query = str.split("#");
        QStringList lst = query[1].split("&");
        for (int i=0; i<lst.count(); i++ )
        {
            QStringList pair = lst[i].split("=");
            if (pair[0] == "access_token")
            {
                m_strAccessToken = pair[1];
                emit accessTokenObtained();
                QDialog::accept();
            }
        }
    }
}

QString LoginDialog::accessToken()
{
    return m_strAccessToken;
}


void LoginDialog::setLoginUrl(const QString& url)
{
    //ui->webView->setUrl(QUrl(""));
   ui->webView->setUrl(QUrl(url));
}
