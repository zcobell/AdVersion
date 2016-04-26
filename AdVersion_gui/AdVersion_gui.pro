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
#  File: AdVersion_gui.pro
#
#------------------------------------------------------------------------------

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
        mainwindow.cpp \
    advfolderchooser.cpp \
    advqfilesystemmodel.cpp

HEADERS  += mainwindow.h \
    advfolderchooser.h \
    advqfilesystemmodel.h

FORMS    += mainwindow.ui \
    advfolderchooser.ui

DISTFILES +=

RESOURCES += \
    resource.qrc
