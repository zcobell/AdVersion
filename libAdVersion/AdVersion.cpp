//-----GPL----------------------------------------------------------------------
//
// This file is part of AdVersion
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
//  File: AdVersion.cpp
//
//------------------------------------------------------------------------------
#include "AdVersion.h"
#include <float.h>
#include <QtMath>
#include <QStringList>
#include <stdlib.h>



//-----------------------------------------------------------------------------------------//
//
//
//   C O N S T R U C T O R  /  D E S T R U C T O R
//             M E T H O D S
//
//
//-----------------------------------------------------------------------------------------//



//-----------------------------------------------------------------------------------------//
//...Constructor for AdVersion object
//-----------------------------------------------------------------------------------------//
/**
 * \brief Constructor for AdVersion object
 *
 * Constructor for AdVersion object
 *
 **/
//-----------------------------------------------------------------------------------------//
AdVersion::AdVersion(QObject *parent) : QObject(parent)
{
    this->mesh = NULL;
    this->fort13 = NULL;
    this->hashAlgorithm = QCryptographicHash::Sha1;
    this->nMeshPartitions = -1;
}
//-----------------------------------------------------------------------------------------//



//-----------------------------------------------------------------------------------------//
//...Destructor for AdVersion object
//-----------------------------------------------------------------------------------------//
/**
 * \brief Destrictor for AdVersion object
 *
 * Destructor for AdVersion object
 *
 **/
//-----------------------------------------------------------------------------------------//
AdVersion::~AdVersion()
{

}
//-----------------------------------------------------------------------------------------//



//-----------------------------------------------------------------------------------------//
//
//
//   P U B L I C
//             M E T H O D S
//
//
//-----------------------------------------------------------------------------------------//



//-----------------------------------------------------------------------------------------//
//...Method to create the mesh partitions
//-----------------------------------------------------------------------------------------//
/**
 * \brief Method to create a partitioned mesh using METIS
 *
 * @param[in]  meshFile       ADCIRC formatted file to read
 * @param[in]  outputFile     Directory to create for the partitioned mesh
 * @param[in]  numPartitions  Number of partitions to divide the mesh into
 *
 * Method to create a partitioned mesh using METIS
 *
 **/
//-----------------------------------------------------------------------------------------//
int AdVersion::createPartitions(QString meshFile, QString outputFile, int numPartitions)
{

    int ierr;

    this->nMeshPartitions = numPartitions;

    //...Get the adcirc mesh that will be partitioned
    if(this->mesh==NULL)
    {
        this->mesh = new adcirc_mesh(this);
        ierr = this->mesh->read(meshFile);
        if(ierr!=ERROR_NOERROR)
            return -1;
    }

    //...Build the directory tree (if necessary)
    ierr = this->generateDirectoryNames(outputFile);
    if(ierr!=ERROR_NOERROR)
        return -1;

    ierr = this->buildDirectoryTree(outputFile);
    if(ierr!=ERROR_NOERROR)
        return -2;

    //...Use METIS to partition the mesh
    ierr = this->metisPartition();
    if(ierr!=ERROR_NOERROR)
        return -3;

    //...Build the rectangles from the METIS partition
    ierr = this->buildRectangles();
    if(ierr!=ERROR_NOERROR)
        return -4;

    //...Write the partitions to the file
    ierr = this->writeRectanglePartitions();
    if(ierr!=ERROR_NOERROR)
        return -5;

    return ERROR_NOERROR;

}
//-----------------------------------------------------------------------------------------//



//-----------------------------------------------------------------------------------------//
//...Method to create the mesh partitions
//-----------------------------------------------------------------------------------------//
/**
 * \fn AdVersion::writePartitionedMesh(QString meshFile, QString outputFile)
 * \brief Method to write the partitioned mesh
 *
 * @param[in]  meshFile       ADCIRC formatted file to read
 * @param[in]  outputFile     Directory to create for the partitioned mesh
 *
 * Method to write the partitioned mesh and optionally fort13
 *
 **/
//-----------------------------------------------------------------------------------------//
int AdVersion::writePartitionedMesh(QString meshFile, QString outputFile)
{
    int ierr = this->writeAdvMesh(meshFile,NULL,outputFile);
    return ierr;
}
//-----------------------------------------------------------------------------------------//



