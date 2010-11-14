#-------------------------------------------------
#
# Project created by QtCreator 2010-10-31T17:41:33
#
#-------------------------------------------------

QT       += network sql

QT       += gui

TARGET = libqokved
TEMPLATE = lib
CONFIG += staticlib

SOURCES += libqokved.cpp \
    odswriter.cpp \
    okvedssortfilterproxymodel.cpp

HEADERS += libqokved.h \
    odswriter.h \
    okvedssortfilterproxymodel.h

TARGET = qokved

