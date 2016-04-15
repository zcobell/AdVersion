#include "libAdcircHash.h"
#include <QDir>
#include <QDebug>

Adcirc_hashlib::Adcirc_hashlib(QObject *parent)
{
}

int Adcirc_hashlib::hashAdcircMesh(QString path)
{
    int ierr;
    this->mesh = new ahl_adcirc_mesh(this);
    ierr = this->mesh->read(path);
    this->mesh->hashMesh();
    //this->organizeHashes();

    if(ierr!=ERROR_NOERROR)
        return -1;

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