//-----------------------------------------------------------------------------------------//
//...Method to create the mesh partitions
//-----------------------------------------------------------------------------------------//
/**
 * \overload AdVersion::writePartitionedMesh(QString meshFile, QString fort13File, QString outputFile)
 * \brief Method to write the partitioned mesh
 *
 * @param[in]  meshFile       ADCIRC formatted file to read
 * @param[in]  fort13File     ADCIRC formatted fort13 file to read
 * @param[in]  outputFile     Directory to create for the partitioned mesh
 *
 * Method to write the partitioned mesh and optionally fort13
 *
 **/
//-----------------------------------------------------------------------------------------//
int AdVersion::writePartitionedMesh(QString meshFile, QString fort13File, QString outputFile)
{
    int ierr = this->writeAdvMesh(meshFile,fort13File,outputFile);
    return ierr;
}
//-----------------------------------------------------------------------------------------//



//-----------------------------------------------------------------------------------------//
//...Method to read the mesh that has been partitioned into the repository
//-----------------------------------------------------------------------------------------//
/**
 * \brief Method to read the mesh that has been partitioned into the repository
 *
 * @param[in] meshFolder (named *.adv) to read that contains the repository
 *
 * Method to read the mesh that has been partitioned into the repository
 *
 **/
//-----------------------------------------------------------------------------------------//
int AdVersion::readPartitionedMesh(QString meshFolder,bool readNodalAttributes)
{
    int i,j,nNodes,nNodesInMesh,nElements,nElementsInMesh;
    int nodeIndex,elementIndex,code,ierr;
    qreal x,y,z;
    QString tempLine,filename,file,tempString,boundaryHash;
    QString tempX,tempY,tempZ,codeString,nNodesString;
    QString nodeHash,elementHash,nodeHash1,nodeHash2,nodeHash3;
    QStringList tempList,tempList2;
    QByteArray fileData;
    QFile thisFile,openBoundFile,landBoundFile;
    QVector<QString> openBoundaryHashFilenames;
    QVector<QString> landBoundaryHashFilenames;
    adcirc_node *tempNode;
    adcirc_element *tempElement;
    adcirc_boundary *tempBoundary;

    //...Check if folder exists
    QFile folder(meshFolder);
    if(!folder.exists())
        return -1;

    //...Generate the directory names
    this->generateDirectoryNames(meshFolder);

    //...Check that all the system files we need exist
    QFile meshHeaderFile(this->systemDir.path()+"/mesh.header");
    QFile partitionFile(this->systemDir.path()+"/partition.control");
    QFile hashFile(this->systemDir.path()+"/hash.type");

    if(!meshHeaderFile.exists())
        return -1;

    if(!partitionFile.exists())
        return -1;

    if(!hashFile.exists())
        return -1;

    //...Start up a new mesh
    this->mesh = new adcirc_mesh(this);

    //...Read the system files
    if(!meshHeaderFile.open(QIODevice::ReadOnly))
        return -1;

    if(!partitionFile.open(QIODevice::ReadOnly))
        return -1;

    tempLine = meshHeaderFile.readLine().simplified();
    this->mesh->title = tempLine;
    meshHeaderFile.close();

    tempLine = partitionFile.readLine().simplified();
    this->nMeshPartitions = tempLine.toInt();

    //...Check if all the node files exist
    for(i=0;i<this->nMeshPartitions;i++)
    {
        file.sprintf("partition_%4.4i.node",i);
        filename = this->nodeDir.path()+"/"+file;
        thisFile.setFileName(filename);
        if(!thisFile.exists())
            return -1;
    }

    //...Check if all the element files exist
    for(i=0;i<this->nMeshPartitions;i++)
    {
        file.sprintf("partition_%4.4i.element",i);
        filename = this->elemDir.path()+"/"+file;
        thisFile.setFileName(filename);
        if(!thisFile.exists())
            return -1;
    }

    //...Check if all the boundary files exists
    openBoundFile.setFileName(this->openBoundDir.path()+"/open.boundary");
    if(!openBoundFile.open(QIODevice::ReadOnly))
        return -1;

    tempLine = openBoundFile.readLine().simplified();
    this->mesh->numOpenBoundaries = tempLine.toInt();
    openBoundaryHashFilenames.resize(this->mesh->numOpenBoundaries);

    for(i=0;i<this->mesh->numOpenBoundaries;i++)
    {
        openBoundaryHashFilenames[i] = this->openBoundDir.path()+"/"+openBoundFile.readLine().simplified()+".bnd";
        thisFile.setFileName(openBoundaryHashFilenames[i]);
        if(!thisFile.exists())
            return -1;
    }
    openBoundFile.close();

    landBoundFile.setFileName(this->landBoundDir.path()+"/land.boundary");
    if(!landBoundFile.open(QIODevice::ReadOnly))
        return -1;

    tempLine = landBoundFile.readLine().simplified();
    this->mesh->numLandBoundaries = tempLine.toInt();
    landBoundaryHashFilenames.resize(this->mesh->numLandBoundaries);

    for(i=0;i<this->mesh->numLandBoundaries;i++)
    {
        landBoundaryHashFilenames[i] = this->landBoundDir.path()+"/"+landBoundFile.readLine().simplified()+".bnd";
        thisFile.setFileName(landBoundaryHashFilenames[i]);
        if(!thisFile.exists())
            return -1;
    }
    landBoundFile.close();

    //...Start reading the node files
    this->nodeList.resize(this->nMeshPartitions);
    nNodesInMesh = 0;
    for(i=0;i<this->nMeshPartitions;i++)
    {
        file.sprintf("partition_%4.4i.node",i);
        filename = this->nodeDir.path()+"/"+file;
        thisFile.setFileName(filename);
        if(!thisFile.open(QIODevice::ReadOnly))
            return -1;

        fileData = thisFile.readAll();
        tempList = QString(fileData).split("\n");
        thisFile.close();

        tempLine = tempList.value(0);
        nNodes   = tempLine.toInt();

        for(j=0;j<nNodes;j++)
        {
            tempString = tempList.value(j+1);
            tempList2  = tempString.simplified().split(" ");
            nodeHash   = tempList2.value(0);
            tempX      = tempList2.value(1);
            tempY      = tempList2.value(2);
            tempZ      = tempList2.value(3);
            x          = tempX.toDouble();
            y          = tempY.toDouble();
            z          = tempZ.toDouble();
            tempNode   = new adcirc_node(this);
            tempNode->position.setX(x);
            tempNode->position.setY(y);
            tempNode->position.setZ(z);
            tempNode->positionHash = nodeHash;
            this->nodeList[i].append(tempNode);
            nNodesInMesh = nNodesInMesh + 1;
        }
    }
    this->mesh->numNodes = nNodesInMesh;

    //...Rectify the nodes to the mesh object
    nodeIndex = 0;
    this->mesh->nodes.resize(nNodesInMesh);
    for(i=0;i<this->nMeshPartitions;i++)
    {
        for(j=0;j<this->nodeList[i].length();j++)
        {
            this->mesh->nodes[nodeIndex] = nodeList[i].at(j);
            this->mesh->nodes[nodeIndex]->id = nodeIndex+1;
            this->nodeMap[this->mesh->nodes[nodeIndex]->positionHash.toUtf8()] = this->mesh->nodes[nodeIndex];
            nodeIndex = nodeIndex + 1;
        }
    }

    //...Read the element files
    this->elementList.resize(this->nMeshPartitions);
    nElementsInMesh = 0;
    for(i=0;i<this->nMeshPartitions;i++)
    {
        file.sprintf("partition_%4.4i.element",i);
        filename = this->elemDir.path()+"/"+file;
        thisFile.setFileName(filename);
        if(!thisFile.open(QIODevice::ReadOnly))
            return -1;

        fileData = thisFile.readAll();
        tempList = QString(fileData).split("\n");
        thisFile.close();

        tempLine  = tempList.value(0);
        nElements = tempLine.toInt();

        for(j=0;j<nElements;j++)
        {
            tempString  = tempList.value(j+1);
            tempList2   = tempString.simplified().split(" ");
            elementHash = tempList2.value(0);
            nodeHash1   = tempList2.value(1);
            nodeHash2   = tempList2.value(2);
            nodeHash3   = tempList2.value(3);
            tempElement = new adcirc_element(this);

            tempElement->hash = elementHash;
            tempElement->connections[0] = nodeMap[nodeHash1.toUtf8()];
            tempElement->connections[1] = nodeMap[nodeHash2.toUtf8()];
            tempElement->connections[2] = nodeMap[nodeHash3.toUtf8()];
            nElementsInMesh = nElementsInMesh + 1;

            this->elementList[i].append(tempElement);

        }
    }
    this->mesh->numElements = nElementsInMesh;


    //...Rectify the elements to the mesh object
    elementIndex = 0;
    this->mesh->elements.resize(nElementsInMesh);
    for(i=0;i<this->nMeshPartitions;i++)
    {
        for(j=0;j<this->elementList[i].length();j++)
        {
            this->mesh->elements[elementIndex] = elementList[i].at(j);
            this->mesh->elements[elementIndex]->id = elementIndex+1;
            elementIndex = elementIndex + 1;
        }
    }

    //...Read the open boundaries
    this->mesh->openBC.resize(this->mesh->numOpenBoundaries);
    for(i=0;i<this->mesh->numOpenBoundaries;i++)
    {
        thisFile.setFileName(openBoundaryHashFilenames[i]);
        if(!thisFile.open(QIODevice::ReadOnly))
                return -1;

        tempLine     = thisFile.readLine().simplified();
        tempList     = tempLine.split(" ");
        boundaryHash = tempList.value(0);
        nNodesString = tempList.value(1);
        nNodes       = nNodesString.toInt();
        codeString   = tempList.value(2);
        code         = codeString.toInt();

        tempBoundary = new adcirc_boundary(this);
        tempBoundary->setupBoundary(code,nNodes);

        for(j=0;j<tempBoundary->numNodes;j++)
        {
            tempLine = thisFile.readLine().simplified();
            ierr     = this->readBoundaryHashLine(tempLine,tempBoundary,j,this->nodeMap);
        }

        thisFile.close();

        this->mesh->openBC[i] = tempBoundary;

    }


    //...Read the land boundaries
    this->mesh->landBC.resize(this->mesh->numLandBoundaries);
    for(i=0;i<this->mesh->numLandBoundaries;i++)
    {
        thisFile.setFileName(landBoundaryHashFilenames[i]);
        if(!thisFile.open(QIODevice::ReadOnly))
                return -1;

        tempLine     = thisFile.readLine().simplified();
        tempList     = tempLine.split(" ");
        boundaryHash = tempList.value(0);
        nNodesString = tempList.value(1);
        nNodes       = nNodesString.toInt();
        codeString   = tempList.value(2);
        code         = codeString.toInt();

        tempBoundary = new adcirc_boundary(this);
        tempBoundary->setupBoundary(code,nNodes);

        for(j=0;j<tempBoundary->numNodes;j++)
        {
            tempLine = thisFile.readLine().simplified();
            ierr     = this->readBoundaryHashLine(tempLine,tempBoundary,j,nodeMap);
        }

        thisFile.close();

        this->mesh->landBC[i] = tempBoundary;

    }

    //...Sort the boundaries from east-->west
    for(i=0;i<this->mesh->numOpenBoundaries;i++)
        this->mesh->openBC[i]->calculateAverageLongitude();

    for(i=0;i<this->mesh->numLandBoundaries;i++)
        this->mesh->landBC[i]->calculateAverageLongitude();

    std::sort(this->mesh->openBC.begin(),this->mesh->openBC.end(),AdVersion::boundaryPositionLessThan);
    std::sort(this->mesh->landBC.begin(),this->mesh->landBC.end(),AdVersion::boundaryPositionLessThan);

    this->renumber();

    if(readNodalAttributes)
        this->readPartitionedNodalAttributes();

    return ERROR_NOERROR;
}
//-----------------------------------------------------------------------------------------//



//-----------------------------------------------------------------------------------------//
//...Method to call the routine to write a mesh in QADCModuless
//-----------------------------------------------------------------------------------------//
/**
 * \brief Method to call the routine to write a mesh in QADCModules
 *
 * @param[in] outputFile  Name of the file to write
 *
 * Method to call the routine to write a mesh in QADCModules
 *
 **/
//-----------------------------------------------------------------------------------------//
int AdVersion::writeMesh(QString outputFile, QString output13)
{

    QFile meshFile(outputFile);
    if(meshFile.exists())
        meshFile.remove();

    int ierr;
    ierr = this->mesh->write(outputFile);

    if(ierr!=ERROR_NOERROR)
        return ierr;

    if(!output13.isNull())
    {
        QFile fort13File(output13);
        if(fort13File.exists())
            fort13File.remove();

        ierr = this->fort13->write(output13);
        if(ierr!=ERROR_NOERROR)
            return ierr;
    }

    return ERROR_NOERROR;
}
//-----------------------------------------------------------------------------------------//



