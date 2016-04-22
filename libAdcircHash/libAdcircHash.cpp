#include "libAdcircHash.h"
#include "metis.h"
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

}

int Adcirc_hashlib::hashAdcircMesh(QString inputFile, QString outputFile)
{
    int ierr;

    this->mesh = new adcirc_mesh();
    ierr = this->mesh->read(inputFile);
    if(ierr!=ERROR_NOERROR)
        return -1;

    ierr = this->mesh->hashMesh();

    ierr = this->partitionMesh(12);

    if(ierr!=ERROR_NOERROR)
        return -1;

    return 0;
}

int Adcirc_hashlib::writeHashedMesh(QString outputFile)
{
    int i,j,k;
    QString fileName,file,line,x,y,z,hash;
    QFile thisFile;

    this->buildDirectoryTree(outputFile);

    int nFiles = qPow(16,this->nSplit);

    //...Write the node files
    for(i=0;i<nFiles;i++)
    {
        file     = QString::number(i,16);
        if(file.length()<this->nSplit)
            file = "0"+file;
        fileName = this->nodeDir.path()+"/"+file;
        thisFile.setFileName(fileName);
        if(!thisFile.open(QIODevice::WriteOnly))
            return -1;

        line = QString("%1 \n").arg(this->nodeHashList[i].length());
        thisFile.write(line.toUtf8());

        for(j=0;j<this->nodeHashList[i].length();j++)
        {
            hash = this->nodeHashList[i].value(j)->positionHash;
            x.sprintf("%+018.12e",this->nodeHashList[i].value(j)->position.x());
            y.sprintf("%+018.12e",this->nodeHashList[i].value(j)->position.y());
            z.sprintf("%+018.12e",this->nodeHashList[i].value(j)->position.z());
            line = QString("%1 %2 %3 %4 \n").arg(hash).arg(x).arg(y).arg(z);
            thisFile.write(line.toUtf8());
        }

        thisFile.close();
    }

    //...Write the element files
    for(i=0;i<nFiles;i++)
    {
        file     = QString::number(i,16);
        if(file.length()<this->nSplit)
            file = "0"+file;
        fileName = this->elemDir.path()+"/"+file;
        thisFile.setFileName(fileName);
        if(!thisFile.open(QIODevice::WriteOnly))
            return -1;

        line = QString("%1 \n").arg(this->elementHashList[i].length());
        thisFile.write(line.toUtf8());

        for(j=0;j<this->elementHashList[i].length();j++)
        {
            line = QString("%1 %2 %3 %4 \n")
                    .arg(this->elementHashList[i].value(j)->hash)
                    .arg(this->elementHashList[i].value(j)->connections[0]->positionHash)
                    .arg(this->elementHashList[i].value(j)->connections[1]->positionHash)
                    .arg(this->elementHashList[i].value(j)->connections[2]->positionHash);
            thisFile.write(line.toUtf8());
        }
        thisFile.close();
    }

    //...Write the open boundary files
    for(i=0;i<nFiles;i++)
    {
        file     = QString::number(i,16);
        if(file.length()<this->nSplit)
            file = "0"+file;
        fileName = this->openBoundDir.path()+"/"+file;
        thisFile.setFileName(fileName);
        if(!thisFile.open(QIODevice::WriteOnly))
            return -1;

        line = QString("%1 \n").arg(this->openBCHashList[i].length());
        thisFile.write(line.toUtf8());

        for(j=0;j<this->openBCHashList[i].length();j++)
        {
            line = QString("%1 %2 \n").arg(this->openBCHashList[i].value(j)->hash).arg(this->openBCHashList[i].value(j)->numNodes);
            thisFile.write(line.toUtf8());
            for(k=0;k<this->openBCHashList[i].value(j)->numNodes;k++)
            {
                line = this->formatBoundaryHashLine(this->openBCHashList[i].value(j),k);
                thisFile.write(line.toUtf8());
            }
        }
        thisFile.close();
    }

    //...Write the land boundary files
    for(i=0;i<nFiles;i++)
    {
        file     = QString::number(i,16);
        if(file.length()<this->nSplit)
            file = "0"+file;
        fileName = this->landBoundDir.path()+"/"+file;
        thisFile.setFileName(fileName);
        if(!thisFile.open(QIODevice::WriteOnly))
            return -1;

        line = QString("%1 \n").arg(this->landBCHashList[i].length());
        thisFile.write(line.toUtf8());

        for(j=0;j<this->landBCHashList[i].length();j++)
        {
            line = QString("%1 %2 \n").arg(this->landBCHashList[i].value(j)->hash).arg(this->landBCHashList[i].value(j)->numNodes);
            thisFile.write(line.toUtf8());
            for(k=0;k<this->landBCHashList[i].value(j)->numNodes;k++)
            {
                line = this->formatBoundaryHashLine(this->landBCHashList[i].value(j),k);
                thisFile.write(line.toUtf8());
            }
        }
        thisFile.close();
    }

    return 0;
}

