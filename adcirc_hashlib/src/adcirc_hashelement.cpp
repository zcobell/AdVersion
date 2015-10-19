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
//  File: adcirc_hashelement.cpp
//
//------------------------------------------------------------------------------

#include "adcirc_hashlib.h"

//------------------------------------------------------------------------------
//...Hash an ADCIRC element using mesh and element ID as input
//------------------------------------------------------------------------------
QString adcirc_hashlib::hashElement(adcirc_mesh &mesh, int elementid)
{
    QString hashSeed,hashSeed1,hashSeed2,hashSeed3;

    //...initialize the sha1 hash
    QCryptographicHash localHash(QCryptographicHash::Sha1);
    localHash.reset();

    //...Create a formatted string for each vertex as a product
    //   of each location hash
    hashSeed1 = QString();
    hashSeed2 = QString();
    hashSeed3 = QString();

    //...Grab the node hashes to be the seeds for the element.
    //   Check if the node has been hashed previously and if not
    //   send it to the hashing function
    if(mesh.node[mesh.element[elementid].c1-1].locationHash==NULL)
        hashSeed1 = hashNode(mesh.node[mesh.element[elementid].c1-1]);
    else
        hashSeed1 = mesh.node[mesh.element[elementid].c1-1].locationHash;

    if(mesh.node[mesh.element[elementid].c2-1].locationHash==NULL)
        hashSeed1 = hashNode(mesh.node[mesh.element[elementid].c2-1]);
    else
        hashSeed1 = mesh.node[mesh.element[elementid].c2-1].locationHash;

    if(mesh.node[mesh.element[elementid].c3-1].locationHash==NULL)
        hashSeed1 = hashNode(mesh.node[mesh.element[elementid].c3-1]);
    else
        hashSeed1 = mesh.node[mesh.element[elementid].c3-1].locationHash;

    //...Check if the nodes to make sure the hash has already been computed
    if(hashSeed1==NULL || hashSeed2==NULL || hashSeed3==NULL)
        return QString("ERROR");

    //...Concatenate the formatted strings together
    hashSeed = QString();
    hashSeed = hashSeed1+hashSeed2+hashSeed3;

    //...Create the hash for the local node
    localHash.addData(hashSeed.toUtf8(),hashSeed.length());

    //...return the element hash
    return localHash.result().toHex();
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//...Hash an ADCIRC element using adcirc_node structs as input
//------------------------------------------------------------------------------
QString adcirc_hashlib::hashElement(adcirc_node n1, adcirc_node n2, adcirc_node n3)
{
    QString hashSeed,hashSeed1,hashSeed2,hashSeed3;

    //...initialize the sha1 hash
    QCryptographicHash localHash(QCryptographicHash::Sha1);
    localHash.reset();

    //...Create a formatted string for each vertex as a product
    //   of each location hash
    hashSeed1 = QString();
    hashSeed2 = QString();
    hashSeed3 = QString();

    //...Grab the node hashes to be the seeds for the element
    hashSeed1 = hashNode(n1);
    hashSeed2 = hashNode(n2);
    hashSeed3 = hashNode(n3);

    //...Concatenate the formatted strings together
    hashSeed = QString();
    hashSeed = hashSeed1+hashSeed2+hashSeed3;

    //...Create the hash for the local node
    localHash.addData(hashSeed.toUtf8(),hashSeed.length());

    //...return the element hash
    return localHash.result().toHex();
}
//----------------------------------------------------------------------------


//------------------------------------------------------------------------------
//...Hash an ADCIRC element using x,y locations as input
//------------------------------------------------------------------------------
QString adcirc_hashlib::hashElement(double x1, double y1, double x2, double y2, double x3, double y3)
{
    QString hashSeed,hashSeed1,hashSeed2,hashSeed3;

    //...initialize the sha1 hash
    QCryptographicHash localHash(QCryptographicHash::Sha1);
    localHash.reset();

    //...Create a formatted string for each vertex as a product
    //   of each location hash
    hashSeed1 = QString();
    hashSeed2 = QString();
    hashSeed3 = QString();

    //...Grab the node hashes to be the seeds for the element
    hashSeed1 = hashNode(x1,y1);
    hashSeed2 = hashNode(x2,y2);
    hashSeed3 = hashNode(x3,y3);

    //...Concatenate the formatted strings together
    hashSeed = QString();
    hashSeed = hashSeed1+hashSeed2+hashSeed3;

    //...Create the hash for the local node
    localHash.addData(hashSeed.toUtf8(),hashSeed.length());

    //...return the element hash
    return localHash.result().toHex();
}
//----------------------------------------------------------------------------
