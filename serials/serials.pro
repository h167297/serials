#-------------------------------------------------
#
# Project created by QtCreator 2014-12-16T13:32:36
#
#-------------------------------------------------

QT       += core gui network dbus

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = serials
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    serialsdispatcher.cpp

HEADERS  += mainwindow.h \
    serialsdispatcher.h

FORMS    += mainwindow.ui

RESOURCES += \
    resources.qrc

RC_FILE = appICO.rc


unix|win32: LIBS += -lQt5SerialPort
