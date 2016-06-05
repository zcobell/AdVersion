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

#...Global include
include(../global.pri)

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

#...Source files
SOURCES += \
    rectangle.cpp \
    AdVersion.cpp \
    AdVersion_static.cpp \
    AdVersion_private.cpp \
    AdVersion_comparison.cpp

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

