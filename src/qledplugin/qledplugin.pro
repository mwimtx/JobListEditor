include(../projectconfig.pri)

CONFIG      += designer plugin debug_and_release staticlib
TARGET      = $$qtLibraryTarget($$TARGET)
TEMPLATE    = lib
QT          += svg

DESTDIR = ../lib

HEADERS     = qled.h \
              qledplugin.h
SOURCES     = qled.cpp \
              qledplugin.cpp

RESOURCES = qled.qrc

#LIBS += /usr/local/Trolltech/Qt-4.6.2/lib/libQtDesigner.so


DEFINES += JOBLIST_LIBRARY

# install
target.path = $$[QT_INSTALL_PLUGINS]/designer
sources.files = $$SOURCES $$HEADERS *.pro
sources.path = $$[QT_INSTALL_EXAMPLES]/designer/qledplugin
INSTALLS += target sources

