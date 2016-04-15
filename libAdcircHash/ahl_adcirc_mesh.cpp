#include "ahl_adcirc_mesh.h"

ahl_adcirc_mesh::ahl_adcirc_mesh(QObject *parent)
{

}

int ahl_adcirc_mesh::hashMesh()
{
    QString hashSeed;
    int i;

    //...initialize the sha1 hash
    QCryptographicHash localHash(QCryptographicHash::Sha1);
    localHash.reset();

    //...Compute the local hashes (nodes, elements, boundaries)
    for(i=0;i<this->numNodes;i++)
        this->nodes[i]->hashNode();

    for(i=0;i<this->numElements;i++)
        this->elements[i]->hashElement();

    for(i=0;i<this->numOpenBoundaries;i++)
        this->openBC[i]->hashBonudary();

    for(i=0;i<this->numLandBoundaries;i++)
        this->landBC[i]->hashBonudary();

    //...Now, build the global mesh hash
    localHash.addData(this->title.toUtf8(),this->title.length());
    for(i=0;i<this->numNodes;i++)
        localHash.addData(this->nodes[i]->fullHash.toUtf8(),this->nodes[i]->fullHash.length());

    for(i=0;i<this->numElements;i++)
        localHash.addData(this->elements[i]->hash.toUtf8(),this->elements[i]->hash.length());

    for(i=0;i<this->numOpenBoundaries;i++)
        localHash.addData(this->openBC[i]->hash.toUtf8(),this->openBC[i]->hash.length());

    for(i=0;this->numLandBoundaries;i++)
        localHash.addData(this->landBC[i]->hash.toUtf8(),this->landBC[i]->hash.length());

    this->hash = localHash.result().toHex();

    return 0;

}
