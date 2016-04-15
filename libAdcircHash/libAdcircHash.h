#ifndef ADCIRC_HASHLIB_H
#define ADCIRC_HASHLIB_H

#include <QObject>
#include <QVector>
#include "libAdcircHash_global.h"
#include "QADCModules.h"
#include "ahl_adcirc_mesh.h"


class LIBADCIRCHASHSHARED_EXPORT Adcirc_hashlib : public QObject
{

public:
    explicit Adcirc_hashlib(QObject *parent = 0);

    int hashAdcircMesh(QString path);
    int writeHashedMesh();
    int readHashedMesh();
    int organizeHashes();

protected:
    int buildDirectoryTree(QString directory);

private:
    QString meshPath;

    ahl_adcirc_mesh *mesh;

    QVector<ahl_adcirc_node*>     nodeHashList;
    QVector<ahl_adcirc_element*>  elementHashList;
    QVector<ahl_adcirc_boundary*> openBCHashList;
    QVector<ahl_adcirc_boundary*> landBCHashList;

    ///Number of files to split into when hashing (default: 256)
    int nSplit = 256;

};



#endif // ADCIRC_HASHLIB_H
