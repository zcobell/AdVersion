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
#ifdef GUI
int adcirc_hashlib::createAdcircHashes(adcirc_mesh &myMesh, QProgressDialog &dialog, int &counter)
#else
int adcirc_hashlib::createAdcircHashes(adcirc_mesh &myMesh)
#endif
{

    int i;

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
