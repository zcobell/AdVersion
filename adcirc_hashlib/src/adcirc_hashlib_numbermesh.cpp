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
//  File: adcirc_numbermesh.cpp
//
//------------------------------------------------------------------------------

#include "adcirc_hashlib.h"

int adcirc_hashlib::numberAdcircMesh(adcirc_mesh &myMesh)
{
    QMap<QString,int> mapping_s2a;
    int i,j;
    double a;

    //...Create a mapping table
    for(i=0;i<myMesh.NumNodes;i++)
        mapping_s2a[myMesh.node[i].locationHash] = i;

    //...Generate the element table
    for(i=0;i<myMesh.NumElements;i++)
    {
        myMesh.element[i].c1 = mapping_s2a[myMesh.element[i].h1];
        myMesh.element[i].c2 = mapping_s2a[myMesh.element[i].h2];
        myMesh.element[i].c3 = mapping_s2a[myMesh.element[i].h3];
    }

    //...Map the open boundary arrays
    for(i=0;i<myMesh.NumOpenBoundaries;i++)
    {
        for(j=0;j<myMesh.openBoundary[i].NumNodes;j++)
            myMesh.openBoundary[i].node1[j] = mapping_s2a[myMesh.openBoundary[i].node1_hash[j]];
    }

    //...Map the land boundary array
    for(i=0;i<myMesh.NumLandBoundaries;i++)
    {
        if(myMesh.landBoundary[i].code  == 0   ||
            myMesh.landBoundary[i].code == 1   ||
            myMesh.landBoundary[i].code == 2   ||
            myMesh.landBoundary[i].code == 10  ||
            myMesh.landBoundary[i].code == 11  ||
            myMesh.landBoundary[i].code == 12  ||
            myMesh.landBoundary[i].code == 20  ||
            myMesh.landBoundary[i].code == 21  ||
            myMesh.landBoundary[i].code == 22  ||
            myMesh.landBoundary[i].code == 30  ||
            myMesh.landBoundary[i].code == 52  ||
            myMesh.landBoundary[i].code == 102 ||
            myMesh.landBoundary[i].code == 112 ||
            myMesh.landBoundary[i].code == 122 ||
            myMesh.landBoundary[i].code == 3   ||
            myMesh.landBoundary[i].code == 13  ||
            myMesh.landBoundary[i].code == 23 )
        {
            for(j=0;j<myMesh.landBoundary[i].NumNodes;j++)
                myMesh.landBoundary[i].node1[j] = mapping_s2a[myMesh.landBoundary[i].node1_hash[j]];
        }
        else if(myMesh.landBoundary[i].code == 4  ||
                myMesh.landBoundary[i].code == 24 ||
                myMesh.landBoundary[i].code == 5  ||
                myMesh.landBoundary[i].code == 25 )
        {
            for(j=0;j<myMesh.landBoundary[i].NumNodes;j++)
            {
                myMesh.landBoundary[i].node1[j] = mapping_s2a[myMesh.landBoundary[i].node1_hash[j]];
                myMesh.landBoundary[i].node2[j] = mapping_s2a[myMesh.landBoundary[i].node2_hash[j]];
            }
        }
        else
        {
            for(j=0;j<myMesh.landBoundary[i].NumNodes;j++)
                myMesh.landBoundary[i].node1[j] = mapping_s2a[myMesh.landBoundary[i].node1_hash[j]];
        }
    }

    //...Now put the boundaries in East --> West order
    QVector<adcirc_boundary> openBoundary;
    QVector<adcirc_boundary> landBoundary;
    openBoundary = myMesh.openBoundary;
    landBoundary = myMesh.landBoundary;

    for(i=0;i<myMesh.NumOpenBoundaries;i++)
    {
        a = 0.0;
        for(j=0;j<openBoundary[i].NumNodes;j++)
            a += myMesh.node[openBoundary[i].node1[j]].x;
        a = a / openBoundary[i].NumNodes;
        openBoundary[i].averageLongitude = a;
    }

    for(i=0;i<myMesh.NumLandBoundaries;i++)
    {
        a = 0.0;
        for(j=0;j<landBoundary[i].NumNodes;j++)
            a += myMesh.node[landBoundary[i].node1[j]].x;
        a = a / landBoundary[i].NumNodes;
        landBoundary[i].averageLongitude = a;
    }

    qSort(openBoundary);
    qSort(landBoundary);

    for(i=0;i<myMesh.NumOpenBoundaries;i++)
        myMesh.openBoundary[i].node1.clear();
    myMesh.openBoundary.clear();

    for(i=0;i<myMesh.NumLandBoundaries;i++)
    {
        myMesh.landBoundary[i].node1.clear();
        myMesh.landBoundary[i].node2.clear();
        myMesh.landBoundary[i].elevation.clear();
        myMesh.landBoundary[i].subcritical.clear();
        myMesh.landBoundary[i].supercritical.clear();
        myMesh.landBoundary[i].pipe_ht.clear();
        myMesh.landBoundary[i].pipe_coef.clear();
        myMesh.landBoundary[i].pipe_diam.clear();
    }
    myMesh.landBoundary.clear();

    myMesh.openBoundary = openBoundary;
    myMesh.landBoundary = landBoundary;

    return ERR_NOERR;
}
