QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = adcirc_hashmesh_gui
TEMPLATE = app

INCLUDEPATH += include ../adcirc_hashlib/include

SOURCES += src/main.cpp\
           src/mainwindow.cpp \
           ../adcirc_hashlib/src/adcirc_hashlib.cpp \
           src/pushbutton.cpp

HEADERS  += include/mainwindow.h \
            ../adcirc_hashlib/include/adcirc_hashlib.h

FORMS    += ui/mainwindow.ui

QMAKE_CXXFLAGS += -DGUI
