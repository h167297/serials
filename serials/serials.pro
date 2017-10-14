#-------------------------------------------------
#
# Project created by QtCreator 2014-12-16T13:32:36
#
#-------------------------------------------------

QT       += core gui network dbus

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets serialport

TARGET = serials
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    serialsdispatcher.cpp \
    frmmessagebox.cpp \
    iconhelper.cpp \
    scanthread.cpp \
    commondispatcher.cpp

HEADERS  += mainwindow.h \
    serialsdispatcher.h \
    frmmessagebox.h \
    iconhelper.h \
    myhelper.h \
    scanthread.h \
    commondispatcher.h

FORMS    += mainwindow.ui \
    frmmessagebox.ui

RESOURCES += \
    resources.qrc

RC_FILE = appinfo.rc
