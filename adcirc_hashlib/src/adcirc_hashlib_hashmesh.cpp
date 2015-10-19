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
//  File: adcirc_hashmesh.cpp
//
//------------------------------------------------------------------------------

#include "adcirc_hashlib.h"

//------------------------------------------------------------------------------
//...Function to create the hashes for an entire adcirc mesh and save in the
//   mesh structure
//------------------------------------------------------------------------------
int adcirc_hashlib::createAdcircHashes(adcirc_mesh &mesh)
{

    int i;

    //...loop over nodes to generate a unique hash
    //   for each node
    for(i=0;i<mesh.NumNodes;i++)
        mesh.node[i].locationHash = adcirc_hashlib::hashNode(mesh.node[i]);

    //...Now create the hash for each element which is based upon the
    //   locations of each hash
    for(i=0;i<mesh.NumElements;i++)
    {
        //...Save the local hash for this node into the array
        mesh.element[i].elementHash = hashElement(mesh,i);

        //...Check for errors
        if(mesh.element[i].elementHash == "ERROR")
            return ERR_UNKNOWN;
    }

    //...Hash the open boundary conditions
    for(i=0;i<mesh.NumOpenBoundaries;i++)
        mesh.openBoundaryH[i].boundary_hash = hashOpenBC(mesh,i);

    //...Hash the land boundary conditions
    for(i=0;i<mesh.NumLandBoundaries;i++)
        mesh.landBoundaryH[i].boundary_hash = hashLandBC(mesh,i);

    return ERR_NOERR;
}

QString adcirc_hashlib::hashMesh(adcirc_mesh &mesh)
{
    QString hashSeed,hashSeed1,hashSeed2,hashSeed3,hashSeed4;
    QString elevation,supercritical,subcritical,pipeht,pipediam,pipecoef;
    int i,j;

    //...Compute the full mesh hash by iterating over all the data
    //   contained
    QCryptographicHash fullHash(QCryptographicHash::Sha1);

    //...The full mesh hash is computed with the z-elevations
    //   so we can easily see if two meshes are different in
    //   any way by checking the hash in the header
    for(i=0;i<mesh.NumNodes;i++)
    {
        hashSeed1.sprintf("%+018.12e",mesh.node[i].x);
        hashSeed2.sprintf("%+018.12e",mesh.node[i].y);
        hashSeed3.sprintf("%+018.12e",mesh.node[i].z);
        hashSeed = hashSeed1+hashSeed2+hashSeed3;
        fullHash.addData(hashSeed.toUtf8(),hashSeed.length());
    }

    for(i=0;i<mesh.NumElements;i++)
    {

        hashSeed1 = mesh.element[i].elementHash;
        hashSeed2 = mesh.element[i].h1;
        hashSeed3 = mesh.element[i].h2;
        hashSeed4 = mesh.element[i].h3;
        hashSeed = hashSeed1+hashSeed2+hashSeed3+hashSeed4;
        fullHash.addData(hashSeed.toUtf8(),hashSeed.length());
    }

    for(i=0;i<mesh.NumOpenBoundaries;i++)
    {
        for(j=0;j<mesh.openBoundaryH[i].NumNodes;j++)
        {
            hashSeed = mesh.openBoundaryH[i].node1_hash[j];
            fullHash.addData(hashSeed.toUtf8(),hashSeed.length());
        }
    }

    for(i=0;i<mesh.NumLandBoundaries;i++)
    {
        if(mesh.landBoundaryH[i].code  == 0   ||
            mesh.landBoundaryH[i].code == 1   ||
            mesh.landBoundaryH[i].code == 2   ||
            mesh.landBoundaryH[i].code == 10  ||
            mesh.landBoundaryH[i].code == 11  ||
            mesh.landBoundaryH[i].code == 12  ||
            mesh.landBoundaryH[i].code == 20  ||
            mesh.landBoundaryH[i].code == 21  ||
            mesh.landBoundaryH[i].code == 22  ||
            mesh.landBoundaryH[i].code == 30  ||
            mesh.landBoundaryH[i].code == 52  ||
            mesh.landBoundaryH[i].code == 102 ||
            mesh.landBoundaryH[i].code == 112 ||
            mesh.landBoundaryH[i].code == 122 )
        {
            for(j=0;j<mesh.landBoundaryH[i].NumNodes;j++)
            {
                hashSeed = mesh.landBoundaryH[i].node1_hash[j];
                fullHash.addData(hashSeed.toUtf8(),hashSeed.length());
            }
        }
        else if(mesh.landBoundaryH[i].code == 3  ||
                mesh.landBoundaryH[i].code == 13 ||
                mesh.landBoundaryH[i].code == 23 )
        {

            for(j=0;j<mesh.landBoundaryH[i].NumNodes;j++)
            {
                elevation.sprintf("%+18.12e",mesh.landBoundaryH[i].elevation[j]);
                supercritical.sprintf("%+18.12e",mesh.landBoundaryH[i].supercritical[j]);
                hashSeed = mesh.landBoundaryH[i].node1_hash[j]+
                             elevation+supercritical;
                fullHash.addData(hashSeed.toUtf8(),hashSeed.length());
            }
        }
        else if(mesh.landBoundaryH[i].code == 4  ||
                mesh.landBoundaryH[i].code == 24 )
        {
            for(j=0;j<mesh.landBoundaryH[i].NumNodes;j++)
            {
                elevation.sprintf("%+18.12e",mesh.landBoundaryH[i].elevation[j]);
                supercritical.sprintf("%+18.12e",mesh.landBoundaryH[i].supercritical[j]);
                subcritical.sprintf("%+18.12e",mesh.landBoundaryH[i].subcritical[j]);
                hashSeed = mesh.landBoundaryH[i].node1_hash[j]+
                             mesh.landBoundaryH[i].node2_hash[j]+
                             elevation+subcritical+supercritical;
                fullHash.addData(hashSeed.toUtf8(),hashSeed.length());
            }
        }
        else if(mesh.landBoundaryH[i].code == 5  ||
                mesh.landBoundaryH[i].code == 25 )
        {
            for(j=0;j<mesh.landBoundaryH[i].NumNodes;j++)
            {
                elevation.sprintf("%+18.12e",mesh.landBoundaryH[i].elevation[j]);
                supercritical.sprintf("%+18.12e",mesh.landBoundaryH[i].supercritical[j]);
                subcritical.sprintf("%+18.12e",mesh.landBoundaryH[i].subcritical[j]);
                pipeht.sprintf("%+18.12e",mesh.landBoundaryH[i].pipe_ht[j]);
                pipecoef.sprintf("%+18.12e",mesh.landBoundaryH[i].pipe_coef[j]);
                pipediam.sprintf("%+18.12e",mesh.landBoundaryH[i].pipe_diam[j]);
                hashSeed = mesh.landBoundaryH[i].node1_hash[j]+
                             mesh.landBoundaryH[i].node2_hash[j]+
                             elevation+subcritical+supercritical+pipeht+
                             pipecoef+pipediam;
                fullHash.addData(hashSeed.toUtf8(),hashSeed.length());
            }
        }
        else
        {
            for(j=0;j<mesh.landBoundaryH[i].NumNodes;j++)
            {
                hashSeed = mesh.landBoundaryH[i].node1_hash[j];
                fullHash.addData(hashSeed.toUtf8(),hashSeed.length());
            }
        }
    }
    return fullHash.result().toHex();
}
