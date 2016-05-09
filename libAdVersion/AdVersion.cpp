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
#include "metis.h"
#include "git2.h"
#include <float.h>
#include <QtMath>
#include <QStringList>
#include <stdlib.h>

//-----------------------------------------------------------------------------------------//
//
//
//   C O M P A R I S O N
//             M E T H O D S
//
//
//-----------------------------------------------------------------------------------------//


//-----------------------------------------------------------------------------------------//
//...Comparison operator for ordering node hashes
//-----------------------------------------------------------------------------------------//
/**
 * \brief Comparison operator for ordering node hashes
 *
 * @param[in] node1  first node for comparison
 * @param[in] node2  second node for comparison
 *
 * Comparison operator for ordering node hashes. Return is if node1 < node2
 *
 **/
//-----------------------------------------------------------------------------------------//
bool nodeHashLessThan(const adcirc_node *node1, const adcirc_node *node2)
{
    if(node1->positionHash<node2->positionHash)
        return true;
    else
        return false;
}
//-----------------------------------------------------------------------------------------//



//-----------------------------------------------------------------------------------------//
//...Comparison operator for ordering element hashes
//-----------------------------------------------------------------------------------------//
/**
 * \brief Comparison operator for ordering element hashes
 *
 * @param[in] element1  first element for comparison
 * @param[in] element2  second element for comparison
 *
 * Comparison operator for ordering node hashes. Return is if element1 < element2
 *
 **/
//-----------------------------------------------------------------------------------------//
bool elementHashLessThan(const adcirc_element *element1, const adcirc_element *element2)
{
    if(element1->hash<element2->hash)
        return true;
    else
        return false;
}
//-----------------------------------------------------------------------------------------//



//-----------------------------------------------------------------------------------------//
//...Comparison operator for ordering boundary hashes
//-----------------------------------------------------------------------------------------//
/**
 * \brief Comparison operator for ordering boundary hashes
 *
 * @param[in] boundary1  first boundary for comparison
 * @param[in] boundary2  second boundary for comparison
 *
 * Comparison operator for ordering boundary hashes. Return is if boundary1 < boundary2
 *
 **/
//-----------------------------------------------------------------------------------------//
bool boundaryHashLessThan(const adcirc_boundary *boundary1, const adcirc_boundary *boundary2)
{
    if(boundary1->hash<boundary2->hash)
        return true;
    else
        return false;
}
//-----------------------------------------------------------------------------------------//



//-----------------------------------------------------------------------------------------//
//...Comparison operator for ordering rectangle areas
//-----------------------------------------------------------------------------------------//
/**
 * \brief Comparison operator for ordering rectangle areas
 *
 * @param[in] rectangle1  first rectangle for comparison
 * @param[in] rectangle2  second rectangle for comparison
 *
 * Comparison operator for ordering node hashes. Return is if rectangle1 < rectangle2
 *
 **/
//-----------------------------------------------------------------------------------------//
bool rectangeleAreaLessThan(const Rectangle rectangle1, const Rectangle rectangle2)
{
    if(rectangle1.area<rectangle2.area)
        return true;
    else
        return false;
}
//-----------------------------------------------------------------------------------------//



//-----------------------------------------------------------------------------------------//
//...Comparison operator for ordering node IDs
//-----------------------------------------------------------------------------------------//
/**
 * \brief Comparison operator for ordering node IDs
 *
 * @param[in] node1  first node for comparison
 * @param[in] node2  second node for comparison
 *
 * Comparison operator for ordering node IDs. Return is if node1 < node2
 *
 **/
//-----------------------------------------------------------------------------------------//
bool nodeNumberLessThan(const adcirc_node *node1, const adcirc_node *node2)
{
    return node1->id<node2->id;
}
//-----------------------------------------------------------------------------------------//


//-----------------------------------------------------------------------------------------//
//...Comparison operator for ordering element by the sum of their nodes
//-----------------------------------------------------------------------------------------//
/**
 * \brief Comparison operator for ordering elements by the sum of their node IDs
 *
 * @param[in] node1  first node for comparison
 * @param[in] node2  second node for comparison
 *
 * Comparison operator for ordering elements by the sum of their node IDs. This does a decent
 * job of ordering the element IDs without doing something more complex
 *
 **/
//-----------------------------------------------------------------------------------------//
bool elementSumLessThan(const adcirc_element *element1, const adcirc_element *element2)
{
    int s1 = element1->connections[0]->id + element1->connections[1]->id + element1->connections[2]->id;
    int s2 = element2->connections[0]->id + element2->connections[1]->id + element1->connections[2]->id;
    return s1<s2;
}
//-----------------------------------------------------------------------------------------//



//-----------------------------------------------------------------------------------------//
//...Comparison operator for ordering boundary conditions
//-----------------------------------------------------------------------------------------//
/**
 * \brief Comparison operator for ordering boundaries by position geographicly
 *
 * @param[in] boundary1  first boundary for comparison
 * @param[in] boundary2  second boundary for comparison
 *
 * Comparison operator for ordering boundary conditions. Return is if boundary1 > boundary2
 * so that boundaries are positioned east-->west
 *
 **/
//-----------------------------------------------------------------------------------------//
bool boundaryPositionLessThan(const adcirc_boundary *boundary1,const adcirc_boundary *boundary2)
{
    return boundary1->averageLongitude > boundary2->averageLongitude;
}
//-----------------------------------------------------------------------------------------//




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
    this->hashAlgorithm = QCryptographicHash::Sha1;
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
//   S T A T I C
//             M E T H O D S
//
//
//-----------------------------------------------------------------------------------------//



//-----------------------------------------------------------------------------------------//
//...Static method to retrieve the Git version form a repository
//-----------------------------------------------------------------------------------------//
/**
 * \brief Static method to return the Git version of the HEAD in a Git repository
 *
 * @param[in]  gitDirectory Location of the Git repository
 * @param[out] version      Current revision of the Git repository
 *
 * Static method to return the Git version fo the HEAD in a Git repository.
 * Equivalent to: git describe --always --tags
 *
 **/
