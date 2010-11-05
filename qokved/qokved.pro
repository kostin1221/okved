#-------------------------------------------------
#
# Project created by QtCreator 2010-10-30T13:18:08
#
#-------------------------------------------------

QT       += core gui sql

TARGET = qokved
TEMPLATE = app

INCLUDEPATH += ../libqokved

SOURCES += main.cpp\
        qokvedmainwindow.cpp \
    addokveddialog.cpp

HEADERS  += qokvedmainwindow.h \
    addokveddialog.h

FORMS    += qokvedmainwindow.ui \
    addokveddialog.ui

TARGET = ../bin/qokved

LIBS += ../libqokved/libqokved.a 
LIBS += -lsqlite3
PRE_TARGETDEPS += ../libqokved/libqokved.a

RESOURCES += \
    qokved.qrc
