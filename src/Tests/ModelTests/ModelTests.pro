#-------------------------------------------------
#
# Project created by QtCreator 2013-05-14T14:59:22
#
#-------------------------------------------------

QT       += xml xmlpatterns testlib
QT       += gui

TARGET = tst_modeltests
CONFIG   += console
CONFIG   -= app_bundle

include(../../projectconfig.pri)
include(../../testconfig.pri)


LIBS += -lJobList -lTools -lHWDatabase

SOURCES += tst_modeltests.cc
HEADERS += tst_modeltests.h

QMAKE_CXXFLAGS_RELEASE += -fpermissive
QMAKE_CFLAGS_RELEASE += -fpermissive
