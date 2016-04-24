#ifndef ADCIRC_HASHLIB_H
#define ADCIRC_HASHLIB_H

#include <QDir>
#include <QObject>
#include <QVector>
#include <QPolygonF>
#include "libAdcircHash_global.h"
#include "QADCModules.h"
#include "rectangle.h"

class LIBADCIRCHASHSHARED_EXPORT Adcirc_hashlib : public QObject
{

public:
    explicit Adcirc_hashlib(QObject *parent = 0);

    int createPartitions(QString meshFile, QString outputFile, int numPartitions);
    int writePartitionedMesh(QString meshFile,QString outputFile);

protected:

private:

    int hashAdcircMesh(QString inputFile);
    int readHashedMesh();
    int organizeHashes();
    int partitionMesh();
    int readPolygons();
    int metisPartition();
    int buildPolygons();
    int writePolygonPartitions();
    int buildDirectoryTree(QString directory);
    int generateDirectoryNames(QString directory);
    int deletePolygons();
    bool removeDirectory(const QString &dirName);
    QString formatBoundaryHashLine(adcirc_boundary *boundary, int index);

    int                              nMeshPartitions;
    QString                          meshPath;
    QString                          outputPath;
    adcirc_mesh                     *mesh;
    QVector<QList<adcirc_node*>>     nodeList;
    QVector<QList<adcirc_element*>>  elementList;
    QVector<int>                     nodePartitionList;
    QVector<int>                     elementPartitionList;
    QVector<adcirc_element*>         continuousElementTable;
    QVector<Rectangle>               partitionRectangles;
    QDir myDir,nodeDir,elemDir,boundDir,openBoundDir;
    QDir landBoundDir,systemDir;

};



#endif // ADCIRC_HASHLIB_H
