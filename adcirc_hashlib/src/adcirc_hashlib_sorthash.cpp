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
//  File: adcirc_sorthash.cpp
//
//------------------------------------------------------------------------------

#include "adcirc_hashlib.h"

int adcirc_hashlib::sortAdcircHashes(adcirc_mesh &myMesh)
{
    int i,j;

    //...Sort the nodes, but don't transfer yet
    QVector<adcirc_node> nodeList;
    nodeList.resize(myMesh.NumNodes);
    nodeList = myMesh.node;
    qSort(nodeList);

    //...Sort the elements
    QVector<adcirc_element> elementList;
    elementList.resize(myMesh.NumElements);
    elementList = myMesh.element;
    qSort(elementList);
    myMesh.element = elementList;
    elementList.clear();

    //...Hash the connectivity table
    for(int i=0;i<myMesh.NumElements;i++)
    {
        myMesh.element[i].h1 = myMesh.node[myMesh.element[i].c1-1].locationHash;
        myMesh.element[i].h2 = myMesh.node[myMesh.element[i].c2-1].locationHash;
        myMesh.element[i].h3 = myMesh.node[myMesh.element[i].c3-1].locationHash;
    }

    //...Sort the open boundary segments
    QVector<adcirc_boundary_hash> openboundaryList;
    openboundaryList.resize(myMesh.NumOpenBoundaries);
    openboundaryList = myMesh.openBoundaryH;
    qSort(openboundaryList);

    //...Clear the information from the mesh about open boundaries
    for(i=0;i<myMesh.NumOpenBoundaries;i++)
        myMesh.openBoundaryH[i].node1.clear();
    myMesh.openBoundaryH.clear();

    //...Save the sorted information
    myMesh.openBoundaryH = openboundaryList;

    //...Hash the open boundary array
    for(i=0;i<myMesh.NumOpenBoundaries;i++)
    {
        myMesh.openBoundaryH[i].node1_hash.resize(myMesh.openBoundaryH[i].NumNodes);
        for(j=0;j<myMesh.openBoundaryH[i].NumNodes;j++)
            myMesh.openBoundaryH[i].node1_hash[j] =
                    myMesh.node[myMesh.openBoundaryH[i].node1[j]-1].locationHash;
    }

    //...Sort the land boundary segments
    QVector<adcirc_boundary_hash> landBoundaryList;
    landBoundaryList.resize(myMesh.NumLandBoundaries);
    landBoundaryList = myMesh.landBoundaryH;
    qSort(landBoundaryList);

    //...Clear the boundary arrays
    for(i=0;i<myMesh.NumLandBoundaries;i++)
    {
        myMesh.landBoundaryH[i].node1.clear();
        myMesh.landBoundaryH[i].node2.clear();
        myMesh.landBoundaryH[i].elevation.clear();
        myMesh.landBoundaryH[i].subcritical.clear();
        myMesh.landBoundaryH[i].supercritical.clear();
        myMesh.landBoundaryH[i].pipe_ht.clear();
        myMesh.landBoundaryH[i].pipe_coef.clear();
        myMesh.landBoundaryH[i].pipe_diam.clear();
    }

    //...Transfer the new boundary array
    myMesh.landBoundaryH = landBoundaryList;
    landBoundaryList.clear();

    //Accumulate a hash along the boundary string
    for(i=0;i<myMesh.NumLandBoundaries;i++)
    {
        if(myMesh.landBoundaryH[i].code  == 0   ||
            myMesh.landBoundaryH[i].code == 1   ||
            myMesh.landBoundaryH[i].code == 2   ||
            myMesh.landBoundaryH[i].code == 10  ||
            myMesh.landBoundaryH[i].code == 11  ||
            myMesh.landBoundaryH[i].code == 12  ||
            myMesh.landBoundaryH[i].code == 20  ||
            myMesh.landBoundaryH[i].code == 21  ||
            myMesh.landBoundaryH[i].code == 22  ||
            myMesh.landBoundaryH[i].code == 30  ||
            myMesh.landBoundaryH[i].code == 52  ||
            myMesh.landBoundaryH[i].code == 102 ||
            myMesh.landBoundaryH[i].code == 112 ||
            myMesh.landBoundaryH[i].code == 122 ||
            myMesh.landBoundaryH[i].code == 3   ||
            myMesh.landBoundaryH[i].code == 13  ||
            myMesh.landBoundaryH[i].code == 23 )
        {
            myMesh.landBoundaryH[i].node1_hash.resize(myMesh.landBoundaryH[i].NumNodes);
            for(j=0;j<myMesh.landBoundaryH[i].NumNodes;j++)
                myMesh.landBoundaryH[i].node1_hash[j] =
                        myMesh.node[myMesh.landBoundaryH[i].node1[j]-1].locationHash;
        }
        else if(myMesh.landBoundaryH[i].code == 4  ||
                myMesh.landBoundaryH[i].code == 24 ||
                myMesh.landBoundaryH[i].code == 5  ||
                myMesh.landBoundaryH[i].code == 25 )
        {
            myMesh.landBoundaryH[i].node1_hash.resize(myMesh.landBoundaryH[i].NumNodes);
            myMesh.landBoundaryH[i].node2_hash.resize(myMesh.landBoundaryH[i].NumNodes);
            for(j=0;j<myMesh.landBoundaryH[i].NumNodes;j++)
            {
                myMesh.landBoundaryH[i].node1_hash[j] =
                        myMesh.node[myMesh.landBoundaryH[i].node1[j]-1].locationHash;
                myMesh.landBoundaryH[i].node2_hash[j] =
                        myMesh.node[myMesh.landBoundaryH[i].node2[j]-1].locationHash;
            }
        }
        else
        {
            myMesh.landBoundaryH[i].node1_hash.resize(myMesh.landBoundaryH[i].NumNodes);
            for(j=0;j<myMesh.landBoundaryH[i].NumNodes;j++)
                myMesh.landBoundaryH[i].node1_hash[j] =
                        myMesh.node[myMesh.landBoundaryH[i].node1[j]-1].locationHash;
        }
    }

    //...Transfer the sorted node list
    myMesh.node = nodeList;
    nodeList.clear();

    return ERR_NOERR;
}
