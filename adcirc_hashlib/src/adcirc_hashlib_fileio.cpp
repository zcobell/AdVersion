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
//  File: adcirc_hashlib_fileio.cpp
//
//------------------------------------------------------------------------------

#include "adcirc_hashlib.h"

//------------------------------------------------------------------------------
//...Read an ADCIRC mesh
//------------------------------------------------------------------------------
#ifdef GUI
int adcirc_hashlib::readAdcircMesh(QString fileName, adcirc_mesh &myMesh, QProgressDialog &dialog, int &counter)
#else
int adcirc_hashlib::readAdcircMesh(QString fileName, adcirc_mesh &myMesh)
#endif
{
    //Variables
    int i,j;

    QString readData;
    QStringList readDataList;

    QFile meshFile(fileName);

#ifdef GUI
    dialog.setLabelText("Reading the ADCIRC mesh...");
    dialog.setValue(0);
#endif

    //----------------------------------------------------//

    //Check if we can open the file
    if(!meshFile.open(QIODevice::ReadOnly|QIODevice::Text))
    {
        myMesh.status = -1;
        return ERR_NOFILE;
    }

    //Read the header line
    myMesh.header = meshFile.readLine().simplified();

    //Read the number of nodes/elements
    readData = meshFile.readLine().simplified();
    readDataList = readData.split(" ");
    readData = readDataList.value(1);
    myMesh.NumNodes = readData.toInt();
    readData = readDataList.value(0);
    myMesh.NumElements = readData.toInt();

#ifdef GUI
    dialog.setMaximum(myMesh.NumNodes*4+myMesh.NumElements*5);
#endif

    //Begin sizing the arrays
    myMesh.node.resize(myMesh.NumNodes);
    myMesh.element.resize(myMesh.NumElements);

    //Begin reading the nodes
    for(i = 0; i < myMesh.NumNodes; i++)
    {
        readData = meshFile.readLine().simplified();
        readDataList = readData.split(" ");
        readData = readDataList.value(1);
        myMesh.node[i].x = readData.toDouble();
        readData = readDataList.value(2);
        myMesh.node[i].y = readData.toDouble();
        readData = readDataList.value(3);
        myMesh.node[i].z = readData.toDouble();

#ifdef GUI
        //...Update the progress bar
        updateProgress(counter,dialog);

        //...Catch the cancelled signal
        if(dialog.wasCanceled())
            return ERR_CANCELED;
#endif

    }


    //Begin reading the elements
    for(i = 0; i< myMesh.NumElements; i++)
    {
        readData = meshFile.readLine().simplified();
        readDataList = readData.split(" ");
        readData = readDataList.value(2);
        myMesh.element[i].c1 = readData.toInt();
        readData = readDataList.value(3);
        myMesh.element[i].c2 = readData.toInt();
        readData = readDataList.value(4);
        myMesh.element[i].c3 = readData.toInt();

#ifdef GUI
        //...Update the progress bar
        updateProgress(counter,dialog);

        //...Catch the cancelled signal
        if(dialog.wasCanceled())
            return ERR_CANCELED;
#endif
    }

    //Begin reading the boundaries

    //Open Boundaries
    readData = meshFile.readLine().simplified();
    readDataList = readData.split(" ");
    readData = readDataList.value(0);
    myMesh.NumOpenBoundaries = readData.toInt();
    readData = meshFile.readLine().simplified();
    readDataList = readData.split(" ");
    readData = readDataList.value(0);
    myMesh.NumOpenBoundaryNodes = readData.toInt();
    myMesh.openBoundaryH.resize(myMesh.NumOpenBoundaries);
    for(i=0;i<myMesh.NumOpenBoundaries;i++)
    {
        readData = meshFile.readLine().simplified();
        readDataList = readData.split(" ");
        readData = readDataList.value(0);
        myMesh.openBoundaryH[i].NumNodes = readData.toInt();
        myMesh.openBoundaryH[i].code = -1;
        myMesh.openBoundaryH[i].node1.resize(myMesh.openBoundaryH[i].NumNodes);
        for(j=0;j<myMesh.openBoundaryH[i].NumNodes;j++)
        {
            readData = meshFile.readLine().simplified();
            myMesh.openBoundaryH[i].node1[j] = readData.toInt();
        }
    }

    //Land Boundaries
    readData = meshFile.readLine().simplified();
    readDataList = readData.split(" ");
    readData = readDataList.value(0);
    myMesh.NumLandBoundaries = readData.toInt();
    readData = meshFile.readLine().simplified();
    readDataList = readData.split(" ");
    readData = readDataList.value(0);
    myMesh.NumLandBoundaryNodes = readData.toInt();
    myMesh.landBoundaryH.resize(myMesh.NumLandBoundaries);

    for(i=0;i<myMesh.NumLandBoundaries;i++)
    {
        readData = meshFile.readLine().simplified();
        readDataList = readData.split(" ");
        readData = readDataList.value(0);
        myMesh.landBoundaryH[i].NumNodes = readData.toInt();
        readData = readDataList.value(1);
        myMesh.landBoundaryH[i].code = readData.toInt();

        //Read the boundary string appropriately
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
            myMesh.landBoundaryH[i].code == 122 )
        {
            myMesh.landBoundaryH[i].node1.resize(myMesh.landBoundaryH[i].NumNodes);
            for(j=0;j<myMesh.landBoundaryH[i].NumNodes;j++)
            {
                readData = meshFile.readLine().simplified();
                myMesh.landBoundaryH[i].node1[j] = readData.toInt();
            }
        }
        else if(myMesh.landBoundaryH[i].code == 3  ||
                myMesh.landBoundaryH[i].code == 13 ||
                myMesh.landBoundaryH[i].code == 23 )
        {
            myMesh.landBoundaryH[i].node1.resize(myMesh.landBoundaryH[i].NumNodes);
            myMesh.landBoundaryH[i].elevation.resize(myMesh.landBoundaryH[i].NumNodes);
            myMesh.landBoundaryH[i].supercritical.resize(myMesh.landBoundaryH[i].NumNodes);
            for(j=0;j<myMesh.landBoundaryH[i].NumNodes;j++)
            {
                readData = meshFile.readLine().simplified();
                readDataList = readData.split(" ");
                readData = readDataList.value(0);
                myMesh.landBoundaryH[i].node1[j] = readData.toInt();
                readData = readDataList.value(1);
                myMesh.landBoundaryH[i].elevation[j] = readData.toDouble();
                readData = readDataList.value(2);
                myMesh.landBoundaryH[i].supercritical[j] = readData.toDouble();
            }
        }
        else if(myMesh.landBoundaryH[i].code == 4  ||
                myMesh.landBoundaryH[i].code == 24 )
        {
            myMesh.landBoundaryH[i].node1.resize(myMesh.landBoundaryH[i].NumNodes);
            myMesh.landBoundaryH[i].node2.resize(myMesh.landBoundaryH[i].NumNodes);
            myMesh.landBoundaryH[i].elevation.resize(myMesh.landBoundaryH[i].NumNodes);
            myMesh.landBoundaryH[i].supercritical.resize(myMesh.landBoundaryH[i].NumNodes);
            myMesh.landBoundaryH[i].subcritical.resize(myMesh.landBoundaryH[i].NumNodes);
            for(j=0;j<myMesh.landBoundaryH[i].NumNodes;j++)
            {
                readData = meshFile.readLine().simplified();
                readDataList = readData.split(" ");
                readData = readDataList.value(0);
                myMesh.landBoundaryH[i].node1[j] = readData.toInt();
                readData = readDataList.value(1);
                myMesh.landBoundaryH[i].node2[j] = readData.toInt();
                readData = readDataList.value(2);
                myMesh.landBoundaryH[i].elevation[j] = readData.toDouble();
                readData = readDataList.value(3);
                myMesh.landBoundaryH[i].subcritical[j] = readData.toDouble();
                readData = readDataList.value(4);
                myMesh.landBoundaryH[i].supercritical[j] = readData.toDouble();
            }
        }
        else if(myMesh.landBoundaryH[i].code == 5  ||
                myMesh.landBoundaryH[i].code == 25 )
        {
            myMesh.landBoundaryH[i].node1.resize(myMesh.landBoundaryH[i].NumNodes);
            myMesh.landBoundaryH[i].node2.resize(myMesh.landBoundaryH[i].NumNodes);
            myMesh.landBoundaryH[i].elevation.resize(myMesh.landBoundaryH[i].NumNodes);
            myMesh.landBoundaryH[i].supercritical.resize(myMesh.landBoundaryH[i].NumNodes);
            myMesh.landBoundaryH[i].subcritical.resize(myMesh.landBoundaryH[i].NumNodes);
            myMesh.landBoundaryH[i].pipe_ht.resize(myMesh.landBoundaryH[i].NumNodes);
            myMesh.landBoundaryH[i].pipe_coef.resize(myMesh.landBoundaryH[i].NumNodes);
            myMesh.landBoundaryH[i].pipe_diam.resize(myMesh.landBoundaryH[i].NumNodes);
            for(j=0;j<myMesh.landBoundaryH[i].NumNodes;j++)
            {
                readData = meshFile.readLine().simplified();
                readDataList = readData.split(" ");
                readData = readDataList.value(0);
                myMesh.landBoundaryH[i].node1[j] = readData.toInt();
                readData = readDataList.value(1);
                myMesh.landBoundaryH[i].node2[j] = readData.toInt();
                readData = readDataList.value(2);
                myMesh.landBoundaryH[i].elevation[j] = readData.toDouble();
                readData = readDataList.value(3);
                myMesh.landBoundaryH[i].subcritical[j] = readData.toDouble();
                readData = readDataList.value(4);
                myMesh.landBoundaryH[i].supercritical[j] = readData.toDouble();
                readData = readDataList.value(5);
                myMesh.landBoundaryH[i].pipe_ht[j] = readData.toDouble();
                readData = readDataList.value(6);
                myMesh.landBoundaryH[i].pipe_coef[j] = readData.toDouble();
                readData = readDataList.value(7);
                myMesh.landBoundaryH[i].pipe_diam[j] = readData.toDouble();
            }
        }
        else
        {
            myMesh.landBoundaryH[i].node1.resize(myMesh.landBoundaryH[i].NumNodes);
            for(j=0;j<myMesh.landBoundaryH[i].NumNodes;j++)
            {
                readData = meshFile.readLine().simplified();
                myMesh.landBoundaryH[i].node1[j] = readData.toInt();
            }
        }
    }

    meshFile.close();

    return ERR_NOERR;
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//...Write the adcirc mesh using the SHA1 hashes
//------------------------------------------------------------------------------
#ifdef GUI
int adcirc_hashlib::writeAdcircHashMesh(QString fileName, adcirc_mesh &myMesh, QProgressDialog &dialog, int &counter)
#else
int adcirc_hashlib::writeAdcircHashMesh(QString fileName, adcirc_mesh &myMesh)
#endif
{
    QString line,tempString;
    QString elevation,supercritical,subcritical,pipeht,pipediam,pipecoef;
    QString hashSeed,hashSeed1,hashSeed2,hashSeed3,hashSeed4;
    int i,j;

    QFile outputFile(fileName);
    QTextStream output(&outputFile);
    outputFile.open(QIODevice::WriteOnly);

#ifdef GUI
    dialog.setLabelText("Writing the hashed ADCIRC mesh...");
#endif

    //...Compute the full mesh hash by iterating over all the data
    //   contained
    QCryptographicHash fullHash(QCryptographicHash::Sha1);

    //...The full mesh hash is computed with the z-elevations
    //   so we can easily see if two meshes are different in
    //   any way by checking the hash in the header
    for(i=0;i<myMesh.NumNodes;i++)
    {
        hashSeed1.sprintf("%+018.12e",myMesh.node[i].x);
        hashSeed2.sprintf("%+018.12e",myMesh.node[i].y);
        hashSeed3.sprintf("%+018.12e",myMesh.node[i].z);
        hashSeed = hashSeed1+hashSeed2+hashSeed3;
        fullHash.addData(hashSeed.toUtf8(),hashSeed.length());

#ifdef GUI
        //...Update the progress bar
        updateProgress(counter,dialog);

        //...Catch the cancelled signal
        if(dialog.wasCanceled())
            return ERR_CANCELED;
#endif
    }

    for(i=0;i<myMesh.NumElements;i++)
    {
        hashSeed1 = myMesh.element[i].elementHash;
        hashSeed2 = myMesh.element[i].h1;
        hashSeed3 = myMesh.element[i].h2;
        hashSeed4 = myMesh.element[i].h3;
        hashSeed = hashSeed1+hashSeed2+hashSeed3+hashSeed4;
        fullHash.addData(hashSeed.toUtf8(),hashSeed.length());

#ifdef GUI
        //...Update the progress bar
        updateProgress(counter,dialog);

        //...Catch the cancelled signal
        if(dialog.wasCanceled())
            return ERR_CANCELED;
#endif
    }

    for(i=0;i<myMesh.NumOpenBoundaries;i++)
    {
        for(j=0;j<myMesh.openBoundaryH[i].NumNodes;j++)
        {
            hashSeed = myMesh.openBoundaryH[i].node1_hash[j];
            fullHash.addData(hashSeed.toUtf8(),hashSeed.length());
        }
    }

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
            myMesh.landBoundaryH[i].code == 122 )
        {
            for(j=0;j<myMesh.landBoundaryH[i].NumNodes;j++)
            {
                hashSeed = myMesh.landBoundaryH[i].node1_hash[j];
                fullHash.addData(hashSeed.toUtf8(),hashSeed.length());
            }
        }
        else if(myMesh.landBoundaryH[i].code == 3  ||
                myMesh.landBoundaryH[i].code == 13 ||
                myMesh.landBoundaryH[i].code == 23 )
        {

            for(j=0;j<myMesh.landBoundaryH[i].NumNodes;j++)
            {
                elevation.sprintf("%+18.12e",myMesh.landBoundaryH[i].elevation[j]);
                supercritical.sprintf("%+18.12e",myMesh.landBoundaryH[i].supercritical[j]);
                hashSeed = myMesh.landBoundaryH[i].node1_hash[j]+
                             elevation+supercritical;
                fullHash.addData(hashSeed.toUtf8(),hashSeed.length());
            }
        }
        else if(myMesh.landBoundaryH[i].code == 4  ||
                myMesh.landBoundaryH[i].code == 24 )
        {
            for(j=0;j<myMesh.landBoundaryH[i].NumNodes;j++)
            {
                elevation.sprintf("%+18.12e",myMesh.landBoundaryH[i].elevation[j]);
                supercritical.sprintf("%+18.12e",myMesh.landBoundaryH[i].supercritical[j]);
                subcritical.sprintf("%+18.12e",myMesh.landBoundaryH[i].subcritical[j]);
                hashSeed = myMesh.landBoundaryH[i].node1_hash[j]+
                             myMesh.landBoundaryH[i].node2_hash[j]+
                             elevation+subcritical+supercritical;
                fullHash.addData(hashSeed.toUtf8(),hashSeed.length());
            }
        }
        else if(myMesh.landBoundaryH[i].code == 5  ||
                myMesh.landBoundaryH[i].code == 25 )
        {
            for(j=0;j<myMesh.landBoundaryH[i].NumNodes;j++)
            {
                elevation.sprintf("%+18.12e",myMesh.landBoundaryH[i].elevation[j]);
                supercritical.sprintf("%+18.12e",myMesh.landBoundaryH[i].supercritical[j]);
                subcritical.sprintf("%+18.12e",myMesh.landBoundaryH[i].subcritical[j]);
                pipeht.sprintf("%+18.12e",myMesh.landBoundaryH[i].pipe_ht[j]);
                pipecoef.sprintf("%+18.12e",myMesh.landBoundaryH[i].pipe_coef[j]);
                pipediam.sprintf("%+18.12e",myMesh.landBoundaryH[i].pipe_diam[j]);
                hashSeed = myMesh.landBoundaryH[i].node1_hash[j]+
                             myMesh.landBoundaryH[i].node2_hash[j]+
                             elevation+subcritical+supercritical+pipeht+
                             pipecoef+pipediam;
                fullHash.addData(hashSeed.toUtf8(),hashSeed.length());
            }
        }
        else
        {
            for(j=0;j<myMesh.landBoundaryH[i].NumNodes;j++)
            {
                hashSeed = myMesh.landBoundaryH[i].node1_hash[j];
                fullHash.addData(hashSeed.toUtf8(),hashSeed.length());
            }
        }
    }

    myMesh.mesh_hash = fullHash.result().toHex();

    output << myMesh.header << "\n";
    output << myMesh.mesh_hash << "\n";
    output << myMesh.NumElements << " " << myMesh.NumNodes << "\n";

    //...Write the node positions and elevations
    for(i=0;i<myMesh.NumNodes;i++)
    {
        line = "";
        line = myMesh.node[i].locationHash;
        line = line + " " + tempString.sprintf("%+18.12e %+18.12e %+18.12e \n",
                                         myMesh.node[i].x,
                                         myMesh.node[i].y,
                                         myMesh.node[i].z);
        output << line;

#ifdef GUI
        //...Update the progress bar
        updateProgress(counter,dialog);

        //...Catch the cancelled signal
        if(dialog.wasCanceled())
            return ERR_CANCELED;
#endif

    }

    //...Write the element connectivity
    for(i=0;i<myMesh.NumElements;i++)
    {
        line = "";
        line = myMesh.element[i].elementHash;
        line = line + " " +
               myMesh.element[i].h1 + " " +
               myMesh.element[i].h2 + " " +
               myMesh.element[i].h3 + "\n";
        output << line;

#ifdef GUI
        //...Update the progress bar
        updateProgress(counter,dialog);

        //...Catch the cancelled signal
        if(dialog.wasCanceled())
            return ERR_CANCELED;
#endif

    }

    //...Write the open boundaries
    output << myMesh.NumOpenBoundaries << " \n";
    output << myMesh.NumOpenBoundaryNodes << " \n";
    for(i=0;i<myMesh.NumOpenBoundaries;i++)
    {
        output << myMesh.openBoundaryH[i].boundary_hash << "  "
               << myMesh.openBoundaryH[i].NumNodes << " \n";
        for(j=0;j<myMesh.openBoundaryH[i].NumNodes;j++)
            output << myMesh.openBoundaryH[i].node1_hash[j] << " \n";
    }

    //...Write the land boundaries
    output << myMesh.NumLandBoundaries << " \n";
    output << myMesh.NumLandBoundaryNodes << " \n";
    for(i=0;i<myMesh.NumLandBoundaries;i++)
    {
        output << myMesh.landBoundaryH[i].boundary_hash << "  "
               << myMesh.landBoundaryH[i].NumNodes << "   "
               << myMesh.landBoundaryH[i].code << " \n";
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
            myMesh.landBoundaryH[i].code == 122 )
        {
            for(j=0;j<myMesh.landBoundaryH[i].NumNodes;j++)
                output << myMesh.landBoundaryH[i].node1_hash[j] << " \n";
        }
        else if(myMesh.landBoundaryH[i].code == 3  ||
                myMesh.landBoundaryH[i].code == 13 ||
                myMesh.landBoundaryH[i].code == 23 )
        {
            for(j=0;j<myMesh.landBoundaryH[i].NumNodes;j++)
            {
                elevation.sprintf("%+18.12e",myMesh.landBoundaryH[i].elevation[j]);
                supercritical.sprintf("%+18.12e",myMesh.landBoundaryH[i].supercritical[j]);
                output << myMesh.landBoundaryH[i].node1_hash[j] << " "
                       << elevation << " " << supercritical << " \n";
            }
        }
        else if(myMesh.landBoundaryH[i].code == 4  ||
                myMesh.landBoundaryH[i].code == 24 )
        {
            for(j=0;j<myMesh.landBoundaryH[i].NumNodes;j++)
            {
                elevation.sprintf("%+18.12e",myMesh.landBoundaryH[i].elevation[j]);
                supercritical.sprintf("%+18.12e",myMesh.landBoundaryH[i].supercritical[j]);
                subcritical.sprintf("%+18.12e",myMesh.landBoundaryH[i].subcritical[j]);
                output << myMesh.landBoundaryH[i].node1_hash[j] << " "
                       << myMesh.landBoundaryH[i].node2_hash[j] << " "
                       << elevation << " " << subcritical << " " << supercritical << " \n";
            }
        }
        else if(myMesh.landBoundaryH[i].code == 5  ||
                myMesh.landBoundaryH[i].code == 25 )
        {
            for(j=0;j<myMesh.landBoundaryH[i].NumNodes;j++)
            {
                elevation.sprintf("%+18.12e",myMesh.landBoundaryH[i].elevation[j]);
                supercritical.sprintf("%+18.12e",myMesh.landBoundaryH[i].supercritical[j]);
                subcritical.sprintf("%+18.12e",myMesh.landBoundaryH[i].subcritical[j]);
                pipeht.sprintf("%+18.12e",myMesh.landBoundaryH[i].pipe_ht[j]);
                pipecoef.sprintf("%+18.12e",myMesh.landBoundaryH[i].pipe_coef[j]);
                pipediam.sprintf("%+18.12e",myMesh.landBoundaryH[i].pipe_diam[j]);
                output << myMesh.landBoundaryH[i].node1_hash[j] << " " <<
                          myMesh.landBoundaryH[i].node2_hash[j] << " " <<
                          elevation << " " << subcritical << " " << supercritical <<
                          pipeht << " " << pipecoef << " " << pipediam << " \n";
            }
        }
        else
        {
            for(j=0;j<myMesh.openBoundaryH[i].NumNodes;j++)
                output << myMesh.landBoundaryH[i].node1_hash[j] << " \n";
        }
    }

    outputFile.close();

    return ERR_NOERR;
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//...Read an ADCIRC mesh using the SHA1 format
//------------------------------------------------------------------------------
#ifdef GUI
int adcirc_hashlib::readAdcircSha1Mesh(QString fileName, adcirc_mesh &myMesh, QProgressDialog &dialog, int &counter)
#else
int adcirc_hashlib::readAdcircSha1Mesh(QString fileName, adcirc_mesh &myMesh)
#endif
{
    QString tempString;
    QStringList tempList;
    int i,j;

#ifdef GUI
    dialog.setLabelText("Reading the hashed ADCIRC mesh...");
#endif

    QFile meshFile(fileName);
    if(!meshFile.open(QIODevice::ReadOnly|QIODevice::Text))
    {
        myMesh.status = -1;
        return ERR_NOFILE;
    }

    myMesh.header = meshFile.readLine().simplified();
    myMesh.mesh_hash = meshFile.readLine().simplified();
    tempString = meshFile.readLine().simplified();
    tempList = tempString.split(" ");
    tempString = tempList.value(1);
    myMesh.NumNodes = tempString.toInt();
    tempString = tempList.value(0);
    myMesh.NumElements = tempString.toInt();

#ifdef GUI
    dialog.setMaximum((myMesh.NumNodes+myMesh.NumElements)*3);
#endif

    myMesh.node.resize(myMesh.NumNodes);
    myMesh.element.resize(myMesh.NumElements);

    //...Reading the nodes
    for(i=0;i<myMesh.NumNodes;i++)
    {
        tempString = meshFile.readLine().simplified();
        tempList = tempString.split(" ");
        myMesh.node[i].locationHash = tempList.value(0);
        tempString = tempList.value(1);
        myMesh.node[i].x = tempString.toDouble();
        tempString = tempList.value(2);
        myMesh.node[i].y = tempString.toDouble();
        tempString = tempList.value(3);
        myMesh.node[i].z = tempString.toDouble();

#ifdef GUI
        //...Update the progress bar
        updateProgress(counter,dialog);

        //...Catch the cancelled signal
        if(dialog.wasCanceled())
            return ERR_CANCELED;
#endif
    }

    //...Reading the elements
    for(i=0;i<myMesh.NumElements;i++)
    {
        tempString = meshFile.readLine().simplified();
        tempList = tempString.split(" ");
        myMesh.element[i].elementHash = tempList.value(0);
        myMesh.element[i].h1 = tempList.value(1);
        myMesh.element[i].h2 = tempList.value(2);
        myMesh.element[i].h3 = tempList.value(3);

#ifdef GUI
        //...Update the progress bar
        updateProgress(counter,dialog);

        //...Catch the cancelled signal
        if(dialog.wasCanceled())
            return ERR_CANCELED;
#endif
    }

    //...Read the open boundaries
    tempString = meshFile.readLine().simplified();
    tempList = tempString.split(" ");
    tempString = tempList.value(0);
    myMesh.NumOpenBoundaries = tempString.toInt();
    tempString = meshFile.readLine().simplified();
    tempList = tempString.split(" ");
    tempString = tempList.value(0);
    myMesh.NumOpenBoundaryNodes = tempString.toInt();

    myMesh.openBoundary.resize(myMesh.NumOpenBoundaries);
    for(i=0;i<myMesh.NumOpenBoundaries;i++)
    {
        tempString = meshFile.readLine().simplified();
        tempList = tempString.split(" ");
        myMesh.openBoundary[i].boundary_hash = tempList.value(0);
        tempString = tempList.value(1);
        myMesh.openBoundary[i].NumNodes = tempString.toInt();
        myMesh.openBoundary[i].node1.resize(myMesh.openBoundary[i].NumNodes);
        myMesh.openBoundary[i].node1_hash.resize(myMesh.openBoundary[i].NumNodes);
        for(j=0;j<myMesh.openBoundary[i].NumNodes;j++)
        {
            tempString = meshFile.readLine().simplified();
            myMesh.openBoundary[i].node1_hash[j] = tempString;
        }
    }

    //...Read the land boundaries
    tempString = meshFile.readLine().simplified();
    tempList = tempString.split(" ");
    tempString = tempList.value(0);
    myMesh.NumLandBoundaries = tempString.toInt();
    tempString = meshFile.readLine().simplified();
    tempList = tempString.split(" ");
    tempString = tempList.value(0);
    myMesh.NumLandBoundaryNodes = tempString.toInt();

    myMesh.landBoundary.resize(myMesh.NumLandBoundaries);
    for(i=0;i<myMesh.NumLandBoundaries;i++)
    {
        tempString = meshFile.readLine().simplified();
        tempList = tempString.split(" ");
        myMesh.landBoundary[i].boundary_hash = tempList.value(0);
        tempString = tempList.value(1);
        myMesh.landBoundary[i].NumNodes = tempString.toInt();
        tempString = tempList.value(2);
        myMesh.landBoundary[i].code = tempString.toInt();

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
            myMesh.landBoundary[i].code == 122 )
        {
            myMesh.landBoundary[i].node1.resize(myMesh.landBoundary[i].NumNodes);
            myMesh.landBoundary[i].node1_hash.resize(myMesh.landBoundary[i].NumNodes);
            for(j=0;j<myMesh.landBoundary[i].NumNodes;j++)
            {
                tempString = meshFile.readLine().simplified();
                myMesh.landBoundary[i].node1_hash[j] = tempString;
            }
        }
        else if(myMesh.landBoundary[i].code == 3  ||
                myMesh.landBoundary[i].code == 13 ||
                myMesh.landBoundary[i].code == 23 )
        {
            myMesh.landBoundary[i].node1.resize(myMesh.landBoundary[i].NumNodes);
            myMesh.landBoundary[i].node1_hash.resize(myMesh.landBoundary[i].NumNodes);
            myMesh.landBoundary[i].elevation.resize(myMesh.landBoundary[i].NumNodes);
            myMesh.landBoundary[i].supercritical.resize(myMesh.landBoundary[i].NumNodes);
            for(j=0;j<myMesh.landBoundary[i].NumNodes;j++)
            {
                tempString = meshFile.readLine().simplified();
                tempList = tempString.split(" ");
                myMesh.landBoundary[i].node1_hash[j] = tempList.value(0);
                tempString = tempList.value(1);
                myMesh.landBoundary[i].elevation[j] = tempString.toDouble();
                tempString = tempList.value(2);
                myMesh.landBoundary[i].supercritical[j] = tempString.toDouble();
            }
        }
        else if(myMesh.landBoundary[i].code == 4  ||
                myMesh.landBoundary[i].code == 24 )
        {
            myMesh.landBoundary[i].node1.resize(myMesh.landBoundary[i].NumNodes);
            myMesh.landBoundary[i].node1_hash.resize(myMesh.landBoundary[i].NumNodes);
            myMesh.landBoundary[i].node2.resize(myMesh.landBoundary[i].NumNodes);
            myMesh.landBoundary[i].node2_hash.resize(myMesh.landBoundary[i].NumNodes);
            myMesh.landBoundary[i].elevation.resize(myMesh.landBoundary[i].NumNodes);
            myMesh.landBoundary[i].supercritical.resize(myMesh.landBoundary[i].NumNodes);
            myMesh.landBoundary[i].subcritical.resize(myMesh.landBoundary[i].NumNodes);
            for(j=0;j<myMesh.landBoundary[i].NumNodes;j++)
            {
                tempString = meshFile.readLine().simplified();
                tempList = tempString.split(" ");
                myMesh.landBoundary[i].node1_hash[j] = tempList.value(0);
                myMesh.landBoundary[i].node2_hash[j] = tempList.value(1);
                tempString = tempList.value(2);
                myMesh.landBoundary[i].elevation[j] = tempString.toDouble();
                tempString = tempList.value(3);
                myMesh.landBoundary[i].subcritical[j] = tempString.toDouble();
                tempString = tempList.value(4);
                myMesh.landBoundary[i].supercritical[j] = tempString.toDouble();
            }
        }
        else if(myMesh.landBoundary[i].code == 5  ||
                myMesh.landBoundary[i].code == 25 )
        {
            myMesh.landBoundary[i].node1.resize(myMesh.landBoundary[i].NumNodes);
            myMesh.landBoundary[i].node1_hash.resize(myMesh.landBoundary[i].NumNodes);
            myMesh.landBoundary[i].node2.resize(myMesh.landBoundary[i].NumNodes);
            myMesh.landBoundary[i].node2_hash.resize(myMesh.landBoundary[i].NumNodes);
            myMesh.landBoundary[i].elevation.resize(myMesh.landBoundary[i].NumNodes);
            myMesh.landBoundary[i].supercritical.resize(myMesh.landBoundary[i].NumNodes);
            myMesh.landBoundary[i].subcritical.resize(myMesh.landBoundary[i].NumNodes);
            myMesh.landBoundary[i].pipe_ht.resize(myMesh.landBoundary[i].NumNodes);
            myMesh.landBoundary[i].pipe_coef.resize(myMesh.landBoundary[i].NumNodes);
            myMesh.landBoundary[i].pipe_diam.resize(myMesh.landBoundary[i].NumNodes);
            for(j=0;j<myMesh.landBoundary[i].NumNodes;j++)
            {
                tempString = meshFile.readLine().simplified();
                tempList = tempString.split(" ");
                myMesh.landBoundary[i].node1_hash[j] = tempList.value(0);
                myMesh.landBoundary[i].node2_hash[j] = tempList.value(1);
                tempString = tempList.value(2);
                myMesh.landBoundary[i].elevation[j] = tempString.toDouble();
                tempString = tempList.value(3);
                myMesh.landBoundary[i].subcritical[j] = tempString.toDouble();
                tempString = tempList.value(4);
                myMesh.landBoundary[i].supercritical[j] = tempString.toDouble();
                tempString = tempList.value(5);
                myMesh.landBoundary[i].pipe_ht[j] = tempString.toDouble();
                tempString = tempList.value(6);
                myMesh.landBoundary[i].pipe_coef[j] = tempString.toDouble();
                tempString = tempList.value(7);
                myMesh.landBoundary[i].pipe_diam[j] = tempString.toDouble();
            }
        }
        else
        {
            myMesh.landBoundary[i].node1.resize(myMesh.landBoundary[i].NumNodes);
            myMesh.landBoundary[i].node1_hash.resize(myMesh.landBoundary[i].NumNodes);
            for(j=0;j<myMesh.landBoundary[i].NumNodes;j++)
            {
                tempString = meshFile.readLine().simplified();
                myMesh.landBoundary[i].node1_hash[j] = tempString;
            }
        }
    }

    meshFile.close();
    return ERR_NOERR;
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//...Write the adcirc mesh in standard format
//------------------------------------------------------------------------------
#ifdef GUI
int adcirc_hashlib::writeAdcircMesh(QString fileName, adcirc_mesh &myMesh, QProgressDialog &dialog, int &counter)
#else
int adcirc_hashlib::writeAdcircMesh(QString fileName, adcirc_mesh &myMesh)
#endif
{
    QString line,elevation,supercritical,subcritical,pipeht,pipecoef,pipediam,node1,node2;
    int i,j;
    QFile outputFile(fileName);
    QTextStream output(&outputFile);
    outputFile.open(QIODevice::WriteOnly);

#ifdef GUI
    //...Set the progress bar update time
    polling = QTime::currentTime().addMSecs(100);

    dialog.setLabelText("Writing the ADCIRC mesh...");
#endif

    output << myMesh.header << "\n";
    output << myMesh.NumElements << " " << myMesh.NumNodes << "\n";
    for(i=0;i<myMesh.NumNodes;i++)
    {
        line.sprintf("%10i %+18.12e %+18.12e %+18.12e \n",i+1,myMesh.node[i].x,
                     myMesh.node[i].y,myMesh.node[i].z);
        output << line;

#ifdef GUI
        //...Update the progress bar
        updateProgress(counter,dialog);

        //...Catch the cancelled signal
        if(dialog.wasCanceled())
            return ERR_CANCELED;
#endif
    }

    for(i=0;i<myMesh.NumElements;i++)
    {
        line.sprintf("%10i %5i %10i %10i %10i \n",i+1,3,myMesh.element[i].c1+1,
                myMesh.element[i].c2+1,myMesh.element[i].c3+1);
        output << line;

#ifdef GUI
        //...Update the progress bar
        updateProgress(counter,dialog);

        //...Catch the cancelled signal
        if(dialog.wasCanceled())
            return ERR_CANCELED;
#endif
    }

    output << myMesh.NumOpenBoundaries << " \n";
    output << myMesh.NumOpenBoundaryNodes << " \n";

    for(i=0;i<myMesh.NumOpenBoundaries;i++)
    {
        output << myMesh.openBoundary[i].NumNodes << " \n";
        for(j=0;j<myMesh.openBoundary[i].NumNodes;j++)
        {
            node1.sprintf("%10i",myMesh.openBoundary[i].node1[j]+1);
            output << node1 << " \n";
        }
    }

    output << myMesh.NumLandBoundaries << " \n";
    output << myMesh.NumLandBoundaryNodes << " \n";

    for(i=0;i<myMesh.NumLandBoundaries;i++)
    {
        output << myMesh.landBoundary[i].NumNodes << " "
               <<  myMesh.landBoundary[i].code << " \n";
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
            myMesh.landBoundary[i].code == 122 )
        {
            for(j=0;j<myMesh.landBoundary[i].NumNodes;j++)
            {
                node1.sprintf("%10i",myMesh.landBoundary[i].node1[j]+1);
                output << node1 << " \n";
            }
        }
        else if(myMesh.landBoundary[i].code == 3  ||
                myMesh.landBoundary[i].code == 13 ||
                myMesh.landBoundary[i].code == 23 )
        {
            for(j=0;j<myMesh.landBoundary[i].NumNodes;j++)
            {
                node1.sprintf("%10i",myMesh.landBoundary[i].node1[j]+1);
                elevation.sprintf("%+18.12e",myMesh.landBoundary[i].elevation[j]);
                supercritical.sprintf("%+18.12e",myMesh.landBoundary[i].supercritical[j]);
                output << node1 << " " << elevation << " " << supercritical << " \n";
            }
        }
        else if(myMesh.landBoundary[i].code == 4  ||
                myMesh.landBoundary[i].code == 24 )
        {
            for(j=0;j<myMesh.landBoundary[i].NumNodes;j++)
            {
                node1.sprintf("%10i",myMesh.landBoundary[i].node1[j]+1);
                node2.sprintf("%10i",myMesh.landBoundary[i].node2[j]+1);
                elevation.sprintf("%+18.12e",myMesh.landBoundary[i].elevation[j]);
                supercritical.sprintf("%+18.12e",myMesh.landBoundary[i].supercritical[j]);
                subcritical.sprintf("%+18.12e",myMesh.landBoundary[i].subcritical[j]);
                output << node1 << " " << node2 << " " << elevation << " "
                       << subcritical << " " << supercritical << " \n";
            }
        }
        else if(myMesh.landBoundary[i].code == 5  ||
                myMesh.landBoundary[i].code == 25 )
        {
            for(j=0;j<myMesh.landBoundary[i].NumNodes;j++)
            {
                node1.sprintf("%10i",myMesh.landBoundary[i].node1[j]+1);
                node2.sprintf("%10i",myMesh.landBoundary[i].node2[j]+1);
                elevation.sprintf("%+18.12e",myMesh.landBoundary[i].elevation[j]);
                supercritical.sprintf("%+18.12e",myMesh.landBoundary[i].supercritical[j]);
                subcritical.sprintf("%+18.12e",myMesh.landBoundary[i].subcritical[j]);
                pipeht.sprintf("%+18.12e",myMesh.landBoundary[i].pipe_ht[j]);
                pipecoef.sprintf("%+18.12e",myMesh.landBoundary[i].pipe_coef[j]);
                pipediam.sprintf("%+18.12e",myMesh.landBoundary[i].pipe_diam[j]);
                output << node1 << " " << node2 << " " << elevation << " "
                       << subcritical << " " << supercritical << " "
                       << pipeht << " " << pipecoef << " " << pipediam << " \n";
            }
        }
        else
        {
            for(j=0;j<myMesh.landBoundary[i].NumNodes;j++)
            {
                node1.sprintf("%10i",myMesh.landBoundary[i].node1[j]+1);
                output << node1 << " \n";
            }
        }

    }
    outputFile.close();
    return ERR_NOERR;
}
//------------------------------------------------------------------------------

