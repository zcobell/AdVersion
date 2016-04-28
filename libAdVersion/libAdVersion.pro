#-----GPL----------------------------------------------------------------------
#
# This file is part of AdVersion
# Copyright (C) 2015  Zach Cobell
#
#
#  This program is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program.  If not, see <http://www.gnu.org/licenses/>.
#
#------------------------------------------------------------------------------
#
#  File: libAdVersion.pro
#
#------------------------------------------------------------------------------

#...User should set paths in this file
include(../config.pri)

QT       += gui

TARGET   = AdVersion
TEMPLATE = lib
VERSION  = 0.0.1

win32 {
TARGET_EXT = .dll
}

DEFINES += LIBADVERSION_LIBRARY

HEADERS += rectangle.h \
           libAdVersion_global.h \
           AdVersion.h

#...QADCModules
INCLUDEPATH += $$QADCMODULES_PATH/include

#...LibGit2
INCLUDEPATH += $$LIBGIT2_PATH/include

#...Add libraries
win32{
LIBS += -L$$QADCMODULES_PATH/bin -lQADCModules -L$$LIBGIT2_PATH/bin -lgit2
}
unix {
LIBS += -L$$QADCMODULES_PATH/lib -lQADCModules -lmetis -L$$LIBGIT2_PATH/lib -lgit2 -lmetis
}


#...Source files
SOURCES += \
    rectangle.cpp \
    AdVersion.cpp

#...Install location
win32{
    isEmpty(PREFIX) {
        PREFIX = $$OUT_PWD
    }
    target.path = $$PREFIX/bin
    header_files.files = $$HEADERS
    header_files.path = $$PREFIX/include
    INSTALLS += header_files target
}

unix{
    isEmpty(PREFIX) {
        PREFIX = /usr/local
    }
    target.path = $$PREFIX/lib
    header_files.files = $$HEADERS
    header_files.path = $$PREFIX/include
    INSTALLS += header_files target
}


#############################
#    METIS (windows only)   #
#############################

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

win32{
SOURCES += $$GKLIBSRC $$METISSRC
INCLUDEPATH += $$GKLIBSRCDIR $$METISSRCDIR $$METISHOME/include

#...Metis needs these defines under MSVC2015 (64 bit)
DEFINES     += WIN32 MSC _CRT_SECURE_NO_DEPRECATE USE_GKREGEX NDEBUG NDEBUG2 __thread=__declspec(thread)
}
