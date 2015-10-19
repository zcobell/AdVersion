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
//  File: adcirc_hashbc.cpp
//
//------------------------------------------------------------------------------

#include "adcirc_hashlib.h"

//------------------------------------------------------------------------------
//...Hash an adcirc Open boundary condition
//------------------------------------------------------------------------------
QString adcirc_hashlib::hashOpenBC(adcirc_mesh &mesh, int OpenBCIndex)
{
    QString hashSeed;
    int j;

    //...Initialize the sha1 hash
    QCryptographicHash localHash(QCryptographicHash::Sha1);
    localHash.reset();

    //Hash the boundary code, for open boundaries, -1
    hashSeed.sprintf("%+6i",-1);
    localHash.addData(hashSeed.toUtf8(),hashSeed.length());

    //Accumulate a hash along the boundary string
    for(j=0;j<mesh.openBoundaryH[OpenBCIndex].NumNodes;j++)
    {
        hashSeed = QString();
        hashSeed = mesh.node[mesh.openBoundaryH[OpenBCIndex].node1[j]-1].locationHash;
        localHash.addData(hashSeed.toUtf8(),hashSeed.length());
    }
    return localHash.result().toHex();
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//...Hash an adcirc land boundary condition
//------------------------------------------------------------------------------
QString adcirc_hashlib::hashLandBC(adcirc_mesh &mesh, int LandBCIndex)
{
    QString hashSeed;
    int j;

    //...Initialize the sha1 hash
    QCryptographicHash localHash(QCryptographicHash::Sha1);
    localHash.reset();

    //Hash the boundary code
    hashSeed.sprintf("%+6i",mesh.landBoundaryH[LandBCIndex].code);
    localHash.addData(hashSeed.toUtf8(),hashSeed.length());

    //Accumulate a hash along the boundary string
    for(j=0;j<mesh.landBoundaryH[LandBCIndex].NumNodes;j++)
    {

        //Single node boundaries and dual node boundaries
        //lumped since we only care if the position
        //changes, not the attributes
        if(mesh.landBoundaryH[LandBCIndex].code  == 0   ||
            mesh.landBoundaryH[LandBCIndex].code == 1   ||
            mesh.landBoundaryH[LandBCIndex].code == 2   ||
            mesh.landBoundaryH[LandBCIndex].code == 10  ||
            mesh.landBoundaryH[LandBCIndex].code == 11  ||
            mesh.landBoundaryH[LandBCIndex].code == 12  ||
            mesh.landBoundaryH[LandBCIndex].code == 20  ||
            mesh.landBoundaryH[LandBCIndex].code == 21  ||
            mesh.landBoundaryH[LandBCIndex].code == 22  ||
            mesh.landBoundaryH[LandBCIndex].code == 30  ||
            mesh.landBoundaryH[LandBCIndex].code == 52  ||
            mesh.landBoundaryH[LandBCIndex].code == 102 ||
            mesh.landBoundaryH[LandBCIndex].code == 112 ||
            mesh.landBoundaryH[LandBCIndex].code == 122 ||
            mesh.landBoundaryH[LandBCIndex].code == 3   ||
            mesh.landBoundaryH[LandBCIndex].code == 13  ||
            mesh.landBoundaryH[LandBCIndex].code == 23 )
        {
            //Accumulate a hash along the boundary string
            for(j=0;j<mesh.landBoundaryH[LandBCIndex].NumNodes;j++)
            {
                hashSeed = QString();
                hashSeed = mesh.node[mesh.landBoundaryH[LandBCIndex].node1[j]-1].locationHash;
                localHash.addData(hashSeed.toUtf8(),hashSeed.length());
            }
        }
        else if(mesh.landBoundaryH[LandBCIndex].code == 4  ||
                mesh.landBoundaryH[LandBCIndex].code == 24 ||
                mesh.landBoundaryH[LandBCIndex].code == 5  ||
                mesh.landBoundaryH[LandBCIndex].code == 25 )
        {
            //Accumulate a hash along the boundary string
            for(j=0;j<mesh.landBoundaryH[LandBCIndex].NumNodes;j++)
            {
                hashSeed = QString();
                hashSeed = mesh.node[mesh.landBoundaryH[LandBCIndex].node1[j]-1].locationHash +
                           mesh.node[mesh.landBoundaryH[LandBCIndex].node2[j]-1].locationHash;
                localHash.addData(hashSeed.toUtf8(),hashSeed.length());
            }
        }
        else
        {
            //Accumulate a hash along the boundary string
            for(j=0;j<mesh.landBoundaryH[LandBCIndex].NumNodes;j++)
            {
                hashSeed = QString();
                hashSeed = mesh.node[mesh.landBoundaryH[LandBCIndex].node1[j]-1].locationHash;
                localHash.addData(hashSeed.toUtf8(),hashSeed.length());
            }
        }
    }
    return localHash.result().toHex();
}
//------------------------------------------------------------------------------
