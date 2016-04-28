//-----GPL----------------------------------------------------------------------
//
// This file is part of libAdVersion
// Copyright (C) 2015  Zach Cobell
//
//
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
//------------------------------------------------------------------------------
//
//  File: libAdVersion_global.h
//
//------------------------------------------------------------------------------
#ifndef ADVERSION_GLOBAL_H
#define ADVERSION_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(LIBADVERSION_LIBRARY)
#  define LIBADVERSIONSHARED_EXPORT Q_DECL_EXPORT
#else
#  define LIBADVERSIONSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // ADCIRC_HASHLIB_GLOBAL_H
