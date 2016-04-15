#ifndef ADCIRC_HASHLIB_H
#define ADCIRC_HASHLIB_H

#include <QObject>
#include <QVector>
#include "libAdcircHash_global.h"
#include "QADCModules.h"


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

    QPointer<adcirc_mesh> mesh;

    QVector<QList<adcirc_node*>>     nodeHashList;
    QVector<QList<adcirc_element*>>  elementHashList;
    QVector<QList<adcirc_boundary*>> openBCHashList;
    QVector<QList<adcirc_boundary*>> landBCHashList;

    ///Number of files to split into when hashing (default: 3 (4096))
    int nSplit = 1;

};



#endif // ADCIRC_HASHLIB_H
