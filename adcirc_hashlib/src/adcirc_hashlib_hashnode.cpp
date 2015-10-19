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
//  File: adcirc_hashnode.cpp
//
//------------------------------------------------------------------------------

#include "adcirc_hashlib.h"

//------------------------------------------------------------------------------
//...Hash an ADCIRC node based on a node struct
//------------------------------------------------------------------------------
QString adcirc_hashlib::hashNode(adcirc_node &node)
{
    QString hashSeed,hashSeed1,hashSeed2;

    //...initialize the sha1 hash
    QCryptographicHash localHash(QCryptographicHash::Sha1);
    localHash.reset();

    //...Create a formatted string for each x, y and z;
    hashSeed1 = QString();
    hashSeed2 = QString();
    hashSeed1.sprintf("%+018.12e",node.x);
    hashSeed2.sprintf("%+018.12e",node.y);

    //...Concatenate the formatted strings together
    hashSeed = QString();
    hashSeed = hashSeed1+hashSeed2;

    //...Create the hash for the local node
    localHash.addData(hashSeed.toUtf8(),hashSeed.length());

    //...return the node hash
    return localHash.result().toHex();
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//...Hash an ADCIRC node with just x, y information
//------------------------------------------------------------------------------
QString adcirc_hashlib::hashNode(double x, double y)
{
    QString hashSeed,hashSeed1,hashSeed2;

    //...initialize the sha1 hash
    QCryptographicHash localHash(QCryptographicHash::Sha1);
    localHash.reset();

    //...Create a formatted string for each x, y and z;
    hashSeed1 = QString();
    hashSeed2 = QString();
    hashSeed1.sprintf("%+018.12e",x);
    hashSeed2.sprintf("%+018.12e",y);

    //...Concatenate the formatted strings together
    hashSeed = QString();
    hashSeed = hashSeed1+hashSeed2;

    //...Create the hash for the local node
    localHash.addData(hashSeed.toUtf8(),hashSeed.length());

    //...return the node hash
    return localHash.result().toHex();
}
//------------------------------------------------------------------------------