int Adcirc_hashlib::organizeHashes()
{
    int i,subIndex;
    bool err;
    QString subString;

    this->nodeHashList.resize(qPow(16,this->nSplit));
    this->elementHashList.resize(qPow(16,this->nSplit));
    this->openBCHashList.resize(qPow(16,this->nSplit));
    this->landBCHashList.resize(qPow(16,this->nSplit));

    for(i=0;i<this->mesh->numNodes;i++)
    {
        subString = this->mesh->nodes[i]->positionHash.left(this->nSplit);
        subIndex  = subString.toInt(&err,16);
        this->nodeHashList[subIndex].append(this->mesh->nodes[i]);
    }

    for(i=0;i<this->mesh->numElements;i++)
    {
        subString = this->mesh->elements[i]->hash.left(this->nSplit);
        subIndex  = subString.toInt(&err,16);
        this->elementHashList[subIndex].append(this->mesh->elements[i]);
    }

    for(i=0;i<this->mesh->numOpenBoundaries;i++)
    {
        subString = this->mesh->openBC[i]->hash.left(this->nSplit);
        subIndex  = subString.toInt(&err,16);
        this->openBCHashList[subIndex].append(this->mesh->openBC[i]);
    }

    for(i=0;i<this->mesh->numLandBoundaries;i++)
    {
        subString = this->mesh->landBC[i]->hash.left(this->nSplit);
        subIndex  = subString.toInt(&err,16);
        this->landBCHashList[subIndex].append(this->mesh->landBC[i]);
    }

    for(i=0;i<qPow(16,this->nSplit);i++)
    {
        std::sort(this->nodeHashList[i].begin(),this->nodeHashList[i].end(),nodeHashLessThan);
        std::sort(this->elementHashList[i].begin(),this->elementHashList[i].end(),elementHashLessThan);
        std::sort(this->openBCHashList[i].begin(),this->openBCHashList[i].end(),boundaryHashLessThan);
        std::sort(this->landBCHashList[i].begin(),this->landBCHashList[i].end(),boundaryHashLessThan);
    }

    return 0;
}

int Adcirc_hashlib::buildDirectoryTree(QString directory)
{

    bool err;

    this->myDir.setPath(directory);
    this->nodeDir.setPath(directory+"/nodes");
    this->elemDir.setPath(directory+"/elements");
    this->boundDir.setPath(directory+"/boundaries");
    this->openBoundDir.setPath(directory+"/boundaries/openBoundaries");
    this->landBoundDir.setPath(directory+"/boundaries/landBoundaries");

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

    return 0;
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
    idx_t ne,nn,ncommon,numflag,*xadj,*adj;
    idx_t nEdgeCut,nPartitions;
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
    ncommon = 1;
    numflag = 0;
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

    //...Delete the arrays
    delete eptr;
    delete eind;
    delete npart;
    delete epart;
    delete xadj;
    delete adj;

    return 0;

}

int Adcirc_hashlib::partitionMesh(int numPartitions)
{

    int ierr;
    ierr = this->metisPartition(numPartitions);
    ierr = this->buildPolygons(numPartitions);

    return 0;

}

int Adcirc_hashlib::buildPolygons(int numPartitions)
{

    int i,j,k;
    int nNode,nElements,index;
    adcirc_mesh *tempMesh;

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

    }

}
