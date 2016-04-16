#include "libAdcircHash.h"
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
    this->organizeHashes();
    this->writeHashedMesh(outputFile);

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
                                        .arg(super)
                                        .arg(sub);
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
                                                 .arg(super)
                                                 .arg(sub)
                                                 .arg(pipeht)
                                                 .arg(pipediam)
                                                 .arg(pipecoef);
    }
    else
        line = QString("%1 \n").arg(boundary->n1[index]->positionHash);

    return line;
}
