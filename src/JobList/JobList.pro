#-------------------------------------------------
#
# Project created by QtCreator 2014-02-07T13:53:49
#
#-------------------------------------------------
include(../projectconfig.pri)

QT       += xml

QT       += gui

TARGET = JobList
TEMPLATE = lib
CONFIG += staticlib
DESTDIR = ../lib

SOURCES += \
    job.cc \
    datamodel.cc \
    jobchannelconfig.cc \
    sensorchannelconfig.cc \
    joblistmodel.cc

HEADERS += \
    job.h \
    datamodel.h \
    jobchannelconfig.h \
    sensorchannelconfig.h \
    joblistmodel.h

LIBS += -lTools -lHWDatabase

INCLUDEPATH += ../Tools ../JobList ../HWDatabase ../Gui


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
