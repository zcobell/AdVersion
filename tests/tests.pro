QT += core
QT += gui

CONFIG += c++11

TARGET = tests
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

#...QADCMODULES
QADCMODULES_SRCPATH   = "C:/Users/zcobell/Documents/Codes/QADCModules"
QADCMODULES_BUILDPATH = "C:/Users/zcobell/Documents/Codes/build-QADCModules-Desktop_Qt_5_6_0_MSVC2013_64bit-Release"

INCLUDEPATH += $$QADCMODULES_SRCPATH/QADCModules_lib
INCLUDEPATH += $$QADCMODULES_SRCPATH/QADCModules_lib
INCLUDEPATH += $$QADCMODULES_SRCPATH/thirdparty/boost-library
INCLUDEPATH += $$QADCMODULES_SRCPATH/thirdparty/kdtree
INCLUDEPATH += $$QADCMODULES_SRCPATH/netcdf/include
INCLUDEPATH += $$QADCMODULES_SRCPATH/thirdparty/proj4/src
LIBS        += -L$$QADCMODULES_BUILDPATH/QADCModules_lib/release -lQADCModules

#...libAdcircHash
INCLUDEPATH += $$PWD/../libAdcircHash
win32{
CONFIG(debug, debug | release): LIBS += -L$$OUT_PWD/../libAdcircHash/debug -lAdcircHash
CONFIG(release, debug | release): LIBS += -L$$OUT_PWD/../libAdcircHash/release -lAdcircHash
}

unix{
LIBS += -L$$OUT_PWD/../QADCModules_lib -lQADCModules
}

SOURCES += main.cpp
