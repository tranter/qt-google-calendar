# Introduction #

The qt-google-calendar example uses Google Calendar API. To use API you need to register your own application on Google. Do not worry: the procedure is very simple.


# Details #

You need to login to Google, so first you need to create simple Google account. Then you can visit the page

**https://code.google.com/apis/console**

there you can create your application. You need to check access to **Calendar API** in tab "Services" (set Status On as shown in the next figure).

![http://dl.dropbox.com/u/72326178/SetServiceCalendar.jpg](http://dl.dropbox.com/u/72326178/SetServiceCalendar.jpg)

Then you need create client ID (example page view for this in next figure).

![http://dl.dropbox.com/u/72326178/SetAPIAccess.jpg](http://dl.dropbox.com/u/72326178/SetAPIAccess.jpg)

Then  you can see credentials of your application. You need to copy and paste **Client\_ID** and **Redirect\_URI** to the file **oauth2.cpp**.
```
OAuth2::OAuth2(QWidget* parent)
{
    //You need to login to Google, so first you need to create simple
    //Google account. Then you can visit the page
    //
    // https://code.google.com/apis/console
    //
    //there you can create your application. You need to check access to Calendar API.
    //
    //Then  you can see credentials of your application.
    //You need to copy and paste Client_ID and Redirect_URI to the strings below.
    //
    m_strEndPoint = "https://accounts.google.com/o/oauth2/auth";
    m_strScope = "https://www.googleapis.com/auth/calendar"; //Access to Calendar service
    m_strClientID = "YOUR_CLIENT_ID_HERE";
    m_strRedirectURI = "YOUR_REDIRECT_URI_HERE";
    m_strResponseType = "token";
    ///Some more code below .....
}


```

After that you can compile and run **qt-google-calendar**.