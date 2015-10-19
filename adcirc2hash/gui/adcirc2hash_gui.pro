#-------------------------------GPL-------------------------------------#
#
# This file is part of adcirc2hash 
# Copyright (C) 2015  Zach Cobell
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#
#-----------------------------------------------------------------------#

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = adcirc2hash_gui
TEMPLATE = app

INCLUDEPATH += include ../../adcirc_hashlib/include

SOURCES += src/main.cpp\
           src/mainwindow.cpp \
           src/pushbutton.cpp \
           ../../adcirc_hashlib/src/adcirc_hashlib.cpp \
           ../../adcirc_hashlib/src/adcirc_hashlib_fileio.cpp \
           ../../adcirc_hashlib/src/adcirc_hashlib_hashnode.cpp \
           ../../adcirc_hashlib/src/adcirc_hashlib_hashelement.cpp \
           ../../adcirc_hashlib/src/adcirc_hashlib_hashbc.cpp \
           ../../adcirc_hashlib/src/adcirc_hashlib_sorthash.cpp \
           ../../adcirc_hashlib/src/adcirc_hashlib_hashmesh.cpp \
           ../../adcirc_hashlib/src/adcirc_hashlib_numbermesh.cpp

HEADERS  += include/mainwindow.h \
            ../../adcirc_hashlib/include/adcirc_hashlib.h

FORMS    += ui/mainwindow.ui
