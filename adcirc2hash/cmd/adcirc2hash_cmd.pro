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

QT       += core

QT       -= gui

INCLUDEPATH = ../../adcirc_hashlib/include cmd/include

TARGET = adcirc2hash
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

HEADERS  += ../../adcirc_hashlib/include/adcirc_hashlib.h


SOURCES += ../../adcirc_hashlib/src/adcirc_hashlib.cpp \
           ../../adcirc_hashlib/src/adcirc_hashlib_fileio.cpp \
           ../../adcirc_hashlib/src/adcirc_hashlib_hashnode.cpp \
           ../../adcirc_hashlib/src/adcirc_hashlib_hashelement.cpp \
           ../../adcirc_hashlib/src/adcirc_hashlib_hashbc.cpp \
           ../../adcirc_hashlib/src/adcirc_hashlib_sorthash.cpp \
           ../../adcirc_hashlib/src/adcirc_hashlib_hashmesh.cpp \
           ../../adcirc_hashlib/src/adcirc_hashlib_numbermesh.cpp \
           src/adcirc2hash_cmd.cpp
