#-------------------------------------------------
#
# Project created by QtCreator 2016-04-15T13:20:46
#
#-------------------------------------------------

QT       -= gui

TARGET = adcirc_hashlib
TEMPLATE = lib

DEFINES += ADCIRC_HASHLIB_LIBRARY

SOURCES += adcirc_hashlib.cpp

HEADERS += adcirc_hashlib.h\
        adcirc_hashlib_global.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}
