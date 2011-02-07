# -------------------------------------------------
# Project created by QtCreator 2010-10-30T13:18:08
# -------------------------------------------------
unix:isEmpty(PREFIX):PREFIX = /usr
QT += core \
    gui \
    sql
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

# TARGET = $$PREFIX/bin/qokved
target.path = $$PREFIX/bin
INSTALLS += target

CONFIG( debug, debug|release ) {
		LIBS += ../libqokved/debug/libqokved.a
		PRE_TARGETDEPS += ../libqokved/debug/libqokved.a
} else {
		LIBS += ../libqokved/Release/libqokved.a
		PRE_TARGETDEPS += ../libqokved/Release/libqokved.a
}

RESOURCES += qokved.qrc
templates.files = templates/soffice.ods \
    templates/qokved.db.default
win32:templates.path = $$PREFIX
else:mac:templates.path = $$PREFIX
else:templates.path = $$PREFIX/share/qokved/templates
INSTALLS += templates
