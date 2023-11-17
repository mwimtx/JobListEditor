#-------------------------------------------------
#
# Project created by QtCreator 2013-06-13T15:25:37
#
#-------------------------------------------------

QT       += xml xmlpatterns testlib
QT       -= gui

TARGET = tst_hwdbteststest
CONFIG   += console
CONFIG   -= app_bundle


include(../../projectconfig.pri)
include(../../testconfig.pri)

LIBS +=  -lHWDatabase -lTools

SOURCES += tst_hwdbteststest.cc
HEADERS += tst_hwdbteststest.h


QMAKE_CXXFLAGS_RELEASE += -fpermissive
QMAKE_CFLAGS_RELEASE += -fpermissive
