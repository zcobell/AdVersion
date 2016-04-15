#include "ahl_adcirc_boundary.h"

ahl_adcirc_boundary::ahl_adcirc_boundary(QObject *parent)
{

}

int ahl_adcirc_boundary::hashBonudary()
{
    QString hashSeed;
    int i;

    QCryptographicHash localHash(QCryptographicHash::Sha1);
    localHash.reset();

    //...Hash the boundary code
    hashSeed.sprintf("%+6i",this->code);
    localHash.addData(hashSeed.toUtf8(),hashSeed.length());

    //...Accumulate the hash along the boundary string
    if( this->code  == 0   ||
        this->code  == 1   ||
        this->code  == 2   ||
        this->code  == 10  ||
        this->code  == 11  ||
        this->code  == 12  ||
        this->code  == 20  ||
        this->code  == 21  ||
        this->code  == 22  ||
        this->code  == 30  ||
        this->code  == 52  ||
        this->code  == 102 ||
        this->code  == 112 ||
        this->code  == 122    )
    {
        for(i=0;i<this->numNodes;i++)
        {
            hashSeed = QString();
            hashSeed = this->n1[i]->positionHash;
            localHash.addData(hashSeed.toUtf8(),hashSeed.length());
        }
    }
    else if( this->code == 3  ||
             this->code == 13 ||
             this->code == 23   )
    {
        for(i=0;i<this->numNodes;i++)
        {
            hashSeed = QString();
            hashSeed = this->n1[i]->positionHash;
            localHash.addData(hashSeed.toUtf8(),hashSeed.length());
            hashSeed = QString();
            hashSeed.sprintf("%+018.12e",this->crest[i]);
            localHash.addData(hashSeed.toUtf8(),hashSeed.length());
            hashSeed = QString();
            hashSeed.sprintf("%+018.12e",this->supercritical[i]);
            localHash.addData(hashSeed.toUtf8(),hashSeed.length());
        }
    }
    else if( this->code == 4 ||
             this->code == 24 )
    {
        for(i=0;i<this->numNodes;i++)
        {
            hashSeed = QString();
            hashSeed = this->n1[i]->positionHash;
            localHash.addData(hashSeed.toUtf8(),hashSeed.length());
            hashSeed = QString();
            hashSeed = this->n2[i]->positionHash;
            localHash.addData(hashSeed.toUtf8(),hashSeed.length());
            hashSeed = QString();
            hashSeed.sprintf("%+018.12e",this->crest[i]);
            localHash.addData(hashSeed.toUtf8(),hashSeed.length());
            hashSeed = QString();
            hashSeed.sprintf("%+018.12e",this->supercritical[i]);
            localHash.addData(hashSeed.toUtf8(),hashSeed.length());
            hashSeed = QString();
            hashSeed.sprintf("%+018.12e",this->subcritical[i]);
            localHash.addData(hashSeed.toUtf8(),hashSeed.length());
        }
    }
    else if( this->code == 5 ||
             this->code == 25 )
    {
        for(i=0;i<this->numNodes;i++)
        {
            hashSeed = QString();
            hashSeed = this->n1[i]->positionHash;
            localHash.addData(hashSeed.toUtf8(),hashSeed.length());
            hashSeed = QString();
            hashSeed = this->n2[i]->positionHash;
            localHash.addData(hashSeed.toUtf8(),hashSeed.length());
            hashSeed = QString();
            hashSeed.sprintf("%+018.12e",this->crest[i]);
            localHash.addData(hashSeed.toUtf8(),hashSeed.length());
            hashSeed = QString();
            hashSeed.sprintf("%+018.12e",this->supercritical[i]);
            localHash.addData(hashSeed.toUtf8(),hashSeed.length());
            hashSeed = QString();
            hashSeed.sprintf("%+018.12e",this->subcritical[i]);
            localHash.addData(hashSeed.toUtf8(),hashSeed.length());
            hashSeed = QString();
            hashSeed.sprintf("%+018.12e",this->pipeDiam[i]);
            localHash.addData(hashSeed.toUtf8(),hashSeed.length());
            hashSeed = QString();
            hashSeed.sprintf("%+018.12e",this->pipeHeight[i]);
            localHash.addData(hashSeed.toUtf8(),hashSeed.length());
            hashSeed = QString();
            hashSeed.sprintf("%+018.12e",this->pipeCoef[i]);
            localHash.addData(hashSeed.toUtf8(),hashSeed.length());
        }
    }

    this->hash = localHash.result().toHex();

    return 0;
}
