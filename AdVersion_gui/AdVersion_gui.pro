#-------------------------------------------------
#
# Project created by QtCreator 2016-04-24T20:20:39
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = AdVersion_gui
TEMPLATE = app

#...QADCMODULES
win32{
QADCMODULES_SRCPATH   = "C:/Users/zcobell/Documents/Codes/QADCModules"
QADCMODULES_BUILDPATH = "C:/Users/zcobell/Documents/Codes/build-QADCModules-Desktop_Qt_5_6_0_MSVC2015_64bit-Release/QADCModules_lib/release"
INCLUDEPATH += $$QADCMODULES_SRCPATH/QADCModules_lib
INCLUDEPATH += $$QADCMODULES_SRCPATH/QADCModules_lib
INCLUDEPATH += $$QADCMODULES_SRCPATH/thirdparty/boost-library
INCLUDEPATH += $$QADCMODULES_SRCPATH/thirdparty/kdtree
INCLUDEPATH += $$QADCMODULES_SRCPATH/netcdf/include
INCLUDEPATH += $$QADCMODULES_SRCPATH/thirdparty/proj4/src
LIBS        += -L$$QADCMODULES_BUILDPATH -lQADCModules
}

unix{
QADCMODULES_PATH = /home/zcobell/Programs/QADCModules
QADCMODULES_SRCPATH = $$QADCMODULES_PATH/include
QADCMODULES_BUILDPATH  = $$QADCMODULES_PATH/lib
LIBS        += -L$$QADCMODULES_BUILDPATH -lQADCModules
INCLUDEPATH += $$QADCMODULES_SRCPATH
}

#...libAdVersion
INCLUDEPATH += $$PWD/../libAdVersion
win32{
CONFIG(debug, debug | release): LIBS += -L$$OUT_PWD/../libAdVersion/debug -lAdVersion
CONFIG(release, debug | release): LIBS += -L$$OUT_PWD/../libAdVersion/release -lAdVersion
}
unix{
LIBS += -L$$OUT_PWD/../libAdVersion -lAdVersion -lmetis
}

SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui
