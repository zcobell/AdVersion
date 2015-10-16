#-------------------------------------------------
#
# Project created by QtCreator 2015-10-16T13:18:48
#
#-------------------------------------------------

QT       += core

QT       -= gui

INCLUDEPATH = adcirc_hashlib/include cmd/include

TARGET = adcirc_mesh_hash_cmd
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

HEADERS  += adcirc_hashlib/include/adcirc_hashlib.h


SOURCES += adcirc_hashlib/src/adcirc_hashlib.cpp \
           cmd/src/adcirc_hashmesh_cmd.cpp
