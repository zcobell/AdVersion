#-------------------------------------------------
#
# Project created by QtCreator 2016-04-15T13:20:46
#
#-------------------------------------------------

QT       += gui

TARGET   = AdcircHash
TEMPLATE = lib
VERSION  = 0.0.0.1

win32 {
TARGET_EXT = .dll
}

METISHOME   = $$PWD/../thirdparty/metis-5.1.0
METISSRCDIR = $$METISHOME/libmetis
METISSRC    = $$METISSRCDIR/auxapi.c $$METISSRCDIR/balance.c $$METISSRCDIR/bucketsort.c \
              $$METISSRCDIR/checkgraph.c $$METISSRCDIR/coarsen.c $$METISSRCDIR/compress.c \
              $$METISSRCDIR/contig.c $$METISSRCDIR/debug.c $$METISSRCDIR/fm.c \
              $$METISSRCDIR/fortran.c $$METISSRCDIR/frename.c $$METISSRCDIR/gklib.c \
              $$METISSRCDIR/graph.c $$METISSRCDIR/initpart.c $$METISSRCDIR/kmetis.c \
              $$METISSRCDIR/kwayfm.c $$METISSRCDIR/kwayrefine.c $$METISSRCDIR/mcutil.c \
              $$METISSRCDIR/mesh.c $$METISSRCDIR/meshpart.c $$METISSRCDIR/minconn.c \
              $$METISSRCDIR/mincover.c $$METISSRCDIR/mmd.c $$METISSRCDIR/ometis.c \
              $$METISSRCDIR/options.c $$METISSRCDIR/parmetis.c $$METISSRCDIR/pmetis.c \
              $$METISSRCDIR/refine.c $$METISSRCDIR/separator.c $$METISSRCDIR/sfm.c \
              $$METISSRCDIR/srefine.c $$METISSRCDIR/stat.c $$METISSRCDIR/timing.c \
              $$METISSRCDIR/util.c $$METISSRCDIR/wspace.c

GKLIBSRCDIR  = $$METISHOME/GKlib
GKLIBSRC     = $$GKLIBSRCDIR/b64.c $$GKLIBSRCDIR/blas.c $$GKLIBSRCDIR/csr.c $$GKLIBSRCDIR/error.c \
               $$GKLIBSRCDIR/evaluate.c $$GKLIBSRCDIR/fkvkselect.c $$GKLIBSRCDIR/fs.c \
               $$GKLIBSRCDIR/getopt.c $$GKLIBSRCDIR/gkregex.c $$GKLIBSRCDIR/gklib_graph.c \
               $$GKLIBSRCDIR/htable.c $$GKLIBSRCDIR/io.c $$GKLIBSRCDIR/itemsets.c \
               $$GKLIBSRCDIR/mcore.c $$GKLIBSRCDIR/memory.c $$GKLIBSRCDIR/omp.c \
               $$GKLIBSRCDIR/pdb.c $$GKLIBSRCDIR/pqueue.c $$GKLIBSRCDIR/random.c \
               $$GKLIBSRCDIR/rw.c $$GKLIBSRCDIR/seq.c $$GKLIBSRCDIR/sort.c \
               $$GKLIBSRCDIR/string.c $$GKLIBSRCDIR/timers.c $$GKLIBSRCDIR/tokenizer.c \
               $$GKLIBSRCDIR/gklib_util.c

DEFINES += LIBADCIRCHASH_LIBRARY 

INCLUDEPATH += $$GKLIBSRCDIR $$METISSRCDIR $$METISHOME/include

SOURCES += $$GKLIBSRC $$METISSRC libAdcircHash.cpp

HEADERS += libAdcircHash.h\
           libAdcircHash_global.h


win32 {
QADCMODULES_SRCPATH   = "C:/Users/zcobell/Documents/Codes/QADCModules"
QADCMODULES_BUILDPATH = "C:/Users/zcobell/Documents/Codes/build-QADCModules-Desktop_Qt_5_6_0_MSVC2013_64bit-Debug/QADCModules_lib/debug"
INCLUDEPATH += $$QADCMODULES_SRCPATH/QADCModules_lib
INCLUDEPATH += $$QADCMODULES_SRCPATH/thirdparty/boost-library
INCLUDEPATH += $$QADCMODULES_SRCPATH/thirdparty/kdtree
INCLUDEPATH += $$QADCMODULES_SRCPATH/netcdf/include
INCLUDEPATH += $$QADCMODULES_SRCPATH/thirdparty/proj4/src

#...Metis needs these defines under MSVC2015 (64 bit)
DEFINES     += WIN32 MSC _CRT_SECURE_NO_DEPRECATE USE_GKREGEX NDEBUG NDEBUG2 __thread=__declspec(thread)
}

unix {
QADCMODULES_PATH = /home/zcobell/Programs/QADCModules
QADCMODULES_SRCPATH = $$QADCMODULES_PATH/include
QADCMODULES_BUILDPATH  = $$QADCMODULES_PATH/lib
INCLUDEPATH += $$QADCMODULES_SRCPATH
}


LIBS += -L$$QADCMODULES_BUILDPATH -lQADCModules

unix {
    target.path = /usr/lib
    INSTALLS += target
}
