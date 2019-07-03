
QT -= gui
TARGET = adversion
TEMPLATE = lib
CONFIG -= 
CONFIG += c++11

DEFINES += ADVERSION_LIBRARY

#...Abseil path
ABSEILPATH = $$PWD/../../thirdparty/abseil

#...Boost Library
BOOSTPATH     = $$PWD/../../thirdparty/boost_1_66_0

#...Libgit2
LIBGIT2PATH   = $$PWD/../../thirdparty/libgit2

#...Metis
METISPATH     = $$PWD/../../thirdparty/metis-5.1.0

#...NetCDF
NETCDFHOME = /usr/local/Cellar/netcdf/4.6.3_1 
NETCDFPATH    = $$NETCDFHOME/include

#...AdcircModules
ADCIRCMODULESHOME = /Users/zcobell/software/adcircmodules

SOURCES += adversion.cpp \
    adversionimpl.cpp \
    partition.cpp

HEADERS += adversion.h \
    adversionimpl.h \
    partition.h

INCLUDEPATH += $$BOOSTPATH $$ABSEILPATH $$LIBGIT2PATH/include
INCLUDEPATH += $$NETCDFPATH/include $$METISPATH/include
INCLUDEPATH += $$ADCIRCMODULESHOME/include

LIBS += -lnetcdf 

