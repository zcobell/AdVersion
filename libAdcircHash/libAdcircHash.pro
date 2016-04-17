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

METISHOME = $$PWD/../thirdparty/metis
METISPATH = $$METISHOME/Lib
METISSRC  = $$METISPATH/coarsen.c $$METISPATH/fm.c $$METISPATH/initpart.c \
            $$METISPATH/match.c $$METISPATH/ccgraph.c $$METISPATH/memory.c \
            $$METISPATH/pmetis.c $$METISPATH/pqueue.c $$METISPATH/refine.c
            $$METISPATH/util.c $$METISPATH/timing.c $$METISPATH/debug.c \
            $$METISPATH/bucketsort.c $$METISPATH/graph.c $$METISPATH/stat.c \
            $$METISPATH/kmetis.c $$METISPATH/kwayrefine.c $$METISPATH/kwayfm.c \
            $$METISPATH/balance.c $$METISPATH/ometis.c $$METISPATH/srefine.c \
            $$METISPATH/sfm.c $$METISPATH/separator.c $$METISPATH/mincover.c \
            $$METISPATH/mmd.c $$METISPATH/mesh.c $$METISPATH/meshpart.c \
            $$METISPATH/frename.c $$METISPATH/fortran.c $$METISPATH/myqsort.c \
            $$METISPATH/compress.c $$METISPATH/parmetis.c $$METISPATH/estmem.c \
            $$METISPATH/mpmetis.c $$METISPATH/mcoarsen.c $$METISPATH/mmatch.c \
            $$METISPATH/minitpart.c $$METISPATH/mbalance.c $$METISPATH/mrefine.c \
            $$METISPATH/mutil.c $$METISPATH/mfm.c $$METISPATH/mkmetis.c \
            $$METISPATH/mkwayrefine.c $$METISPATH/mkwayfmh.c $$METISPATH/mrefine2.c \
            $$METISPATH/minitpart2.c $$METISPATH/mbalance2.c $$METISPATH/mfm2.c \
            $$METISPATH/kvmetis.c $$METISPATH/kwayvolrefine.c $$METISPATH/kwayvolfm.c \
            $$METISPATH/subdomains.c

DEFINES += LIBADCIRCHASH_LIBRARY

SOURCES += libAdcircHash.cpp $$METISSRC

HEADERS += libAdcircHash.h\
           libAdcircHash_global.h

QADCMODULES_SRCPATH   = "C:/Users/zcobell/Documents/Codes/QADCModules"
QADCMODULES_BUILDPATH = "C:/Users/zcobell/Documents/Codes/build-QADCModules-Desktop_Qt_5_6_0_MSVC2013_64bit-Debug/QADCModules_lib/debug"
#QADCMODULES_BUILDPATH = "C:\Users\zcobell\Documents\Codes\build-QADCModules-Desktop_Qt_5_6_0_MInGW_32bit-Debug\QADCModules_lib\debug"

INCLUDEPATH += $$QADCMODULES_SRCPATH/QADCModules_lib
INCLUDEPATH += $$QADCMODULES_SRCPATH/thirdparty/boost-library
INCLUDEPATH += $$QADCMODULES_SRCPATH/thirdparty/kdtree
INCLUDEPATH += $$QADCMODULES_SRCPATH/netcdf/include
INCLUDEPATH += $$QADCMODULES_SRCPATH/thirdparty/proj4/src
INCLUDEPATH += $$METISHOME/Lib

win32{
INCLUDEPATH += $$METISHOME/Windows
}

LIBS += -L$$QADCMODULES_BUILDPATH -lQADCModules

unix {
    target.path = /usr/lib
    INSTALLS += target
}
