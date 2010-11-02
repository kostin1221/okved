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
        qokvedmainwindow.cpp

HEADERS  += qokvedmainwindow.h

FORMS    += qokvedmainwindow.ui

TARGET = ../bin/qokved

LIBS += ../libqokved/libqokved.a
PRE_TARGETDEPS += ../libqokved/libqokved.a

