#include "libAdcircHash.h"
#include "metis.h"
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



Adcirc_hashlib::Adcirc_hashlib(QObject *parent) : QObject(parent)
{
    this->mesh = NULL;
}



int Adcirc_hashlib::createPartitions(QString meshFile, QString outputFile, int numPartitions)
{

    int ierr;

    //...Get the adcirc mesh that will be partitioned
    this->mesh = new adcirc_mesh(this);
    ierr = this->mesh->read(meshFile);
    if(ierr!=ERROR_NOERROR)
        return -1;

    //...Build the directory tree (if necessary)
    ierr = this->generateDirectoryNames(outputFile);
    if(this->partitionDir.exists())
        this->deletePolygons();
    ierr = this->buildDirectoryTree(outputFile);
    if(ierr!=ERROR_NOERROR)
        return -1;

    //...Use METIS to partition the mesh
    ierr = this->metisPartition(numPartitions);
    if(ierr!=ERROR_NOERROR)
        return -1;

    //...Build the polygons from the METIS partition
    ierr = this->buildPolygons(numPartitions);
    if(ierr!=ERROR_NOERROR)
        return -1;

    //...Write the partitions to the polygon files
    ierr = this->writePolygonPartitions();
    if(ierr!=ERROR_NOERROR)
        return -1;

    return ERROR_NOERROR;

}



int Adcirc_hashlib::writePartitionedMesh(QString meshFile, QString outputFile)
{
    int ierr,i,j,k;
    QString fileName,file,line,x,y,z,hash;
    QFile thisFile;

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

//    //...Write the open boundary files
//    for(i=0;i<nFiles;i++)
//    {
//        file     = QString::number(i,16);
//        if(file.length()<this->nSplit)
//            file = "0"+file;
//        fileName = this->openBoundDir.path()+"/"+file;
//        thisFile.setFileName(fileName);
//        if(!thisFile.open(QIODevice::WriteOnly))
//            return -1;

//        line = QString("%1 \n").arg(this->openBCHashList[i].length());
//        thisFile.write(line.toUtf8());

//        for(j=0;j<this->openBCHashList[i].length();j++)
//        {
//            line = QString("%1 %2 \n").arg(this->openBCHashList[i].value(j)->hash).arg(this->openBCHashList[i].value(j)->numNodes);
//            thisFile.write(line.toUtf8());
//            for(k=0;k<this->openBCHashList[i].value(j)->numNodes;k++)
//            {
//                line = this->formatBoundaryHashLine(this->openBCHashList[i].value(j),k);
//                thisFile.write(line.toUtf8());
//            }
//        }
//        thisFile.close();
//    }

//    //...Write the land boundary files
//    for(i=0;i<nFiles;i++)
//    {
//        file     = QString::number(i,16);
//        if(file.length()<this->nSplit)
//            file = "0"+file;
//        fileName = this->landBoundDir.path()+"/"+file;
//        thisFile.setFileName(fileName);
//        if(!thisFile.open(QIODevice::WriteOnly))
//            return -1;

//        line = QString("%1 \n").arg(this->landBCHashList[i].length());
//        thisFile.write(line.toUtf8());

//        for(j=0;j<this->landBCHashList[i].length();j++)
//        {
//            line = QString("%1 %2 \n").arg(this->landBCHashList[i].value(j)->hash).arg(this->landBCHashList[i].value(j)->numNodes);
//            thisFile.write(line.toUtf8());
//            for(k=0;k<this->landBCHashList[i].value(j)->numNodes;k++)
//            {
//                line = this->formatBoundaryHashLine(this->landBCHashList[i].value(j),k);
//                thisFile.write(line.toUtf8());
//            }
//        }
//        thisFile.close();
//    }

    return ERROR_NOERROR;
}



int Adcirc_hashlib::hashAdcircMesh(QString inputFile, QString outputFile)
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



