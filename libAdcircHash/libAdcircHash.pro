#-------------------------------------------------
#
# Project created by QtCreator 2016-04-15T13:20:46
#
#-------------------------------------------------

QT       += gui

TARGET   = AdcircHash
TEMPLATE = lib
VERSION  = 0.0.0.1

win32{
TARGET_EXT = .dll
}

DEFINES += LIBADCIRCHASH_LIBRARY

SOURCES += libAdcircHash.cpp \
           ahl_adcirc_node.cpp \
           ahl_adcirc_element.cpp \
           ahl_adcirc_mesh.cpp \
           ahl_adcirc_boundary.cpp

HEADERS += libAdcircHash.h\
           libAdcircHash_global.h \
           ahl_adcirc_node.h \
           ahl_adcirc_element.h \
           ahl_adcirc_mesh.h \
           ahl_adcirc_boundary.h

QADCMODULES_SRCPATH   = "C:/Users/zcobell/Documents/Codes/QADCModules"
QADCMODULES_BUILDPATH = "C:/Users/zcobell/Documents/Codes/build-QADCModules-Desktop_Qt_5_6_0_MSVC2013_64bit-Release"

INCLUDEPATH += $$QADCMODULES_SRCPATH/QADCModules_lib
INCLUDEPATH += $$QADCMODULES_SRCPATH/thirdparty/boost-library
INCLUDEPATH += $$QADCMODULES_SRCPATH/thirdparty/kdtree
INCLUDEPATH += $$QADCMODULES_SRCPATH/netcdf/include
INCLUDEPATH += $$QADCMODULES_SRCPATH/thirdparty/proj4/src

LIBS += -L$$QADCMODULES_BUILDPATH/QADCModules_lib/release -lQADCModules

unix {
    target.path = /usr/lib
    INSTALLS += target
}
