#-------------------------------------------------
#
# Project created by QtCreator 2018-11-06T13:52:44
#
#-------------------------------------------------

QT       += network

QT       -= gui

TARGET = sensorsunleashed
TEMPLATE = lib
CONFIG += c++11
DEFINES += SENSORSUNLEASHED_LIBRARY

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        sensorsunleashed.cpp \
        socket.cpp \
        cantcoap/cantcoap.cpp \
        node.cpp \
        sensor.cpp \
        cmp_helpers.c \
        cmp.c \
        crc16.c \
        suinterface.cpp \
        su_message.cpp \
    coap_server.cpp \
    coap_transaction.cpp \
    coap_engine.cpp \
    coap_transmit.cpp \
    coap_resource.cpp

HEADERS += \
        sensorsunleashed.h \
        sensorsunleashed_global.h \ 
        cantcoap/cantcoap.h \
        cantcoap/dbg.h \
        cantcoap/sysdep.h \
        node.h \
        cmp_helpers.h \
        cmp.h \
        crc16.h \
        su_message.h \
    coap_server.h \
    coap_transaction.h \
    coap_engine.h \
    wsn.h \
    coap_resource.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}


