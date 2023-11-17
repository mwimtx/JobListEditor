#-------------------------------------------------
#
# Project created by QtCreator 2014-02-07T13:55:39
#
#-------------------------------------------------
include(../projectconfig.pri)

QT       += xml xmlpatterns

QT       -= gui
DESTDIR = ../lib
TARGET = Tools
TEMPLATE = lib
CONFIG += staticlib

SOURCES += \
        domelementwalker.cc \
        tools_types.cc \
        default_values.cc \
        cli_parser.cc \
        xml_parse_helper.cc \
        xml_dom_helper.cc \
        codemapper.cc \
        geocoordinates.cpp \
    timer.cpp

HEADERS += \
        domelementwalker.h\
        tools_types.h \
        default_values.h \
        cli_parser.h \
        factories.h \
        xml_parse_helper.h \
        xml_dom_helper.h \
        codemapper.h \
        geocoordinates.h \
    timer.h


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