int Adcirc_hashlib::organizeHashes()
{
    int i,subIndex;
    bool err;
    QString subString;

//    this->nodeHashList.resize(qPow(16,this->nSplit));
//    this->elementHashList.resize(qPow(16,this->nSplit));
//    this->openBCHashList.resize(qPow(16,this->nSplit));
//    this->landBCHashList.resize(qPow(16,this->nSplit));

//    for(i=0;i<this->mesh->numNodes;i++)
//    {
//        subString = this->mesh->nodes[i]->positionHash.left(this->nSplit);
//        subIndex  = subString.toInt(&err,16);
//        this->nodeHashList[subIndex].append(this->mesh->nodes[i]);
//    }

//    for(i=0;i<this->mesh->numElements;i++)
//    {
//        subString = this->mesh->elements[i]->hash.left(this->nSplit);
//        subIndex  = subString.toInt(&err,16);
//        this->elementHashList[subIndex].append(this->mesh->elements[i]);
//    }

//    for(i=0;i<this->mesh->numOpenBoundaries;i++)
//    {
//        subString = this->mesh->openBC[i]->hash.left(this->nSplit);
//        subIndex  = subString.toInt(&err,16);
//        this->openBCHashList[subIndex].append(this->mesh->openBC[i]);
//    }

//    for(i=0;i<this->mesh->numLandBoundaries;i++)
//    {
//        subString = this->mesh->landBC[i]->hash.left(this->nSplit);
//        subIndex  = subString.toInt(&err,16);
//        this->landBCHashList[subIndex].append(this->mesh->landBC[i]);
//    }

//    for(i=0;i<qPow(16,this->nSplit);i++)
//    {
//        std::sort(this->nodeHashList[i].begin(),this->nodeHashList[i].end(),nodeHashLessThan);
//        std::sort(this->elementHashList[i].begin(),this->elementHashList[i].end(),elementHashLessThan);
//        std::sort(this->openBCHashList[i].begin(),this->openBCHashList[i].end(),boundaryHashLessThan);
//        std::sort(this->landBCHashList[i].begin(),this->landBCHashList[i].end(),boundaryHashLessThan);
//    }

    return ERROR_NOERROR;
}



int Adcirc_hashlib::generateDirectoryNames(QString directory)
{
    this->myDir.setPath(directory);
    this->nodeDir.setPath(directory+"/nodes");
    this->elemDir.setPath(directory+"/elements");
    this->boundDir.setPath(directory+"/boundaries");
    this->openBoundDir.setPath(directory+"/boundaries/openBoundaries");
    this->landBoundDir.setPath(directory+"/boundaries/landBoundaries");
    this->systemDir.setPath(directory+"/system");
    this->partitionDir.setPath(directory+"/system/partitions");
    return ERROR_NOERROR;
}



int Adcirc_hashlib::buildDirectoryTree(QString directory)
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

    if(!elemDir.exists())
    {
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

    if(!landBoundDir.exists())
    {
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

    if(!partitionDir.exists())
    {
        err = QDir().mkdir(partitionDir.path());
        if(!err)
            return -1;
    }

    return ERROR_NOERROR;
}



QString Adcirc_hashlib::formatBoundaryHashLine(adcirc_boundary *boundary, int index)
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



int Adcirc_hashlib::metisPartition(int numPartitions)
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
    nPartitions = numPartitions;

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


//    QFile checkOutput("/home/zcobell/Development/adcirc_hashlib/fort.63");
//    checkOutput.open(QIODevice::WriteOnly);
//    checkOutput.write(QString("Partitions from METIS \n").toUtf8());
//    checkOutput.write(QString("1 "+QString::number(this->mesh->numNodes)+" -0.9999900E+005  -99999  1 FileFmtVersion: 12345 \n").toUtf8());
//    checkOutput.write(QString("10000.0000 10000 \n").toUtf8());
//    for(i=0;i<this->mesh->numNodes;i++)
//        checkOutput.write(QString(QString::number(i+1)+" "+QString::number(nodePartitionList[i])+"\n").toUtf8());
//    checkOutput.close();


    //...Delete the arrays
    free(eptr);
    free(eind);
    free(npart);
    free(epart);
    free(xadj);
    free(adj);

    return ERROR_NOERROR;

}



