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
/**
 * \class AdVersion
 *
 * \brief Class that is used to version an ADCIRC mesh
 *
 * \author Zach Cobell
 *
 * The AdVersion class is used to partition and re-assemble an ADCIRC mesh
 * for use with the Git versioning software.
 *
 * Contact: zcobell@gmail.com
 *
 */
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

    static bool removeDirectory(const QString &dirName);

private:

    int hashAdcircMesh(QString inputFile);

    int readHashedMesh();

    int organizeHashes();

    int partitionMesh();

    int readRectangles();

    int metisPartition();

    int renumber();

    int buildRectangles();

    int writeRectanglePartitions();

    int writeNodeFiles();

    int writeElementFiles();

    int writeBoundaryFiles();

    int writeSystemFiles();

    int buildDirectoryTree(QString directory);

    int generateDirectoryNames(QString directory);

    QString formatBoundaryHashLine(adcirc_boundary *boundary, int index);

    int readBoundaryHashLine(QString &line, adcirc_boundary *boundary, int index, QMap<QString, adcirc_node *> &map);



    ///Number of partitions to create (or were found) for this mesh
    int                              nMeshPartitions;

    ///Name of the mesh file to use
    QString                          meshFile;

    ///Name of the output file to create
    QString                          outputFile;

    ///Mesh object (from QADCModules)
    adcirc_mesh                     *mesh;

    ///List of nodes that will be found in a particular partition
    QVector<QList<adcirc_node*>>     nodeList;

    ///List of elements that will be found in a particular partition
    QVector<QList<adcirc_element*>>  elementList;

    ///Partition number for each node in the mesh
    QVector<int>                     nodePartitionList;

    ///Partition number for each element in the mesh
    QVector<int>                     elementPartitionList;

    ///Element table that has the gaps between levees removed for Metis
    QVector<adcirc_element*>         continuousElementTable;

    ///Rectangles denoting the partitions for the mesh
    QVector<Rectangle>               partitionRectangles;

    ///Main directory
    QDir                             myDir;

    ///Directory to place the node files
    QDir                             nodeDir;

    ///Directory to place the element files
    QDir                             elemDir;

    ///Directory to place the boundary files
    QDir                             boundDir;

    ///Directory to place the open boudary files
    QDir                             openBoundDir;

    ///Directory to place the land boundary files
    QDir                             landBoundDir;

    ///Directory to place the system files
    QDir                             systemDir;

    ///Hash algorithm to use (MD5 or SHA1)
    QCryptographicHash::Algorithm    hashAlgorithm;

};



#endif // AdVersion_H
