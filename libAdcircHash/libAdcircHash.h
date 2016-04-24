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

    int createPartitions(QString meshFile, QString outputFile, int numPartitions);
    int writePartitionedMesh(QString meshFile,QString outputFile);

protected:

private:

    int hashAdcircMesh(QString inputFile, QString outputFile);
    int readHashedMesh();
    int organizeHashes();
    int partitionMesh();
    int readPolygons();
    int metisPartition(int numPartitions);
    int buildPolygons(int numPartitions);
    int writePolygonPartitions();
    int buildDirectoryTree(QString directory);
    int generateDirectoryNames(QString directory);
    int deletePolygons();
    bool pointInPolygon(QPointF point,int polygonIndex);
    int polygonPrecalc();
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
    QVector<QPolygonF>               polygons;
    QVector<QRectF>                  rect;
    QVector<QVector<double> >        polygonConstant;
    QVector<QVector<double> >        polygonMultiples;
    QDir myDir,nodeDir,elemDir,boundDir,openBoundDir;
    QDir landBoundDir,systemDir,partitionDir;

};



#endif // ADCIRC_HASHLIB_H
