#-------------------------------------------------
#
# Project created by QtCreator 2014-02-07T13:51:13
#
#-------------------------------------------------
include(../projectconfig.pri)

QT       += core gui xml xmlpatterns

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = JobListEditor
TEMPLATE = app

INCLUDEPATH += ../Tools ../JobList ../HWDatabase


SOURCES += main.cc\
        mainwindow.cc \
        datetimeselector.cc \
        keypad.cc \
    simple_job_popup.cc \
    sensorswidget.cc \
    jobconfigwidget.cc \
    discretevalueselector.cc \
    settingsdialog.cc \
    commentswidget.cc \
    sensorpositions.cc \
    positioninputdialog.cc \
    txmparameters.cpp \
    txmelectrodepos.cpp \
    statekeypadwindow.cc \
    freekeypadwindow.cc \
    comparejoblists.cpp \
    fastbuttonselectpad.cpp \
    importautogainoffsetdialog.cpp \
    dialdialog.cpp

HEADERS  += mainwindow.h \
            datetimeselector.h \
            keypad.h \
    simple_job_popup.h \
    sensorswidget.h \
    jobconfigwidget.h \
    discretevalueselector.h \
    settingsdialog.h \
    commentswidget.h \
    sensorpositions.h \
    positioninputdialog.h \
    Strings.h \
    txmparameters.h \
    txmelectrodepos.h \
    statekeypadwindow.h \
    freekeypadwindow.h \
    comparejoblists.h \
    fastbuttonselectpad.h \
    importautogainoffsetdialog.h \
    dialdialog.h


FORMS    += mainwindow.ui \
            about.ui \
            datetimeselector.ui \
            keypad_free.ui \
    simple_job_popup.ui \
    jobconfigwidget.ui \
    discretevalueselector.ui \
    settingsdialog.ui \
    commentswidget.ui \
    sensorpositions.ui \
    positioninputdialog.ui \
    txmparameters.ui \
    txmelectrodepos.ui \
    keypad_free.ui \
    keypad_free_txm.ui \
    keypad_new.ui \
    comparejoblists.ui \
    fastbuttonselectpad.ui \
    importautogainoffsetdialog.ui \
    dialdialog.ui


LIBS += -L../lib
LIBS += -lJobList -lTools -lHWDatabase

procmtbfr {
unix {
CONFIG(debug) {DESTDIR = $$PWD/../../../proc_mt/debug/bin}
CONFIG(release) {DESTDIR = $$PWD/../../../proc_mt/release/bin}
} else {
dwin64 {
CONFIG(debug) {
DESTDIR = $$PWD/../../../proc_mt/debugw64/bin
} else {
CONFIG(release) { DESTDIR = $$PWD/../../../proc_mt/releasew64/bin/}
}
} else {
dwin32 {
CONFIG(debug) {DESTDIR = $$PWD/../../../proc_mt/debugw/bin }
CONFIG(release) { DESTDIR = $$PWD/../../../proc_mt/releasew/bin }
} else {
DESTDIR = bin
}
}
}
}
lcsubdirscript{
DESTDIR = ../../bin
}



RESOURCES += \
    jle_resources.qrc

RC_FILE += myapp.rc

QMAKE_CXXFLAGS_RELEASE += -fpermissive
QMAKE_CFLAGS_RELEASE += -fpermissive