int Adcirc_hashlib::buildPolygons(int numPartitions)
{

    int i,j,k,nid1,nid2,previousNode;
    int nNode,nElements,index,startNode;
    adcirc_mesh *tempMesh;
    QString e1,e2,e3;
    QStringList edgeList,edgeListDuplicates;
    QVector<QVector<int> > edges;
    QVector<bool> edgeUsed;
    bool atEnd;

    this->polygons.resize(numPartitions);

    for(i=0;i<numPartitions;i++)
    {

        //...Count the number of nodes and elements partitioned to this subdomain
        nNode = 0;
        nElements = 0;
        for(j=0;j<this->nodePartitionList.length();j++)
            if(this->nodePartitionList[j]==i)
                nNode = nNode+1;

        for(j=0;j<this->elementPartitionList.length();j++)
            if(this->elementPartitionList[j]==i)
                nElements = nElements+1;

        //...Build a mesh based upon the partition
        tempMesh = new adcirc_mesh(this);
        tempMesh->numNodes = nNode;
        tempMesh->numElements = nElements;
        tempMesh->nodes.resize(tempMesh->numNodes);
        tempMesh->elements.resize(tempMesh->numElements);
        for(j=0;j<tempMesh->numNodes;j++)
            tempMesh->nodes[i] = new adcirc_node(this);
        for(j=0;j<tempMesh->numElements;j++)
            tempMesh->elements[i] = new adcirc_element(this);

        index = 0;
        for(j=0;j<this->nodePartitionList.length();j++)
        {
            if(this->nodePartitionList[j]==i)
            {
                tempMesh->nodes[index] = this->mesh->nodes[j];
                index = index + 1;
            }
        }

        index = 0;
        for(j=0;j<this->elementPartitionList.length();j++)
        {
            if(this->elementPartitionList[j]==i)
            {
                tempMesh->elements[index] = this->continuousElementTable[j];
                index = index + 1;
            }
        }

        //...Get list of the boundary edges and create list of edges
        //   contained by two elements so we can find the boundary
        edgeList.clear();
        edgeListDuplicates.clear();
        for(j=0;j<tempMesh->numElements;j++)
        {
            e1   = QString::number(qMin(tempMesh->elements[j]->connections[0]->id,tempMesh->elements[j]->connections[1]->id))+"--"+
                   QString::number(qMax(tempMesh->elements[j]->connections[0]->id,tempMesh->elements[j]->connections[1]->id));
            e2   = QString::number(qMin(tempMesh->elements[j]->connections[1]->id,tempMesh->elements[j]->connections[2]->id))+"--"+
                   QString::number(qMax(tempMesh->elements[j]->connections[1]->id,tempMesh->elements[j]->connections[2]->id));
            e3   = QString::number(qMin(tempMesh->elements[j]->connections[2]->id,tempMesh->elements[j]->connections[0]->id))+"--"+
                   QString::number(qMax(tempMesh->elements[j]->connections[2]->id,tempMesh->elements[j]->connections[0]->id));
            edgeList.append(e1);
            edgeList.append(e2);
            edgeList.append(e3);

        }

        //...Remove the edges used more than once, hence they
        //   are not boundary edges and not part of our outline
        //   polygon
        for(j=0;j<edgeList.length();j++)
        {
            if(edgeListDuplicates.contains(edgeList.at(j)))
                edgeListDuplicates.removeOne(edgeList.at(j));
            else
                edgeListDuplicates.append(edgeList.at(j));
        }

        //...Break the edge list out to integer lists
        edges.resize(2);
        edges[0].resize(edgeList.length());
        edges[1].resize(edgeList.length());
        edgeUsed.resize(edgeList.length());
        edgeUsed.fill(false);
        for(j=0;j<edgeList.length();j++)
        {
            e1   = edgeList.value(j);
            e2   = e1.split("--").value(0);
            nid1 = e2.toInt();
            e3   = e1.split("--").value(1);
            nid2 = e3.toInt();
            edges[0][j] = nid1;
            edges[1][j] = nid2;
        }

        //...Begin assembling a closed loop polygon
        this->polygons[i].append(this->mesh->nodes[edges[0][0]-1]->toPointF());
        edgeUsed[0] = true;
        startNode = this->mesh->nodes[edges[0][0]-1]->id;
        previousNode = startNode;
        atEnd = false;
        while(!atEnd)
        {
            for(j=0;j<edges[0].length();j++)
            {
                if(!edgeUsed[j])
                {
                    nid1 = edges[0][j];
                    nid2 = edges[1][j];
                    if(nid1==previousNode)
                    {
                        this->polygons[i].append(this->mesh->nodes[nid2-1]->toPointF());
                        previousNode = this->mesh->nodes[nid2-1]->id;
                        edgeUsed[j] = true;
                        break;
                    }
                    else if(nid2==previousNode)
                    {
                        this->polygons[i].append(this->mesh->nodes[nid1-1]->toPointF());
                        previousNode = this->mesh->nodes[nid1-1]->id;
                        edgeUsed[j] = true;
                        break;
                    }
                }
                if(j==edges[0].length()-1)
                    atEnd = true;
            }
        }

        edges[0].clear();
        edges[1].clear();
        edgeUsed.clear();

        //...Close the polygon
        this->polygons[i].append(this->polygons[i].at(0));

        //...Free up memory
        delete tempMesh;

    }

    return ERROR_NOERROR;

}



