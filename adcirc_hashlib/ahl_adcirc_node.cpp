#include "ahl_adcirc_node.h"

ahl_adcirc_node::ahl_adcirc_node(QObject *parent)
{

}

int ahl_adcirc_node::hashNode()
{

    QString hashSeed1,hashSeed2,hashSeed3;

    //...initialize the sha1 hash
    QCryptographicHash localHash(QCryptographicHash::Sha1);
    localHash.reset();

    //...Create a formatted string for each x, y and z;
    hashSeed1 = QString();
    hashSeed2 = QString();
    hashSeed3 = QString();
    hashSeed1.sprintf("%+018.12e",this->position.x());
    hashSeed2.sprintf("%+018.12e",this->position.y());
    hashSeed3.sprintf("%+018.12e",this->position.z());

    //...Create the hash for the local node
    localHash.addData(hashSeed1.toUtf8(),hashSeed1.length());
    localHash.addData(hashSeed2.toUtf8(),hashSeed2.length());

    //...save the x,y node hash
    this->positionHash = localHash.result().toHex();

    //...Save the hash that includes the nodal elevation
    localHash.addData(hashSeed3.toUtf8(),hashSeed3.length());
    this->fullHash = localHash.result().toHex();

    return 0;
}


