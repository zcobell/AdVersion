QT += core
QT += gui

CONFIG += c++11

TARGET = tests
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

#...QADCMODULES
win32{
QADCMODULES_SRCPATH   = "C:/Users/zcobell/Documents/Codes/QADCModules"
QADCMODULES_BUILDPATH = "C:/Users/zcobell/Documents/Codes/build-QADCModules-Desktop_Qt_5_6_0_MSVC2013_64bit-Debug/QADCModules_lib/debug"
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
CONFIG(debug, debug | release): LIBS += -L$$OUT_PWD/../libAdcircHash/debug -lAdVersion
CONFIG(release, debug | release): LIBS += -L$$OUT_PWD/../libAdcircHash/release -lAdVersion
}
unix{
LIBS += -L$$OUT_PWD/../libAdVersion -lAdVersion
}

SOURCES += main.cpp
