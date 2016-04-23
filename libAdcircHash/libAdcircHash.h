#ifndef ADCIRC_HASHLIB_H
#define ADCIRC_HASHLIB_H

#include <QDir>
#include <QObject>
#include <QVector>
#include <QPolygonF>
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
    int metisPartition(int numPartitions);
    int partitionMesh(int numPartitions);
    int buildPolygons(int numPartitions);
    int writePolygonPartitions();

    QPointer<adcirc_mesh> mesh;

protected:
    int buildDirectoryTree(QString directory);
    QString formatBoundaryHashLine(adcirc_boundary *boundary, int index);

private:
    QString meshPath;
    QString outputPath;

    QVector<QList<adcirc_node*>>     nodeHashList;
    QVector<QList<adcirc_element*>>  elementHashList;
    QVector<QList<adcirc_boundary*>> openBCHashList;
    QVector<QList<adcirc_boundary*>> landBCHashList;
    QVector<int>                     nodePartitionList;
    QVector<int>                     elementPartitionList;
    QVector<adcirc_element*>         continuousElementTable;
    QVector<QPolygonF>               polygons;

    QDir myDir,nodeDir,elemDir,boundDir,openBoundDir;
    QDir landBoundDir,systemDir,partitionDir;

};



#endif // ADCIRC_HASHLIB_H
