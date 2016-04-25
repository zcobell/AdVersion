//-----GPL----------------------------------------------------------------------
//
// This file is part of libAdVersion
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
//  File: libAdVersion.cpp
//
//------------------------------------------------------------------------------
#include "libAdVersion.h"
#include "metis.h"
#include <float.h>
#include <QtMath>
#include <QStringList>
#include <QDebug>

bool nodeHashLessThan(const adcirc_node *node1, const adcirc_node *node2)
{
    if(node1->positionHash<node2->positionHash)
        return true;
    else
        return false;
}



bool elementHashLessThan(const adcirc_element *element1, const adcirc_element *element2)
{
    if(element1->hash<element2->hash)
        return true;
    else
        return false;
}



bool boundaryHashLessThan(const adcirc_boundary *boundary1, const adcirc_boundary *boundary2)
{
    if(boundary1->hash<boundary2->hash)
        return true;
    else
        return false;
}



bool rectangeleAreaLessThan(const Rectangle rectangle1, const Rectangle rectangle2)
{
    if(rectangle1.area<rectangle2.area)
        return true;
    else
        return false;
}



AdVersion::AdVersion(QObject *parent) : QObject(parent)
{
    this->mesh = NULL;
}



int AdVersion::createPartitions(QString meshFile, QString outputFile, int numPartitions)
{

    int ierr;

    this->nMeshPartitions = numPartitions;

    //...Get the adcirc mesh that will be partitioned
    this->mesh = new adcirc_mesh(this);
    ierr = this->mesh->read(meshFile);
    if(ierr!=ERROR_NOERROR)
        return -1;

    //...Build the directory tree (if necessary)
    ierr = this->generateDirectoryNames(outputFile);
    if(ierr!=ERROR_NOERROR)
        return -1;

    ierr = this->buildDirectoryTree(outputFile);
    if(ierr!=ERROR_NOERROR)
        return -1;

    //...Use METIS to partition the mesh
    ierr = this->metisPartition();
    if(ierr!=ERROR_NOERROR)
        return -1;

    //...Build the polygons from the METIS partition
    ierr = this->buildPolygons();
    if(ierr!=ERROR_NOERROR)
        return -1;

    //...Write the partitions to the polygon files
    ierr = this->writePolygonPartitions();
    if(ierr!=ERROR_NOERROR)
        return -1;

    return ERROR_NOERROR;

}



int AdVersion::writePartitionedMesh(QString meshFile, QString outputFile)
{
    int ierr,i,j;
    QString fileName,file,line,x,y,z,hash;
    QFile thisFile;
    QVector<adcirc_boundary*> openBCSort,landBCSort;

    //...Create a new adcirc_mesh
    this->mesh = new adcirc_mesh(this);

    //...Read the mesh file in ADCIRC format
    ierr = this->mesh->read(meshFile);
    if(ierr!=ERROR_NOERROR)
        return -1;

    //...Compute the SHA1 hashes for the mesh
    ierr = this->mesh->hashMesh();
    if(ierr!=ERROR_NOERROR)
        return -1;

    //...Build the directory tree for writing
    ierr = this->buildDirectoryTree(outputFile);

    //...Partition the nodes to the subdomains
    ierr = this->partitionMesh();

    //...Write the node files
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

    //...Write the element files
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
        line = QString("%1 %2 \n").arg(this->mesh->landBC[i]->hash).arg(this->mesh->landBC[i]->numNodes);
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



int AdVersion::hashAdcircMesh(QString inputFile)
{
    int ierr;

    this->mesh = new adcirc_mesh();
    ierr = this->mesh->read(inputFile);
    if(ierr!=ERROR_NOERROR)
        return -1;

    ierr = this->mesh->hashMesh();

    if(ierr!=ERROR_NOERROR)
        return -1;

    return ERROR_NOERROR;
}



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
        this->removeDirectory(this->nodeDir.path());
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
        this->removeDirectory(this->elemDir.path());
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
        this->removeDirectory(this->openBoundDir.path());
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
        this->removeDirectory(this->landBoundDir.path());
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



int AdVersion::buildPolygons()
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



int AdVersion::writePolygonPartitions()
{
    int i;
    qreal x1,x2,y1,y2;
    QString line;
    QFile partitionFile;

    //...Write control file for polygons
    partitionFile.setFileName(this->systemDir.path()+"/partition.control");
    if(partitionFile.exists())
        partitionFile.remove();
    if(!partitionFile.open(QIODevice::WriteOnly))
        return -1;
    partitionFile.write(QString(QString::number(this->partitionRectangles.length())+"\n").toUtf8());

    //...Write polygon files for partition boundaries
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



bool AdVersion::removeDirectory(const QString &dirName)
{
    bool result = true;
    QDir dir(dirName);

    if (dir.exists(dirName)) {
        Q_FOREACH(QFileInfo info, dir.entryInfoList(QDir::NoDotAndDotDot | QDir::System | QDir::Hidden  | QDir::AllDirs | QDir::Files, QDir::DirsFirst)) {
            if (info.isDir()) {
                result = removeDirectory(info.absoluteFilePath());
            }
            else {
                result = QFile::remove(info.absoluteFilePath());
            }

            if (!result) {
                return result;
            }
        }
        result = dir.rmdir(dirName);
    }
    return result;
}



int AdVersion::deletePolygons()
{
    int i;
    QString fileNumber;
    QFile partControl,partFile;
    bool err;

    partControl.setFileName(this->systemDir.path()+"/partition.control");
    if(!partControl.open(QIODevice::ReadOnly))
        return -1;

    this->nMeshPartitions = partControl.readLine().simplified().toInt();
    for(i=0;i<this->nMeshPartitions;i++)
    {
        fileNumber.sprintf("%4.4i",i);
        partFile.setFileName(this->systemDir.path()+"/partition_"+fileNumber+".pol");
        err = partFile.remove();
        if(!err)
            return -1;
    }
    return ERROR_NOERROR;
}



int AdVersion::readPolygons()
{
    int i;
    QString line,x1s,x2s,y1s,y2s;
    QStringList tempList;
    QFile partControl;
    qreal x1,y1,x2,y2;

    partControl.setFileName(this->systemDir.path()+"/partition.control");
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



int AdVersion::partitionMesh()
{
    int i,j,ierr,nNodeFound,nElementFound;
    qreal x,y,x1,x2,x3,y1,y2,y3;
    QVector<QPointF> elementCenters;
    bool partitioned,expanded;

    //...Read the partitioning
    ierr = this->readPolygons();

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

        //...Start organizing the nodes into polygons
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

        //...Organize the elements into polygons
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
        this->writePolygonPartitions();

    return ERROR_NOERROR;
}