//-----------------------------------------------------------------------------------------//
int AdVersion::getGitVersion(QString gitDirectory, QString &version)
{
    int ierr;
    QFile dir(gitDirectory+"/.git");
    QByteArray tempData;
    git_repository *repo;
    git_describe_result *description;
    git_describe_options options;
    git_describe_format_options format;
    git_object *headObject;
    git_buf buffer = { 0 };

    version = QString();

    if(!dir.exists())
        return -1;

    tempData = gitDirectory.toLatin1();
    const char *cgitDirectory = tempData.data();

    git_libgit2_init();

    ierr = git_repository_open(&repo,cgitDirectory);
    if(ierr<0)
    {
        git_repository_free(repo);
        git_libgit2_shutdown();
        return ierr;
    }

    ierr = git_describe_init_options(&options,GIT_DESCRIBE_OPTIONS_VERSION);
    options.show_commit_oid_as_fallback = 1;
    if(ierr<0)
    {
        git_repository_free(repo);
        git_libgit2_shutdown();
        return ierr;
    }

    ierr = git_describe_init_format_options(&format,GIT_DESCRIBE_FORMAT_OPTIONS_VERSION);
    if(ierr<0)
    {
        git_repository_free(repo);
        git_libgit2_shutdown();
        return ierr;
    }

    ierr = git_revparse_single(&headObject,repo,"HEAD");
    if(ierr<0)
    {
        git_object_free(headObject);
        git_repository_free(repo);
        git_libgit2_shutdown();
        return ierr;
    }

    ierr = git_describe_commit(&description,headObject,&options);
    if(ierr<0)
    {
        git_object_free(headObject);
        git_repository_free(repo);
        git_libgit2_shutdown();
        return ierr;
    }

    ierr = git_describe_format(&buffer,description,&format);
    if(ierr<0)
    {
        git_object_free(headObject);
        git_describe_result_free(description);
        git_repository_free(repo);
        git_libgit2_shutdown();
        return ierr;
    }

    version.sprintf("%s",buffer.ptr);

    git_object_free(headObject);
    git_describe_result_free(description);
    git_repository_free(repo);
    git_libgit2_shutdown();

    return ERROR_NOERROR;
}
//-----------------------------------------------------------------------------------------//



//-----------------------------------------------------------------------------------------//
//...Static method to initialize an empty Git repository
//-----------------------------------------------------------------------------------------//
/**
 * \brief Static method to initialize an empty Git repository
 *
 * @param[in]  gitDirectory Location of the Git repository
 *
 * Static method to initialize an empty Git repository
 *
 **/
