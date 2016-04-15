#include "ahl_adcirc_element.h"

ahl_adcirc_element::ahl_adcirc_element(QObject *parent)
{

}

int ahl_adcirc_element::hashElement()
{

    QString hashSeed,hashSeed1,hashSeed2,hashSeed3;

    //...initialize the sha1 hash
    QCryptographicHash localHash(QCryptographicHash::Sha1);
    localHash.reset();

    //...Create a formatted string for each vertex as a product
    //   of each location hash
    hashSeed1 = QString();
    hashSeed2 = QString();
    hashSeed3 = QString();

    //...Grab the node hashes to be the seeds for the element.
    //   Check if the node has been hashed previously and if not
    //   send it to the hashing function
    if(this->connections[0]->positionHash==QString())
        this->connections[0]->hashNode();
    if(this->connections[1]->positionHash==QString())
        this->connections[1]->hashNode();
    if(this->connections[2]->positionHash==QString())
        this->connections[2]->hashNode();

    hashSeed1 = this->connections[0]->positionHash;
    hashSeed2 = this->connections[1]->positionHash;
    hashSeed3 = this->connections[2]->positionHash;

    //...Concatenate the formatted strings together
    hashSeed = QString();
    hashSeed = hashSeed1+hashSeed2+hashSeed3;

    //...Create the hash for the local node
    localHash.addData(hashSeed.toUtf8(),hashSeed.length());

    //...save the element hash
    this->hash = localHash.result().toHex();

    return 0;
}
