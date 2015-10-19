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


//------------------------------------------------------------------------------
//...Comparison operator used in the sorting of nodes
//------------------------------------------------------------------------------
bool operator< (const adcirc_node &first, const adcirc_node &second)
{
    if(first.locationHash<second.locationHash)
        return true;
    else
        return false;
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//...Comparison operator used in the sorting of elements
bool operator< (const adcirc_element &first, const adcirc_element &second)
{
    if(first.elementHash<second.elementHash)
        return true;
    else
        return false;
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//...Comparison operator used in the sorting of boundaries
//------------------------------------------------------------------------------
bool operator< (const adcirc_boundary_hash &first, const adcirc_boundary_hash &second)
{
    if(first.boundary_hash<second.boundary_hash)
        return true;
    else
        return false;
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//...Comparison operator used in the sorting of boundaries
//------------------------------------------------------------------------------
bool operator< (const adcirc_boundary &first, const adcirc_boundary &second)
{
    if(first.averageLongitude<second.averageLongitude)
        return false;
    else
        return true;
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//...Create the ADCIRC mesh hashes
//------------------------------------------------------------------------------
#ifdef GUI
int adcirc_hashlib::createAdcircHashes(adcirc_mesh &myMesh, QProgressDialog &dialog, int &counter)
#else
int adcirc_hashlib::createAdcircHashes(adcirc_mesh &myMesh)
#endif
{

    //...variables
    int i,j;
    QString hashSeed;

#ifdef GUI
    dialog.setLabelText("Hashing the ADCIRC mesh...");
#endif

    //...loop over nodes to generate a unique hash
    //   for each node
    for(i=0;i<myMesh.NumNodes;i++)
    {
        //...Hash the node
        myMesh.node[i].locationHash = adcirc_hashlib::hashNode(myMesh.node[i]);

#ifdef GUI
        //...Update the progress bar
        updateProgress(counter,dialog);

        //...Catch the cancelled signal
        if(dialog.wasCanceled())
            return ERR_CANCELED;
#endif

    }

    //...Now create the hash for each element which is based upon the
    //   locations of each hash
    for(i=0;i<myMesh.NumElements;i++)
    {
        //...Save the local hash for this node into the array
        myMesh.element[i].elementHash = hashElement(myMesh,i);

        //...Check for errors
        if(myMesh.element[i].elementHash == "ERROR")
            return ERR_UNKNOWN;

#ifdef GUI
        //...Update the progress bar
        updateProgress(counter,dialog);

        //...Catch the cancelled signal
        if(dialog.wasCanceled())
            return ERR_CANCELED;
#endif

    }

    //...Hash the open boundary conditions
    for(i=0;i<myMesh.NumOpenBoundaries;i++)
        myMesh.openBoundaryH[i].boundary_hash = hashOpenBC(myMesh,i);

    //...Hash the land boundary conditions
    for(i=0;i<myMesh.NumLandBoundaries;i++)
        myMesh.landBoundaryH[i].boundary_hash = hashLandBC(myMesh,i);

    return ERR_NOERR;
}

#ifdef GUI
int adcirc_hashlib::sortAdcircHashes(adcirc_mesh &myMesh, QProgressDialog &dialog, int &counter)
#else
int adcirc_hashlib::sortAdcircHashes(adcirc_mesh &myMesh)
#endif
{
    int i,j;

#ifdef GUI
    dialog.setLabelText("Sorting the hashes...");
#endif

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

#ifdef GUI
        //...Update the progress bar
        updateProgress(counter,dialog);

        //...Catch the cancelled signal
        if(dialog.wasCanceled())
            return ERR_CANCELED;
#endif
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


#ifdef GUI
int adcirc_hashlib::numberAdcircMesh(adcirc_mesh &myMesh, QProgressDialog &dialog, int &counter)
#else
int adcirc_hashlib::numberAdcircMesh(adcirc_mesh &myMesh)
#endif
{
    QMap<QString,int> mapping_s2a;
    int i,j;
    double a;

#ifdef GUI
    dialog.setLabelText("Numbering the ADCIRC mesh...");
#endif

    //...Create a mapping table
    for(i=0;i<myMesh.NumNodes;i++)
    {
        mapping_s2a[myMesh.node[i].locationHash] = i;

#ifdef GUI
        //...Update the progress bar
        updateProgress(counter,dialog);

        //...Catch the cancelled signal
        if(dialog.wasCanceled())
            return ERR_CANCELED;
#endif
    }

    //...Generate the element table
    for(i=0;i<myMesh.NumElements;i++)
    {
        myMesh.element[i].c1 = mapping_s2a[myMesh.element[i].h1];
        myMesh.element[i].c2 = mapping_s2a[myMesh.element[i].h2];
        myMesh.element[i].c3 = mapping_s2a[myMesh.element[i].h3];

#ifdef GUI
        //...Update the progress bar
        updateProgress(counter,dialog);

        //...Catch the cancelled signal
        if(dialog.wasCanceled())
            return ERR_CANCELED;
#endif
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
