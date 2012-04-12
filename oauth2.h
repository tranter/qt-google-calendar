#ifndef OAUTH2_H
#define OAUTH2_H

#include <QString>
#include <QObject>

class LoginDialog;

class OAuth2 : public QObject
{
    Q_OBJECT

public:
    OAuth2(QWidget* parent = 0);
    QString accessToken();
    bool isAuthorized();
    void startLogin(bool bForce);

    QString loginUrl();

signals:
    void loginDone();

private slots:
    void accessTokenObtained();

private:
    QString m_strAccessToken;

    QString m_strEndPoint;
    QString m_strScope;
    QString m_strClientID;
    QString m_strRedirectURI;
    QString m_strResponseType;

    LoginDialog* m_pLoginDialog;
    QWidget* m_pParent;
};

#endif // OAUTH2_H
