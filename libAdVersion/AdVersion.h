//-----GPL----------------------------------------------------------------------
//
// This file is part of AdVersion
// Copyright (C) 2015  Zach Cobell
//
//
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
//------------------------------------------------------------------------------
//
//  File: AdVersion.h
//
//------------------------------------------------------------------------------
#ifndef ADVERSION_H
#define ADVERSION_H

#include <QDir>
#include <QObject>
#include <QVector>
#include <QPolygonF>
#include "libAdVersion_global.h"
#include "QADCModules.h"
#include "rectangle.h"

class LIBADVERSIONSHARED_EXPORT AdVersion : public QObject
{
    Q_OBJECT
public:
    explicit AdVersion(QObject *parent = 0);

    ~AdVersion();

    int createPartitions(QString meshFile, QString outputFile, int numPartitions);
    int writePartitionedMesh(QString meshFile,QString outputFile);
    int setHashAlgorithm(QCryptographicHash::Algorithm algorithm);

    int readPartitionedMesh(QString meshFolder);
    int writeMesh(QString outputFile);

    static int getGitVersion(QString gitDirectory, QString &version);

    static int gitInit(QString gitDirectory);

protected:

private:

    int hashAdcircMesh(QString inputFile);
    int readHashedMesh();
    int organizeHashes();
    int partitionMesh();
    int readPolygons();
    int metisPartition();
    int renumber();
    int buildPolygons();
    int writePolygonPartitions();
    int buildDirectoryTree(QString directory);
    int generateDirectoryNames(QString directory);
    int deletePolygons();
    bool removeDirectory(const QString &dirName);
    QString formatBoundaryHashLine(adcirc_boundary *boundary, int index);
    int readBoundaryHashLine(QString &line, adcirc_boundary *boundary, int index, QMap<QString, adcirc_node *> &map);

    int                              nMeshPartitions;
    QString                          meshFile;
    QString                          outputFile;
    adcirc_mesh                     *mesh;
    QVector<QList<adcirc_node*>>     nodeList;
    QVector<QList<adcirc_element*>>  elementList;
    QVector<int>                     nodePartitionList;
    QVector<int>                     elementPartitionList;
    QVector<adcirc_element*>         continuousElementTable;
    QVector<Rectangle>               partitionRectangles;
    QDir                             myDir,nodeDir,elemDir;
    QDir                             boundDir,openBoundDir;
    QDir                             landBoundDir,systemDir;
    QCryptographicHash::Algorithm    hashAlgorithm;

};



#endif // AdVersion_H
