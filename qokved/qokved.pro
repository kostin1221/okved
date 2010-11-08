#-------------------------------------------------
#
# Project created by QtCreator 2010-10-30T13:18:08
#
#-------------------------------------------------

unix {
  isEmpty(PREFIX) {
      PREFIX = /usr
}}

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

#TARGET = $$PREFIX/bin/qokved

target.path = $$PREFIX/bin
INSTALLS += target

LIBS += ../libqokved/libqokved.a 
PRE_TARGETDEPS += ../libqokved/libqokved.a

RESOURCES += \
    qokved.qrc

templates.files = templates/soffice.ods
win32 {
    templates.path = $$PREFIX
} else:mac {
    templates.path = $$PREFIX
} else {
    templates.path = $$PREFIX/share/qokved/templates
}

INSTALLS += templates

