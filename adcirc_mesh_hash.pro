QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = adcirc_mesh_hash
TEMPLATE = app

INCLUDEPATH += include

SOURCES += src/main.cpp\
           src/mainwindow.cpp \
           src/adcirc_io.cpp

HEADERS  += include/mainwindow.h \
            include/adcirc_io.h

FORMS    += ui/mainwindow.ui
