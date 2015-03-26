# Introduction #

Short guide on compilation


# Details #

This project depends on QJson library.

You need to get all two projects by:

  * **_git clone https://code.google.com/p/qt-google-calendar/_**
  * **_git clone git://gitorious.org/qjson/qjson.git_**

All two folders: **_qt-google-calendar, qjson_** must be in the same directory.

Then you need to compile **qjson** library first.
We count that **qjson** library is in the **_qjson/build/lib_** directory.


Then the **qt-google-calendar** project will be compiled OK. To run it you need to adjust **LD\_LIBRARY\_PATH** on Linux (or **PATH** on Windows) so that variable points to the **_qjson/build/lib_** directory.