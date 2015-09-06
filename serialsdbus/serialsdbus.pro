TEMPLATE = app
TARGET = serialsdbus
QT += core dbus

inst1.files += conf/org.freedesktop.serials.service
inst1.path = /usr/share/dbus-1/system-services/
inst2.files += conf/org.freedesktop.serials.policy
inst2.path = /usr/share/polkit-1/actions/
inst3.files += conf/org.freedesktop.serials.conf
inst3.path = /etc/dbus-1/system.d/
target.source += $$TARGET
target.path = /usr/lib/serials
INSTALLS += inst1 \
    inst2 \
    inst3 \
    target

HEADERS += \
    serialsAdaptor.h \
    serialsServer.h \
    commonserver.h \
    commonAdaptor.h

SOURCES += main.cpp \
    serialsAdaptor.cpp \
    serialsServer.cpp \
    commonserver.cpp \
    commonAdaptor.cpp


unix|win32: LIBS += -lQt5SerialPort
