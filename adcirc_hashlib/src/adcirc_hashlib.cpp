//-----GPL----------------------------------------------------------------------
//
// This file is part of adcirc_hashlib
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
//  File: adcirc_hashlib.cpp
//
//------------------------------------------------------------------------------

#include "adcirc_hashlib.h"

//...Some checks for the progress bar in the GUI
QTime polling;
int progressUpdateInterval = 10;


//------------------------------------------------------------------------------
adcirc_hashlib::adcirc_hashlib(QObject *parent) : QObject(parent)
{

}
//------------------------------------------------------------------------------