int Adcirc_hashlib::writePolygonPartitions()
{
    int i,j;
    double x1,x2,y1,y2;
    QString line,filename;
    QFile partitionFile,polygonFile;

    //...Write control file for polygons
    partitionFile.setFileName(this->partitionDir.path()+"/partition.control");
    if(!partitionFile.open(QIODevice::WriteOnly))
        return -1;
    partitionFile.write(QString(QString::number(this->polygons.length())+"\n").toUtf8());

    //...Convert the polygons to rectangles
    this->rect.resize(this->polygons.length());
    for(i=0;i<this->polygons.length();i++)
        this->rect[i] = this->polygons[i].boundingRect();

    //...Write polygon files for partition boundaries
    for(i=0;i<this->rect.length();i++)
    {
        //...Write the rectangle bounding box to the main file
        x1   = this->rect[i].bottomLeft().x();
        y1   = this->rect[i].bottomLeft().y();
        x2   = this->rect[i].topRight().x();
        y2   = this->rect[i].topRight().y();
        line.sprintf("%+018.12e %+018.12e %+018.12e %+018.12e \n",x1,y1,x2,y2);
        partitionFile.write(line.toUtf8());

        //...Write the polygon boundary to its own
        filename.sprintf("partition_%4.4i.pol",i);
        filename = this->partitionDir.path()+"/"+filename;
        polygonFile.setFileName(filename);
        if(!polygonFile.open(QIODevice::WriteOnly))
            return -1;

        polygonFile.write(QString(QString::number(this->polygons[i].length())+"\n").toUtf8());
        for(j=0;j<this->polygons[i].length();j++)
        {
            line.sprintf("%+018.12e %+018.12e \n",this->polygons[i].at(j).x(),this->polygons[i].at(j).y());
            polygonFile.write(QString(line).toUtf8());
        }
        polygonFile.close();
    }
    partitionFile.close();
    return ERROR_NOERROR;
}



bool Adcirc_hashlib::removeDirectory(const QString &dirName)
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



int Adcirc_hashlib::deletePolygons()
{
    int i;
    QString fileNumber;
    QFile partControl,partFile;
    bool err;

    partControl.setFileName(this->partitionDir.path()+"/partition.control");
    if(!partControl.open(QIODevice::ReadOnly))
        return -1;

    this->nMeshPartitions = partControl.readLine().simplified().toInt();
    for(i=0;i<this->nMeshPartitions;i++)
    {
        fileNumber.sprintf("%4.4i",i);
        partFile.setFileName(this->partitionDir.path()+"/partition_"+fileNumber+".pol");
        err = partFile.remove();
        if(!err)
            return -1;
    }
    return ERROR_NOERROR;
}



int Adcirc_hashlib::readPolygons()
{
    int i,j,nPoints;
    QString line,x1s,x2s,y1s,y2s,filename;
    QStringList tempList;
    QFile partControl,polygonFile;
    double x1,y1,x2,y2;

    partControl.setFileName(this->partitionDir.path()+"/partition.control");
    if(!partControl.open(QIODevice::ReadOnly))
        return -1;

    this->nMeshPartitions = partControl.readLine().simplified().toInt();
    this->rect.resize(this->nMeshPartitions);
    this->polygons.resize(this->nMeshPartitions);

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
        this->rect[i].setBottomLeft(QPointF(x1,y1));
        this->rect[i].setTopRight(QPointF(x2,y2));

        filename.sprintf("partition_%4.4i.pol",i);
        filename = this->partitionDir.path()+"/"+filename;
        polygonFile.setFileName(filename);

        if(!polygonFile.open(QIODevice::ReadOnly))
            return -1;

        nPoints = polygonFile.readLine().simplified().toInt();

        for(j=0;j<nPoints;j++)
        {
            line     = polygonFile.readLine().simplified();
            tempList = line.split(" ");
            x1s      = tempList.value(0);
            y1s      = tempList.value(1);
            x1       = x1s.toDouble();
            y1       = y1s.toDouble();
            this->polygons[i].append(QPointF(x1,y1));
        }

        if(!this->polygons[i].isClosed())
        {
            qDebug() << "ERROR: Polygon not closed.";
            return -1;
        }
        polygonFile.close();
    }
    partControl.close();

    return ERROR_NOERROR;
}



