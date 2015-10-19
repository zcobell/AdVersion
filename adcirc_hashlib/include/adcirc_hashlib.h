//-----GPL----------------------------------------------------------------------
//
// This file is part of adcirc_hashlib
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
//  File: adcirc_hashlib.h
//
//------------------------------------------------------------------------------

#ifndef ADCIRC_HASHLIB_H
#define ADCIRC_HASHLIB_H

#define ERR_NOERR    -9000
#define ERR_CANCELED -9990
#define ERR_NOFILE   -9991
#define ERR_UNKNOWN  -9999

#include <QObject>
#include <QVector>
#include <QString>
#include <QFile>
#include <QTextStream>
#include <QCryptographicHash>
#include <QDebug>
#include <QTime>
#include <stdio.h>
#include <iostream>
#include <cstdlib>
using namespace std;

//...Structure for the boundary arrays
struct adcirc_boundary
{
    int code;                                  //...boundary code
    int NumNodes;                              //...number of nodes on boundary
    double averageLongitude;                   //...Average Longitude of the boundary segment
    QVector<int> node1;                        //...hash node list for node 1
    QVector<int> node2;                        //...hash node list for node 2
    QVector<QString> node1_hash;               //...hash node list for node 1
    QVector<QString> node2_hash;               //...hash node list for node 2
    QVector<double> elevation;                 //...elevation for boundary
    QVector<double> supercritical;             //...supercritical flow coefficient
    QVector<double> subcritical;               //...subcritical flow coefficient
    QVector<double> pipe_ht;                   //...cross barrier pipe elevation
    QVector<double> pipe_coef;                 //...cross barrier pipe coefficient
    QVector<double> pipe_diam;                 //...cross barrier pipe diameter
    QString boundary_hash;                     //...hash for the full boundary
};

struct adcirc_boundary_hash
{
    int code;                                  //...boundary code
    int NumNodes;                              //...number of nodes on boundary
    QVector<int> node1;                        //...hash node list for node 1
    QVector<int> node2;                        //...hash node list for node 2
    QVector<QString> node1_hash;               //...hash node list for node 1
    QVector<QString> node2_hash;               //...hash node list for node 2
    QVector<double> elevation;                 //...elevation for boundary
    QVector<double> supercritical;             //...supercritical flow coefficient
    QVector<double> subcritical;               //...subcritical flow coefficient
    QVector<double> pipe_ht;                   //...cross barrier pipe elevation
    QVector<double> pipe_coef;                 //...cross barrier pipe coefficient
    QVector<double> pipe_diam;                 //...cross barrier pipe diameter
    QString boundary_hash;                     //...hash for the full boundary
};

struct adcirc_node
{
    QString locationHash;                      //...Hash of the X,Y location
    double x,y,z;                              //...X,Y,Z coordinates of the node
};

struct adcirc_element
{
    QString elementHash;                       //...Hash of the hashes of n1+n2+n3
    int c1,c2,c3;                              //...Nodes that make up the element (n1..n3)
    QString h1,h2,h3;                          //...Nodal connectivity hashes (hash of c1, etc)
};

//...Structure for housing an entire adcirc mesh and its hashes
struct adcirc_mesh
{
    int status;                                    //...Error status
    QString header;                                //...File header
    int NumNodes;                                  //...Number of nodes in mesh
    int NumElements;                               //...Number of elements in mesh
    int NumOpenBoundaries;                         //...Number of open boundaries
    int NumOpenBoundaryNodes;                      //...Number of open boundary nodes
    int NumLandBoundaries;                         //...Number of land boundaries
    int NumLandBoundaryNodes;                      //...Number of land boundary nodes
    QString mesh_hash;                             //...Hash for the entire mesh
    QVector<adcirc_node> node;                     //...Adcirc node vector
    QVector<adcirc_element> element;               //...Adcirc element vector
    QVector<adcirc_boundary_hash> openBoundaryH;   //...Adcirc Open Boundaries vector
    QVector<adcirc_boundary_hash> landBoundaryH;   //...Adcirc Land Boundaries vector
    QVector<adcirc_boundary> openBoundary;         //...Adcirc Open Boundaries vector
    QVector<adcirc_boundary> landBoundary;         //...Adcirc Land Boundaries vector
};


class adcirc_hashlib : public QObject
{
    Q_OBJECT
public:

    explicit adcirc_hashlib(QObject *parent = 0);
    int readAdcircMesh(QString fileName, adcirc_mesh &myMesh);
    int readAdcircSha1Mesh(QString fileName, adcirc_mesh &myMesh);
    int writeAdcircHashMesh(QString fileName, adcirc_mesh &myMesh);
    int sortAdcircHashes(adcirc_mesh &myMesh);
    int createAdcircHashes(adcirc_mesh &mesh);
    int numberAdcircMesh(adcirc_mesh &myMesh);
    int writeAdcircMesh(QString fileName, adcirc_mesh &myMesh);

    QString hashNode(adcirc_node &node);
    QString hashNode(double x, double y);

    QString hashElement(adcirc_mesh &mesh, int elementid);
    QString hashElement(adcirc_node n1, adcirc_node n2, adcirc_node n3);
    QString hashElement(double x1, double y1, double x2, double y2, double x3, double y3);

    QString hashOpenBC(adcirc_mesh &mesh, int OpenBCIndex);
    QString hashLandBC(adcirc_mesh &mesh, int LandBCIndex);

    QString hashMesh(adcirc_mesh &mesh);

};

bool operator< (const adcirc_node &first, const adcirc_node &second);
bool operator< (const adcirc_element &first, const adcirc_element &second);
bool operator< (const adcirc_boundary_hash &first, const adcirc_boundary_hash &second);
bool operator< (const adcirc_boundary &first, const adcirc_boundary &second);

#endif // ADCIRC_HASHLIB_H
