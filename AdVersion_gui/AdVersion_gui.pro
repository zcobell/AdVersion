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

#...User should set paths in this file
include(../config.pri)

#...Global include
include(../global.pri)

#...Base Qt Configuration
QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

#...Target configuration
TARGET = AdVersion_gui
TEMPLATE = app

#...Including libAdVersion. Note that METIS is built directly for win32 while
#   Unix assumes it is installed
INCLUDEPATH += $$PWD/../libAdVersion
win32{
CONFIG(debug, debug | release): LIBS += -L$$OUT_PWD/../libAdVersion/debug -lAdVersion
CONFIG(release, debug | release): LIBS += -L$$OUT_PWD/../libAdVersion/release -lAdVersion
}

unix{
LIBS += -L$$OUT_PWD/../libAdVersion -lAdVersion
}

SOURCES += main.cpp\
        mainwindow.cpp \
        advfolderchooser.cpp \
        advqfilesystemmodel.cpp \
    worker.cpp

HEADERS  += mainwindow.h \
            advfolderchooser.h \
            advqfilesystemmodel.h \
    worker.h

FORMS    += mainwindow.ui \
            advfolderchooser.ui

DISTFILES +=

RESOURCES += resource.qrc