int Adcirc_hashlib::partitionMesh()
{
    int i,j,ierr,nNodesFound,nElementsFound,nearestPartition;
    int nSearch;
    double x,y,x1,x2,x3,y1,y2,y3;
    QVector<QPointF> elementCenters,polygonCenters;
    QVector<bool> nodeFound,elementFound;
    QVector<int> partitionRank;
    qKdtree2 partitionLocator;
    bool found;

    nNodesFound = 0;
    nElementsFound = 0;

    nodeFound.resize(this->mesh->numNodes);
    elementFound.resize(this->mesh->numElements);
    nodeFound.fill(false);
    elementFound.fill(false);

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

    //...Create the polygon center locations
    //   This is used as a backup for nodes not quickly found using ray tracing
    polygonCenters.resize(this->nMeshPartitions);
    for(i=0;i<this->nMeshPartitions;i++)
        polygonCenters[i] = this->rect[i].center();

    //...Build kd-tree of polygon centers
    partitionLocator.build(polygonCenters);

    //...Allocate the containers for each partition
    this->nodeList.resize(this->nMeshPartitions);
    this->elementList.resize(this->nMeshPartitions);

    nSearch = qMin(10,this->nMeshPartitions);
    ierr = this->polygonPrecalc();

    //...Start organizing the nodes into polygons
    qDebug() << "Starting nodes...";
    for(i=0;i<this->mesh->numNodes;i++)
    {
        partitionRank.clear();
        partitionLocator.findXNearest(this->mesh->nodes[i]->toPointF(),nSearch,partitionRank);
        found = false;
        for(j=0;j<nSearch;j++)
        {
            if(this->pointInPolygon(this->mesh->nodes[i]->toPointF(),partitionRank[j]))
            {
                found = true;
                this->nodeList[partitionRank[j]].append(this->mesh->nodes[i]);
                break;
            }
        }
        if(!found)
            this->nodeList[partitionRank[0]].append(this->mesh->nodes[i]);
    }

    //...Organize the elements into polygons
    for(i=0;i<this->mesh->numElements;i++)
    {
        partitionRank.clear();
        partitionLocator.findXNearest(elementCenters[i],nSearch,partitionRank);
        found = false;
//        for(j=0;j<nSearch;j++)
//        {
//            if(this->polygons[partitionRank[j]].containsPoint(elementCenters[i],Qt::OddEvenFill))
//            {
//                found = true;
//                this->elementList[partitionRank[j]].append(this->mesh->elements[i]);
//                break;
//            }
//        }
//        if(!found)
            this->elementList[partitionRank[0]].append(this->mesh->elements[i]);
    }

    return ERROR_NOERROR;
}



bool Adcirc_hashlib::pointInPolygon(QPointF point,int polygonIndex)
{
    int i,j,polyCorners;
    double x,y,x1,x2,y1,y2;
    bool oddNodes;

    polyCorners = this->polygons[polygonIndex].length();
    j = polyCorners-1;

    x = point.x();
    y = point.y();

    for (i=0; i<polyCorners; i++)
    {
        x1 = this->polygons[polygonIndex].at(i).x();
        y1 = this->polygons[polygonIndex].at(j).y();
        x2 = this->polygons[polygonIndex].at(i).x();
        y2 = this->polygons[polygonIndex].at(j).y();
        if ((y1 < y && y2 >= y || y2 < y && y1 >= y))
            oddNodes^=(y*this->polygonMultiples[polygonIndex][i]+this->polygonConstant[polygonIndex][i]<x);
        j=i;
    }

    return oddNodes;
}



int Adcirc_hashlib::polygonPrecalc()
{
    int i,j,k;
    double x1,x2,y1,y2;

    this->polygonConstant.resize(this->nMeshPartitions);
    this->polygonMultiples.resize(this->nMeshPartitions);

    for(i=0;i<this->nMeshPartitions;i++)
    {
        this->polygonConstant[i].resize(this->polygons[i].length());
        this->polygonMultiples[i].resize(this->polygons[i].length());
    }

    for(k=0;k<this->nMeshPartitions;k++)
    {
        j = this->polygons[k].length()-1;
        for(i=0;i<this->polygons[k].length();i++)
        {
            x1 = this->polygons[k].at(i).x();
            y1 = this->polygons[k].at(j).y();
            x2 = this->polygons[k].at(i).x();
            y2 = this->polygons[k].at(j).y();

            if(y2==y1)
            {
                this->polygonConstant[k][i] = x1;
                this->polygonMultiples[k][i] = 0.0;
            }
            else
            {
                this->polygonConstant[k][i]  = x1-(y1*x2)/(y2-y1)+(y1*x1)/(y2-y1);
                this->polygonMultiples[k][i] = (x2-x1)/(y2-y1);
            }
            j=i;
        }

    }
    return ERROR_NOERROR;
}
