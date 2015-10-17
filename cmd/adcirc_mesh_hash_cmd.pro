#-------------------------------------------------
#
# Project created by QtCreator 2015-10-16T13:18:48
#
#-------------------------------------------------

QT       += core

QT       -= gui

INCLUDEPATH = ../adcirc_hashlib/include cmd/include

TARGET = adcirc_hashmesh
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

HEADERS  += ../adcirc_hashlib/include/adcirc_hashlib.h


SOURCES += ../adcirc_hashlib/src/adcirc_hashlib.cpp \
           src/adcirc_hashmesh_cmd.cpp