//-----------------------------------------------------------------------------------------//
int AdVersion::gitInit(QString gitDirectory)
{

    int ierr;
    git_repository *repository;
    QByteArray tempData = gitDirectory.toLatin1();
    const char *cgitDirectory = tempData.data();

    git_libgit2_init();
    ierr = git_repository_init(&repository,cgitDirectory,0);
    git_repository_free(repository);
    git_libgit2_shutdown();
    return ERROR_NOERROR;
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
//...Method to write the partitioned mesh
//-----------------------------------------------------------------------------------------//
/**
 * \brief Method to write the partitioned mesh to the output directory
 *
 * @param[in]  meshFile       ADCIRC formatted file to read
 * @param[in]  outputFile     Directory to create for the partitioned mesh
 *
 * Method to write the partitioned mesh files. Code assumes that partitioning has already
 * been completed and that the partition files can be found in the outputFile folder.
 *
 **/
//-----------------------------------------------------------------------------------------//
int AdVersion::writePartitionedMesh(QString meshFile, QString outputFile)
{
    int ierr;

    //...Initialize an empty git repository if necessary
    QFile mainDirectory(outputFile);
    if(!mainDirectory.exists())
        QDir().mkdir(outputFile);
    QFile gitDirectory(outputFile+"/.git");
    if(!gitDirectory.exists())
        ierr = AdVersion::gitInit(outputFile);

    //...Create a new adcirc_mesh if needed
    if(this->mesh==NULL)
    {
        this->mesh = new adcirc_mesh(this);

        //...Read the mesh file in ADCIRC format
        ierr = this->mesh->read(meshFile);
        if(ierr!=ERROR_NOERROR)
            return -1;
    }

    //...Compute the hashes for the mesh
    ierr = this->mesh->setHashAlgorithm(this->hashAlgorithm);
    ierr = this->mesh->hashMesh();
    if(ierr!=ERROR_NOERROR)
        return -1;

    //...Build the directory tree for writing
    ierr = this->buildDirectoryTree(outputFile);

    //...Partition the nodes to the subdomains
    ierr = this->partitionMesh();

    //...Write the node files
    ierr = this->writeNodeFiles();

    //...Write the element files
    ierr = this->writeElementFiles();

    //...Write the boundary files
    ierr = this->writeBoundaryFiles();

    //...Write the system files
    ierr = this->writeSystemFiles();

    return ERROR_NOERROR;
}
//-----------------------------------------------------------------------------------------//



//-----------------------------------------------------------------------------------------//
//...Method to read the mesh that has been partitioned into the repository
//-----------------------------------------------------------------------------------------//
/**
 * \brief Method to read the mesh that has been partitioned into the repository
 *
 * Method to read the mesh that has been partitioned into the repository
 *
 **/
//-----------------------------------------------------------------------------------------//
int AdVersion::readPartitionedMesh(QString meshFolder)
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
    QMap<QString,adcirc_node*> nodeMap;
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
            nodeMap[this->mesh->nodes[nodeIndex]->positionHash] = this->mesh->nodes[nodeIndex];
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
            tempElement->connections[0] = nodeMap[nodeHash1];
            tempElement->connections[1] = nodeMap[nodeHash2];
            tempElement->connections[2] = nodeMap[nodeHash3];
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
            ierr     = this->readBoundaryHashLine(tempLine,tempBoundary,j,nodeMap);
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

    std::sort(this->mesh->openBC.begin(),this->mesh->openBC.end(),boundaryPositionLessThan);
    std::sort(this->mesh->landBC.begin(),this->mesh->landBC.end(),boundaryPositionLessThan);

    this->renumber();

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
int AdVersion::writeMesh(QString outputFile)
{

    QFile thisFile(outputFile);
    if(thisFile.exists())
        thisFile.remove();

    int ierr;
    ierr = this->mesh->write(outputFile);
    return ierr;
}
//-----------------------------------------------------------------------------------------//



//-----------------------------------------------------------------------------------------//
//
//
//   P R I V A T E
//             M E T H O D S
//
//
//-----------------------------------------------------------------------------------------//



//-----------------------------------------------------------------------------------------//
//...Method to generate the directory names used in the directory tree
//-----------------------------------------------------------------------------------------//
/**
 * \brief Method to generate the directory names used in the directory tree
 *
 * @param[in]  directory   Directory to create for the partitioned mesh
 *
 * Method to generate the directory names used in the directory tree
 *
 **/
//-----------------------------------------------------------------------------------------//
int AdVersion::generateDirectoryNames(QString directory)
{
    this->myDir.setPath(directory);
    this->nodeDir.setPath(directory+"/nodes");
    this->elemDir.setPath(directory+"/elements");
    this->boundDir.setPath(directory+"/boundaries");
    this->openBoundDir.setPath(directory+"/boundaries/openBoundaries");
    this->landBoundDir.setPath(directory+"/boundaries/landBoundaries");
    this->systemDir.setPath(directory+"/system");
    return ERROR_NOERROR;
}
//-----------------------------------------------------------------------------------------//



//-----------------------------------------------------------------------------------------//
//...Method to generate the directory tree
//-----------------------------------------------------------------------------------------//
/**
 * \brief Method to generate the directory tree
 *
 * @param[in]  directory   Directory to create for the partitioned mesh
 *
 * Method to generate the directory tree
 *
 **/
//-----------------------------------------------------------------------------------------//
int AdVersion::buildDirectoryTree(QString directory)
{
    int ierr;
    bool err;

    //...Make the directory names
    ierr = this->generateDirectoryNames(directory);

    //...Create the main directories
    if(!myDir.exists())
    {
        err = QDir().mkdir(myDir.path());
        if(!err)
            return -1;
    }

    if(!nodeDir.exists())
    {
        err = QDir().mkdir(nodeDir.path());
        if(!err)
            return -1;
    }
    else
    {
        this->nodeDir.removeRecursively();
        err = QDir().mkdir(nodeDir.path());
        if(!err)
            return -1;
    }

    if(!elemDir.exists())
    {
        err = QDir().mkdir(elemDir.path());
        if(!err)
            return -1;
    }
    else
    {
        this->elemDir.removeRecursively();
        err = QDir().mkdir(elemDir.path());
        if(!err)
            return -1;
    }

    if(!boundDir.exists())
    {
        err = QDir().mkdir(boundDir.path());
        if(!err)
            return -1;
    }


    if(!openBoundDir.exists())
    {
        err = QDir().mkdir(openBoundDir.path());
        if(!err)
            return -1;
    }
    else
    {
        this->openBoundDir.removeRecursively();
        err = QDir().mkdir(openBoundDir.path());
        if(!err)
            return -1;
    }

    if(!landBoundDir.exists())
    {
        err = QDir().mkdir(landBoundDir.path());
        if(!err)
            return -1;
    }
    else
    {
        this->landBoundDir.removeRecursively();
        err = QDir().mkdir(landBoundDir.path());
        if(!err)
            return -1;
    }


    if(!systemDir.exists())
    {
        err = QDir().mkdir(systemDir.path());
        if(!err)
            return -1;
    }

    return ERROR_NOERROR;
}
//-----------------------------------------------------------------------------------------//



//-----------------------------------------------------------------------------------------//
//...Method to generate a line for the boundary file depending on boundary condition
//-----------------------------------------------------------------------------------------//
/**
 * \brief Method to generate a line for the boundary file depending on boundary condition
 *
 * @param[in]  boundary   boundary condition to write the line in the output file for
 * @param[in]  index      position along the boudnary to write the line in the output file for
 * @return                single line in the output file for one node along a boundary condition
 *
 * Method to generate a line for the boundary file depending on boundary condition
 *
 **/
//-----------------------------------------------------------------------------------------//
QString AdVersion::formatBoundaryHashLine(adcirc_boundary *boundary, int index)
{

    QString line;
    QString crest,super,sub,pipeht,pipediam,pipecoef;

    if(boundary->code==3 || boundary->code==13 || boundary->code==23)
    {
        crest.sprintf("%+018.12e",boundary->crest[index]);
        super.sprintf("%+018.12e",boundary->supercritical[index]);
        line = QString("%1 %2 %3 \n").arg(boundary->n1[index]->positionHash)
                                  .arg(crest)
                                  .arg(super);
    }
    else if(boundary->code==4 || boundary->code == 24)
    {
        crest.sprintf("%+018.12e",boundary->crest[index]);
        super.sprintf("%+018.12e",boundary->supercritical[index]);
        sub.sprintf("%+018.12e",boundary->supercritical[index]);
        line = QString("%1 %2 %3 %4 %5 \n").arg(boundary->n1[index]->positionHash)
                                        .arg(boundary->n2[index]->positionHash)
                                        .arg(crest)
                                        .arg(sub)
                                        .arg(super);
    }
    else if(boundary->code==5 || boundary->code == 25)
    {
        crest.sprintf("%+018.12e",boundary->crest[index]);
        super.sprintf("%+018.12e",boundary->supercritical[index]);
        sub.sprintf("%+018.12e",boundary->supercritical[index]);
        pipeht.sprintf("%+018.12e",boundary->pipeHeight[index]);
        pipediam.sprintf("%+018.12e",boundary->pipeDiam[index]);
        pipecoef.sprintf("%+018.12e",boundary->pipeCoef[index]);
        line = QString("%1 %2 %3 %4 %5 %6 %7 %8 \n").arg(boundary->n1[index]->positionHash)
                                                 .arg(boundary->n2[index]->positionHash)
                                                 .arg(crest)
                                                 .arg(sub)
                                                 .arg(super)
                                                 .arg(pipeht)
                                                 .arg(pipecoef)
                                                 .arg(pipediam);
    }
    else
        line = QString("%1 \n").arg(boundary->n1[index]->positionHash);

    return line;
}
//-----------------------------------------------------------------------------------------//



//-----------------------------------------------------------------------------------------//
//...Method to read a hashed boundary condition line from a file
//-----------------------------------------------------------------------------------------//
/**
 * \brief Function to read a hashed boundary condition line from a file
 *
 * @param[in]  boundary   boundary condition to write the line in the output file for
 * @param[in]  index      position along the boudnary to write the line in the output file for
 *
 * Method to generate a line for the boundary file depending on boundary condition
 *
 **/
//-----------------------------------------------------------------------------------------//
int AdVersion::readBoundaryHashLine(QString &line, adcirc_boundary *boundary, int index, QMap<QString,adcirc_node*> &map)
{
    qreal   crest,super,sub,pipeht,pipediam,pipecoef;
    QString crestS,superS,subS,pipehtS,pipediamS,pipecoefS;
    QString nodeHash1,nodeHash2;
    QStringList tempList;

    line     = line.simplified();
    tempList = line.split(" ");

    if(boundary->code==3 || boundary->code==13 || boundary->code==23)
    {
        nodeHash1 = tempList.value(0);
        crestS    = tempList.value(1);
        superS    = tempList.value(2);
        crest     = crestS.toDouble();
        super     = superS.toDouble();
        boundary->n1[index]            = map[nodeHash1];
        boundary->crest[index]         = crest;
        boundary->supercritical[index] = super;
    }
    else if(boundary->code==4 || boundary->code == 24)
    {
        nodeHash1 = tempList.value(0);
        nodeHash2 = tempList.value(1);
        crestS    = tempList.value(2);
        subS      = tempList.value(3);
        superS    = tempList.value(4);
        crest     = crestS.toDouble();
        sub       = subS.toDouble();
        super     = superS.toDouble();
        boundary->n1[index]            = map[nodeHash1];
        boundary->n2[index]            = map[nodeHash2];
        boundary->crest[index]         = crest;
        boundary->subcritical[index]   = sub;
        boundary->supercritical[index] = super;

    }
    else if(boundary->code==5 || boundary->code == 25)
    {
        nodeHash1 = tempList.value(0);
        nodeHash2 = tempList.value(1);
        crestS    = tempList.value(2);
        subS      = tempList.value(3);
        superS    = tempList.value(4);
        pipehtS   = tempList.value(5);
        pipecoefS = tempList.value(6);
        pipediamS = tempList.value(7);
        crest     = crestS.toDouble();
        sub       = subS.toDouble();
        super     = superS.toDouble();
        pipeht    = pipehtS.toDouble();
        pipecoef  = pipecoefS.toDouble();
        pipediam  = pipediamS.toDouble();
        boundary->n1[index]            = map[nodeHash1];
        boundary->n2[index]            = map[nodeHash2];
        boundary->crest[index]         = crest;
        boundary->subcritical[index]   = sub;
        boundary->supercritical[index] = super;
    }
    else
    {
        nodeHash1 = tempList.value(0);
        boundary->n1[index] = map[nodeHash1];
    }

    return 0;

}
//-----------------------------------------------------------------------------------------//



//-----------------------------------------------------------------------------------------//
//...Method to use METIS to partition the mesh
//-----------------------------------------------------------------------------------------//
/**
 * \brief Method to use METIS to partition the mesh into subdomains
 *
 * Method to use METIS to partition the mesh into subdomains. The METIS partitions
 * are taken and converted into rectangles so that searches are much easier to conduct
 * later on
 *
 **/
//-----------------------------------------------------------------------------------------//
int AdVersion::metisPartition()
{
    QVector<int> qptr,qind;
    int i,j,ierr,index;
    idx_t ne,nn,numflag,*xadj,*adj;
    idx_t nEdgeCut,ncommon,nPartitions,ncon;
    idx_t metis_options[METIS_NOPTIONS];
    adcirc_element *tempElement;
    adcirc_node *n1,*n2,*n3,*n4;

    //...Copy the old connectivity table
    continuousElementTable.resize(this->mesh->numElements);
    for(i=0;i<this->mesh->numElements;i++)
        this->continuousElementTable[i] = this->mesh->elements[i];

    //...Begin adding elements between the levees
    //   This avoids metis disconnecting the mesh
    //   on either side of the levees
    for(i=0;i<this->mesh->numLandBoundaries;i++)
    {
        if(this->mesh->landBC[i]->code == 4  ||
           this->mesh->landBC[i]->code == 24 ||
           this->mesh->landBC[i]->code == 5  ||
           this->mesh->landBC[i]->code == 25 )
        {
            for(j=0;j<this->mesh->landBC[i]->numNodes-1;j++)
            {
                n1 = this->mesh->landBC[i]->n1[j];
                n2 = this->mesh->landBC[i]->n2[j];
                n3 = this->mesh->landBC[i]->n1[j+1];
                n4 = this->mesh->landBC[i]->n2[j+1];

                tempElement = new adcirc_element(this);
                tempElement->connections[0] = n1;
                tempElement->connections[1] = n2;
                tempElement->connections[2] = n3;
                this->continuousElementTable.push_back(tempElement);

                tempElement = new adcirc_element(this);
                tempElement->connections[0] = n4;
                tempElement->connections[1] = n3;
                tempElement->connections[2] = n2;
                this->continuousElementTable.push_back(tempElement);
            }
        }
    }

    //...Begin setting things up for metis
    ne = continuousElementTable.length();
    nn = this->mesh->numNodes;
    ncon = 1;
    numflag = 0;
    ncommon = 1;
    nPartitions = this->nMeshPartitions;

    qptr.resize(ne+1);

    //...Build the connectivity table for METIS
    index = 1;
    for(i=0;i<ne;i++)
    {
        qind.push_back(continuousElementTable[i]->connections[0]->id-1);
        qind.push_back(continuousElementTable[i]->connections[1]->id-1);
        qind.push_back(continuousElementTable[i]->connections[2]->id-1);
        qptr[i] = qind.length()-1-2;
    }
    qptr[ne] = qind.length()-1;

    //...Allocate the arrays for METIS
    idx_t *eptr  = (idx_t*)malloc(qptr.length()*sizeof(idx_t));
    idx_t *eind  = (idx_t*)malloc(qind.length()*sizeof(idx_t));
    idx_t *npart = (idx_t*)malloc(this->mesh->numNodes*sizeof(idx_t));
    idx_t *epart = (idx_t*)malloc(ne*sizeof(idx_t));

    //...Save the arrays for METIS
    for(i=0;i<qptr.size();i++)
        eptr[i] = qptr[i];

    for(i=0;i<qind.size();i++)
        eind[i] = qind[i];

    //...Call METIS
    ierr = METIS_SetDefaultOptions(metis_options);
    ierr = METIS_MeshToNodal(&ne,&nn,eptr,eind,&numflag,&xadj,&adj);
    ierr = METIS_PartMeshNodal(&ne,&nn,eptr,eind,NULL,NULL,&nPartitions,NULL,metis_options,&nEdgeCut,epart,npart);

    //...Save the partitioning
    this->nodePartitionList.resize(this->mesh->numNodes);
    for(i=0;i<this->mesh->numNodes;i++)
        this->nodePartitionList[i] = npart[i];

    this->elementPartitionList.resize(this->continuousElementTable.length());
    for(i=0;i<continuousElementTable.length();i++)
        this->elementPartitionList[i] = epart[i];

    //...Delete the arrays
    free(eptr);
    free(eind);
    free(npart);
    free(epart);
    free(xadj);
    free(adj);

    return ERROR_NOERROR;

}
//-----------------------------------------------------------------------------------------//



//-----------------------------------------------------------------------------------------//
//...Method to build rectangles from the METIS partitions
//-----------------------------------------------------------------------------------------//
/**
 * \brief Method to use the METIS partition result to construct rectangles
 *
 * Method to use the METIS partition result to construct rectangles. Rectangles
 * are sorted in reverse area size order so that large rectangles do not dominate
 * the selection of points
 *
 **/
//-----------------------------------------------------------------------------------------//
int AdVersion::buildRectangles()
{
    int i,partition;
    qreal x,y;
    QVector<qreal> xmin,xmax,ymin,ymax;

    this->partitionRectangles.resize(this->nMeshPartitions);
    xmin.resize(this->nMeshPartitions);
    ymin.resize(this->nMeshPartitions);
    xmax.resize(this->nMeshPartitions);
    ymax.resize(this->nMeshPartitions);

    xmin.fill(DBL_MAX);
    xmax.fill(-DBL_MAX);
    ymin.fill(DBL_MAX);
    ymax.fill(-DBL_MAX);

    for(i=0;i<this->mesh->numNodes;i++)
    {
        partition = this->nodePartitionList[i];
        x = this->mesh->nodes[i]->position.x();
        y = this->mesh->nodes[i]->position.y();
        if(x<xmin[partition])
            xmin[partition] = x;
        if(x>xmax[partition])
            xmax[partition] = x;
        if(y<ymin[partition])
            ymin[partition] = y;
        if(y>ymax[partition])
            ymax[partition] = y;
    }

    for(i=0;i<this->nMeshPartitions;i++)
    {
        this->partitionRectangles[i].setCoords(xmin[i],ymax[i],xmax[i],ymin[i]);
        this->partitionRectangles[i].expand(2.0);
        this->partitionRectangles[i].computeArea();
    }

    std::sort(this->partitionRectangles.begin(),this->partitionRectangles.end(),rectangeleAreaLessThan);

    return ERROR_NOERROR;

}
//-----------------------------------------------------------------------------------------//



//-----------------------------------------------------------------------------------------//
//...Method to write the rectangles that have been generated for partitioning
//-----------------------------------------------------------------------------------------//
/**
 * \brief Method to write the rectangles that have been generated for partitioning
 *
 * Method to write the rectangles that have been generated for partitioning
 *
 **/
//-----------------------------------------------------------------------------------------//
int AdVersion::writeRectanglePartitions()
{
    int i;
    qreal x1,x2,y1,y2;
    QString line;
    QFile partitionFile;

    //...Write control file for rectangles
    partitionFile.setFileName(this->systemDir.path()+"/partition.control");
    if(partitionFile.exists())
        partitionFile.remove();
    if(!partitionFile.open(QIODevice::WriteOnly))
        return -1;
    partitionFile.write(QString(QString::number(this->partitionRectangles.length())+"\n").toUtf8());

    //...Write rectangles files for partition boundaries
    for(i=0;i<this->partitionRectangles.length();i++)
    {
        //...Write the rectangle bounding box to the main file
        x1   = this->partitionRectangles[i].bottomLeft().x();
        y1   = this->partitionRectangles[i].bottomLeft().y();
        x2   = this->partitionRectangles[i].topRight().x();
        y2   = this->partitionRectangles[i].topRight().y();
        line.sprintf("%+018.12e %+018.12e %+018.12e %+018.12e \n",x1,y1,x2,y2);
        partitionFile.write(line.toUtf8());
    }
    partitionFile.close();
    return ERROR_NOERROR;
}
//-----------------------------------------------------------------------------------------//



//-----------------------------------------------------------------------------------------//
//...Method to read the rectangles
//-----------------------------------------------------------------------------------------//
/**
 * \brief Method to read the rectangles that have been constructed for partitioning
 *
 * Method to read the rectangles that have been constructed for partitioning
 *
 **/
//-----------------------------------------------------------------------------------------//
int AdVersion::readRectangles()
{
    int i;
    QString line,x1s,x2s,y1s,y2s;
    QStringList tempList;
    QFile partControl;
    qreal x1,y1,x2,y2;

    partControl.setFileName(this->systemDir.path()+"/partition.control");
    if(!partControl.exists())
        return -1;
    if(!partControl.open(QIODevice::ReadOnly))
        return -1;

    this->nMeshPartitions = partControl.readLine().simplified().toInt();
    this->partitionRectangles.resize(this->nMeshPartitions);

    for(i=0;i<this->nMeshPartitions;i++)
    {
        line     = partControl.readLine().simplified();
        tempList = line.split(" ");
        x1s      = tempList.value(0);
        y1s      = tempList.value(1);
        x2s      = tempList.value(2);
        y2s      = tempList.value(3);
        x1       = x1s.toDouble();
        y1       = y1s.toDouble();
        x2       = x2s.toDouble();
        y2       = y2s.toDouble();
        this->partitionRectangles[i].setBottomLeft(QPointF(x1,y1));
        this->partitionRectangles[i].setTopRight(QPointF(x2,y2));
    }
    partControl.close();

    return ERROR_NOERROR;
}
//-----------------------------------------------------------------------------------------//


//-----------------------------------------------------------------------------------------//
//...Method that will partition an input mesh based upont he previously constructed rectangles
//-----------------------------------------------------------------------------------------//
/**
 * \brief Method that will partition an input mesh based upont he previously constructed rectangles
 *
 * Method that will partition an input mesh based upont he previously constructed rectangles
 *
 **/
//-----------------------------------------------------------------------------------------//
int AdVersion::partitionMesh()
{
    int i,j,ierr,nNodeFound,nElementFound;
    qreal x,y,x1,x2,x3,y1,y2,y3;
    QVector<QPointF> elementCenters;
    bool partitioned,expanded;

    //...Read the partitioning
    if(this->partitionRectangles.isEmpty())
        ierr = this->readRectangles();

    //...Create element center locations
    elementCenters.resize(this->mesh->numElements);
    for(i=0;i<this->mesh->numElements;i++)
    {
        x1 = this->mesh->elements[i]->connections[0]->position.x();
        y1 = this->mesh->elements[i]->connections[0]->position.y();
        x2 = this->mesh->elements[i]->connections[1]->position.x();
        y2 = this->mesh->elements[i]->connections[1]->position.y();
        x3 = this->mesh->elements[i]->connections[2]->position.x();
        y3 = this->mesh->elements[i]->connections[2]->position.y();
        x  = (x1+x2+x3)/3.0;
        y  = (y1+y2+y3)/3.0;
        elementCenters[i].setX(x);
        elementCenters[i].setY(y);
    }

    //...This loop runs until all nodes are found by slowly expanding
    //   the search box for each polygon. This takes care of some potential
    //   round off error when searching for which rectangle a node falls in.
    //   Generally, it shouldn't be used much since by default the rectangles
    //   have a 2% expansion to cover this, so this is mostly a failsafe.
    partitioned = false;
    while(!partitioned)
    {

        //...Counters
        nNodeFound = 0;
        nElementFound = 0;

        //...Allocate the containers for each partition
        this->nodeList.clear();
        this->elementList.clear();
        this->nodeList.resize(this->nMeshPartitions);
        this->elementList.resize(this->nMeshPartitions);

        //...Start organizing the nodes into rectangles
        for(i=0;i<this->mesh->numNodes;i++)
        {
            for(j=0;j<this->nMeshPartitions;j++)
            {
                if(this->partitionRectangles[j].contains(this->mesh->nodes[i]->toPointF()))
                {
                    this->nodeList[j].append(this->mesh->nodes[i]);
                    nNodeFound = nNodeFound + 1;
                    break;
                }
            }
        }

        if(nNodeFound!=this->mesh->numNodes)
        {
            expanded = true;

            //...Expand the rectangles by 0.5%. Then
            //   try partitioning again.
            for(i=0;i<this->nMeshPartitions;i++)
                this->partitionRectangles[i].expand(0.5);
            continue;
        }

        //...Organize the elements into rectangles
        for(i=0;i<this->mesh->numElements;i++)
        {
            for(j=0;j<this->nMeshPartitions;j++)
            {
                if(this->partitionRectangles[j].contains(elementCenters[i]))
                {
                    this->elementList[j].append(this->mesh->elements[i]);
                    nElementFound = nElementFound + 1;
                    break;
                }
            }
        }

        if(nElementFound!=this->mesh->numElements)
        {
            expanded = true;

            //...Expand the rectangles by 0.5%. Then
            //   try partitioning again.
            for(i=0;i<this->nMeshPartitions;i++)
                this->partitionRectangles[i].expand(0.5);
            continue;
        }

        //...Success, exit loop
        partitioned = true;
    }

    //...If we needed to expand the partitions,
    //   save them so we don't need to do it again
    if(expanded)
        this->writeRectanglePartitions();

    return ERROR_NOERROR;
}
//-----------------------------------------------------------------------------------------//



//-----------------------------------------------------------------------------------------//
//...Method to set the type of hashing algorithm to be used
//-----------------------------------------------------------------------------------------//
/**
 * \brief Method to set the type of hashing algorithm to be used
 *
 * Method to set the type of hashing algorithm to be used
 *
 **/
//-----------------------------------------------------------------------------------------//
int AdVersion::setHashAlgorithm(QCryptographicHash::Algorithm algorithm)
{
    this->hashAlgorithm = algorithm;
    return ERROR_NOERROR;
}
//-----------------------------------------------------------------------------------------//



//-----------------------------------------------------------------------------------------//
//...Method to renumber the ADCIRC mesh in a semi-logical way
//-----------------------------------------------------------------------------------------//
/**
 * \brief Method to renumber the ADCIRC mesh in a semi-logical way
 *
 * Method to renumber the mesh in a semi-logical way. The METIS NodeND method is used
 * to generate numbering for the nodes. Then, to roughly orient the elemnts in the same way
 * the elements are ordered based upon the sum of the nodes that make them up. See, I told
 * you it was only semi-logical. Boundary conditions are ordered east-->west. This satisfies
 * that the boundary conditions (open/river) will remain in the same order (unless a condition
 * is added) so that the fort.15 is not jumbled by being put through this code.
 *
 **/
//-----------------------------------------------------------------------------------------//
int AdVersion::renumber()
{

    QVector<int> qptr,qind;
    int i,ierr;
    idx_t ne,nn,numflag,*xadj,*adj;
    idx_t nPartitions,ncon;
    idx_t metis_options[METIS_NOPTIONS];

    //...Begin setting things up for metis
    ne = this->mesh->numElements;
    nn = this->mesh->numNodes;
    ncon = 1;
    numflag = 0;
    nPartitions = this->nMeshPartitions;

    qptr.resize(ne+1);

    //...Build the connectivity table for METIS
    for(i=0;i<ne;i++)
    {
        qind.push_back(this->mesh->elements[i]->connections[0]->id-1);
        qind.push_back(this->mesh->elements[i]->connections[1]->id-1);
        qind.push_back(this->mesh->elements[i]->connections[2]->id-1);
        qptr[i] = qind.length()-1-2;
    }
    qptr[ne] = qind.length()-1;

    //...Allocate the arrays for METIS
    idx_t *eptr  = (idx_t*)malloc(qptr.length()*sizeof(idx_t));
    idx_t *eind  = (idx_t*)malloc(qind.length()*sizeof(idx_t));
    idx_t *npart = (idx_t*)malloc(this->mesh->numNodes*sizeof(idx_t));
    idx_t *epart = (idx_t*)malloc(ne*sizeof(idx_t));
    idx_t *perm  = (idx_t*)malloc(nn*sizeof(idx_t));
    idx_t *iperm = (idx_t*)malloc(nn*sizeof(idx_t));

    //...Save the arrays for METIS
    for(i=0;i<qptr.size();i++)
        eptr[i] = qptr[i];

    for(i=0;i<qind.size();i++)
        eind[i] = qind[i];

    //...Call METIS
    ierr = METIS_SetDefaultOptions(metis_options);
    ierr = METIS_MeshToNodal(&ne,&nn,eptr,eind,&numflag,&xadj,&adj);
    ierr = METIS_NodeND(&nn,xadj,adj,NULL,metis_options,perm,iperm);

    //...Renumber the nodes in the mesh object
    for(i=0;i<this->mesh->numNodes;i++)
        this->mesh->nodes[i]->id = (int)iperm[i]+1;

    //...Sort the nodes
    std::sort(this->mesh->nodes.begin(),this->mesh->nodes.end(),nodeNumberLessThan);

    //...Sort the elements
    std::sort(this->mesh->elements.begin(),this->mesh->elements.end(),elementSumLessThan);
    for(i=0;i<this->mesh->numElements;i++)
        this->mesh->elements[i]->id = i+1;

    //...Delete the arrays
    free(eptr);
    free(eind);
    free(npart);
    free(epart);
    free(xadj);
    free(adj);
    free(perm);
    free(iperm);

    return 0;
}
//-----------------------------------------------------------------------------------------//



//-----------------------------------------------------------------------------------------//
//...Method to write the partitioned elements to the repository
//-----------------------------------------------------------------------------------------//
/**
 * \brief Method to write the partitioned nodes to the repository
 *
 * Method to write the partitioned nodes to the repository
 *
 **/
//-----------------------------------------------------------------------------------------//
int AdVersion::writeNodeFiles()
{
    int i,j;
    QString file,fileName,hash;
    QString x, y, z, line;
    QFile thisFile;

    for(i=0;i<this->nMeshPartitions;i++)
    {

        std::sort(this->nodeList[i].begin(),this->nodeList[i].end(),nodeHashLessThan);

        file.sprintf("partition_%4.4i.node",i);
        fileName = this->nodeDir.path()+"/"+file;

        thisFile.setFileName(fileName);

        if(thisFile.exists())
            thisFile.remove();
        if(!thisFile.open(QIODevice::WriteOnly))
            return -1;

        line = QString("%1 \n").arg(this->nodeList[i].length());
        thisFile.write(line.toUtf8());

        for(j=0;j<this->nodeList[i].length();j++)
        {
            hash = this->nodeList[i].value(j)->positionHash;
            x.sprintf("%+018.12e",this->nodeList[i].value(j)->position.x());
            y.sprintf("%+018.12e",this->nodeList[i].value(j)->position.y());
            z.sprintf("%+018.12e",this->nodeList[i].value(j)->position.z());
            line = QString("%1 %2 %3 %4 \n").arg(hash).arg(x).arg(y).arg(z);
            thisFile.write(line.toUtf8());
        }

        thisFile.close();

    }

    return ERROR_NOERROR;

}
//-----------------------------------------------------------------------------------------//



//-----------------------------------------------------------------------------------------//
//...Method to write the partitioned elements to the repository
//-----------------------------------------------------------------------------------------//
/**
 * \brief Method to write the partitioned elements to the repository
 *
 * Method to write the partitioned elements to the repository
 *
 **/
//-----------------------------------------------------------------------------------------//
int AdVersion::writeElementFiles()
{
    int i,j;
    QString file,fileName,line;
    QFile thisFile;

    for(i=0;i<this->nMeshPartitions;i++)
    {

        std::sort(this->elementList[i].begin(),this->elementList[i].end(),elementHashLessThan);

        file.sprintf("partition_%4.4i.element",i);
        fileName = this->elemDir.path()+"/"+file;
        thisFile.setFileName(fileName);
        if(thisFile.exists())
            thisFile.remove();
        if(!thisFile.open(QIODevice::WriteOnly))
            return -1;

        line = QString("%1 \n").arg(this->elementList[i].length());
        thisFile.write(line.toUtf8());

        for(j=0;j<this->elementList[i].length();j++)
        {
            line = QString("%1 %2 %3 %4 \n")
                    .arg(this->elementList[i].value(j)->hash)
                    .arg(this->elementList[i].value(j)->connections[0]->positionHash)
                    .arg(this->elementList[i].value(j)->connections[1]->positionHash)
                    .arg(this->elementList[i].value(j)->connections[2]->positionHash);
            thisFile.write(line.toUtf8());
        }
        thisFile.close();
    }

    return ERROR_NOERROR;
}
//-----------------------------------------------------------------------------------------//



//-----------------------------------------------------------------------------------------//
//...Method to write the boundary conditions to the repository
//-----------------------------------------------------------------------------------------//
/**
 * \brief Method to write boundary conditions to the repository
 *
 * Method to write the boundary conditions to the repository
 *
 **/
//-----------------------------------------------------------------------------------------//
int AdVersion::writeBoundaryFiles()
{
    int i,j;
    QString fileName,line;
    QFile thisFile;
    QVector<adcirc_boundary*> openBCSort,landBCSort;

    //...Create the open and land boundary lists for sorting
    openBCSort.resize(this->mesh->numOpenBoundaries);
    for(i=0;i<this->mesh->numOpenBoundaries;i++)
        openBCSort[i] = this->mesh->openBC[i];

    landBCSort.resize(this->mesh->numLandBoundaries);
    for(i=0;i<this->mesh->numLandBoundaries;i++)
        landBCSort[i] = this->mesh->landBC[i];

    std::sort(openBCSort.begin(),openBCSort.end(),boundaryHashLessThan);
    std::sort(landBCSort.begin(),landBCSort.end(),boundaryHashLessThan);

    //...Write the files containing the list of boundaries
    QFile fileOpenBC(this->openBoundDir.path()+"/open.boundary");
    QFile fileLandBC(this->landBoundDir.path()+"/land.boundary");

    //...Remove if it exists
    if(fileOpenBC.exists())
        fileOpenBC.remove();

    if(fileLandBC.exists())
        fileLandBC.remove();

    //...Write the header (nBoundaries) and hash for each open boundary
    if(!fileOpenBC.open(QIODevice::WriteOnly))
        return -1;
    fileOpenBC.write(QString(QString::number(this->mesh->numOpenBoundaries)+"\n").toUtf8());
    for(i=0;i<this->mesh->numOpenBoundaries;i++)
    {
        line = QString("%1\n").arg(openBCSort[i]->hash);
        fileOpenBC.write(line.toUtf8());
    }
    fileOpenBC.close();

    //...Write the header (nBoundaries) and hash for each land boundary
    if(!fileLandBC.open(QIODevice::WriteOnly))
        return -1;
    fileLandBC.write(QString(QString::number(this->mesh->numLandBoundaries)+"\n").toUtf8());
    for(i=0;i<this->mesh->numLandBoundaries;i++)
    {
        line = QString("%1\n").arg(landBCSort[i]->hash);
        fileLandBC.write(line.toUtf8());
     }
    fileLandBC.close();

    //...Write the open boundary files
    for(i=0;i<this->mesh->numOpenBoundaries;i++)
    {
        //...Create a new file using the hash as the name
        fileName = this->openBoundDir.path()+"/"+this->mesh->openBC[i]->hash+".bnd";
        thisFile.setFileName(fileName);
        if(thisFile.exists())
            thisFile.remove();

        if(!thisFile.open(QIODevice::WriteOnly))
            return -1;

        //...Write the boundary
        line = QString("%1 %2 \n").arg(this->mesh->openBC[i]->hash).arg(this->mesh->openBC[i]->numNodes);
        thisFile.write(line.toUtf8());
        for(j=0;j<this->mesh->openBC[i]->numNodes;j++)
        {
            line = this->formatBoundaryHashLine(this->mesh->openBC[i],j);
            thisFile.write(line.toUtf8());
        }
        thisFile.close();
    }


    //...Write the land boundary files
    for(i=0;i<this->mesh->numLandBoundaries;i++)
    {
        //...Create a new file using the hash as the name
        fileName = this->landBoundDir.path()+"/"+this->mesh->landBC[i]->hash+".bnd";
        thisFile.setFileName(fileName);
        if(thisFile.exists())
            thisFile.remove();

        if(!thisFile.open(QIODevice::WriteOnly))
            return -1;

        //...Write the boundary
        line = QString("%1 %2 %3 \n").arg(this->mesh->landBC[i]->hash)
                                     .arg(this->mesh->landBC[i]->numNodes)
                                     .arg(this->mesh->landBC[i]->code);
        thisFile.write(line.toUtf8());
        for(j=0;j<this->mesh->landBC[i]->numNodes;j++)
        {
            line = this->formatBoundaryHashLine(this->mesh->landBC[i],j);
            thisFile.write(line.toUtf8());
        }
        thisFile.close();
    }

    return ERROR_NOERROR;
}
//-----------------------------------------------------------------------------------------//



//-----------------------------------------------------------------------------------------//
//...Method to write the system files to the repository
//-----------------------------------------------------------------------------------------//
/**
 * \brief Method to write the system files to the repository
 *
 * Method to write the system files to the repository
 *
 **/
//-----------------------------------------------------------------------------------------//
int AdVersion::writeSystemFiles()
{

    QFile thisFile;

    //...Write the mesh header (title, nNodes, nElements)
    thisFile.setFileName(this->systemDir.path()+"/mesh.header");
    if(thisFile.exists())
        thisFile.remove();
    if(!thisFile.open(QIODevice::WriteOnly))
        return -1;

    thisFile.write(QString(this->mesh->title+"\n").toUtf8());
    thisFile.write(QString(QString::number(this->mesh->numNodes)+"\n").toUtf8());
    thisFile.write(QString(QString::number(this->mesh->numElements)+"\n").toUtf8());
    thisFile.write(QString(QString::number(this->mesh->numOpenBoundaries)+"\n").toUtf8());
    thisFile.write(QString(QString::number(this->mesh->numLandBoundaries)+"\n").toUtf8());
    thisFile.close();

    //...Write the type of hashing algorithm used
    thisFile.setFileName(this->systemDir.path()+"/hash.type");
    if(thisFile.exists())
        thisFile.remove();
    if(!thisFile.open(QIODevice::WriteOnly))
        return -1;

    if(this->hashAlgorithm==QCryptographicHash::Sha1)
        thisFile.write("sha1");
    else
        thisFile.write("md5");
    thisFile.close();
}
//-----------------------------------------------------------------------------------------//
