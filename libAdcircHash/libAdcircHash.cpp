#include "libAdcircHash.h"
#include <QDir>
#include <QDebug>

Adcirc_hashlib::Adcirc_hashlib(QObject *parent) : QObject(parent)
{

}

int Adcirc_hashlib::hashAdcircMesh(QString path)
{
    int ierr;
    this->mesh = new adcirc_mesh();

    //qDebug() << "Reading...";
    ierr = this->mesh->read(path);
    if(ierr!=ERROR_NOERROR)
        return -1;

    qDebug() << "Hashing...";
    ierr = this->mesh->hashMesh();
    this->organizeHashes();

    if(ierr!=ERROR_NOERROR)
        return -1;

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

    for(i=0;i<this->nodeHashList.length();i++)
        qDebug() << nodeHashList[i].length() << elementHashList[i].length() << openBCHashList[i].length() << landBCHashList[i].length();

    return 0;
}


int Adcirc_hashlib::buildDirectoryTree(QString directory)
{

    bool err;
    QDir myDir(directory);
    QDir nodeDir(directory+"/nodes");
    QDir elemDir(directory+"/elements");
    QDir boundDir(directory+"/boundaries");

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

    return 0;
}
