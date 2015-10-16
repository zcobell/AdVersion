QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = adcirc_mesh_hash_gui
TEMPLATE = app

INCLUDEPATH += gui/include adcirc_hashlib/include

SOURCES += gui/src/main.cpp\
           gui/src/mainwindow.cpp \
           adcirc_hashlib/src/adcirc_hashlib.cpp \
           gui/src/pushbutton.cpp

HEADERS  += gui/include/mainwindow.h \
            adcirc_hashlib/include/adcirc_hashlib.h

FORMS    += gui/ui/mainwindow.ui

QMAKE_CXXFLAGS += -DGUI
