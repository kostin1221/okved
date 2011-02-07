#-------------------------------------------------
#
# Project created by QtCreator 2010-10-31T17:41:33
#
#-------------------------------------------------

QT       += core network sql

QT       += gui

TEMPLATE = lib
CONFIG += staticlib

SOURCES += libqokved.cpp \
    odswriter.cpp \
    okvedssortfilterproxymodel.cpp

HEADERS += libqokved.h \
    odswriter.h \
    okvedssortfilterproxymodel.h

win32:TARGET = qokved
else:TARGET = qokved

