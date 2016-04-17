#ifndef ADCIRC_HASHLIB_H
#define ADCIRC_HASHLIB_H

#include <QDir>
#include <QObject>
#include <QVector>
#include "libAdcircHash_global.h"
#include "QADCModules.h"


class LIBADCIRCHASHSHARED_EXPORT Adcirc_hashlib : public QObject
{

public:
    explicit Adcirc_hashlib(QObject *parent = 0);

    int hashAdcircMesh(QString inputFile, QString outputFile);
    int writeHashedMesh(QString outputFile);
    int readHashedMesh();
    int organizeHashes();
    int createPartitions();

protected:
    int buildDirectoryTree(QString directory);
    QString formatBoundaryHashLine(adcirc_boundary *boundary, int index);

private:
    QString meshPath;

    QPointer<adcirc_mesh> mesh;

    QVector<QList<adcirc_node*>>     nodeHashList;
    QVector<QList<adcirc_element*>>  elementHashList;
    QVector<QList<adcirc_boundary*>> openBCHashList;
    QVector<QList<adcirc_boundary*>> landBCHashList;

    QDir myDir,nodeDir,elemDir,boundDir,openBoundDir,landBoundDir;

    ///Number of files to split into when hashing (default: 3 (4096))
    int nSplit = 2;

};



#endif // ADCIRC_HASHLIB_H
