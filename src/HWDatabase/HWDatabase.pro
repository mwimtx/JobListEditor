#-------------------------------------------------
#
# Project created by QtCreator 2014-02-07T13:52:34
#
#-------------------------------------------------
include(../projectconfig.pri)

QT       += xml xmlpatterns

QT       -= gui

TARGET = HWDatabase
TEMPLATE = lib
CONFIG += staticlib

INCLUDEPATH += ../Tools ../JobList
DESTDIR = ../lib

SOURCES += hwdatabase.cc \
        hwdbqueryresult.cc

HEADERS += hwdatabase.h \
        hwkeydata.h \
        hwdbqueryresult.h \
        hwdatabase_global.h

LIBS += -lTools


unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}

QMAKE_CXXFLAGS_RELEASE += -fpermissive
QMAKE_CFLAGS_RELEASE += -fpermissive
