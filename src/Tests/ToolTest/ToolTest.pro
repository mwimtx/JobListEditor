QT       += xml xmlpatterns testlib

QT       -= gui

TARGET = tst_tooltest
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

include(../../projectconfig.pri)
include(../../testconfig.pri)

LIBS += -lTools

SOURCES += tst_tooltest.cc
HEADERS += tst_tooltest.h

QMAKE_CXXFLAGS_RELEASE += -fpermissive
QMAKE_CFLAGS_RELEASE += -fpermissive
