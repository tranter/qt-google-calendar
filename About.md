# Introduction #

Information about use Google API, testing platforms, Qt versions.

# Details #

Project **qt-google-calendar** uses Google Calendar API v3.

### Google API ###

Project **qt-google-calendar** uses Google Calendar API v3.

How it works:
Work with Google-API perfomed by send https-request with using QNetworkAccessManager (see files _calendar\_data\_manager.h_ and _calendar\_data\_manager.cpp_)

API features used in this project:
|get calendar list|
|:----------------|
|get events for calendar|
|create calendar|
|delete calendar|
|update calendar|
|create event|
|move event|
|delete event|
|update event|

File **[HowToRegisterYourAppIicationInGoogle](http://code.google.com/p/qt-google-calendar/wiki/HowToRegisterYourApplicationInGoogle)** describes how register your own application on Google.

### Tested platforms ###
Project was tested on:
| **OS** | **Qt version** |
|:-------|:---------------|
|Linux 64bit|Qt 4.7.4, 4.8.1|
|Windows 7 64bit|Qt 4.8.0|
|Windows XP SP3 32bit|Qt 4.8.0|


# Various comments #

This application can be compiled on Linux, MacOS, Windows. For Windows you can use either MingW compiler or MSVC compiler.