#ifndef ADCIRC_IO_H
#define ADCIRC_IO_H

#include <QVector>
#include <QString>
#include <QFile>
#include <QTextStream>
#include <QCryptographicHash>
#include <QDebug>

//...Structure for the boundary arrays
struct adcirc_boundary
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
    QVector<QString> boundary_hash;            //...hash for the full boundary
};

//...Structure for housing an entire adcirc mesh and its hashes
struct adcirc_mesh
{
    int status;                                    //...variable to hold the status
    QString header;                                //...file header
    int NumNodes;                                  //...Number of nodes in mesh
    int NumElements;                               //...Number of elements in mesh
    int NumOpenBoundaries;                         //...Number of open boundary segments
    int NumLandBoundaries;                         //...Number of land boundary segments
    QString mesh_hash;                             //...hash for the entire mesh
    QVector<double> x_location;                    //...x nodal locations
    QVector<double> y_location;                    //...y nodal locations
    QVector<double> z_elevation;                   //...z nodal elevations
    QVector< QVector<int> >  connectivity;         //...nodal connectivity
    QVector< QVector<QString> > conn_node_hash;    //...node hashes for connectivity
    QVector<adcirc_boundary> OpenBoundaries;       //...open boundary array
    QVector<adcirc_boundary> LandBoundaries;       //...land boundary array
    QVector<QString> location_hash;                //...location sha1 hash
    QVector<QString> connectivity_hash;            //...nodal connectivity sha1 hash
    QVector<QString> openBoundaryHash;             //...open boundary sha1 hashes
    QVector<QString> landBoundaryHash;             //...land boundary sha1 hashes
};

adcirc_mesh readAdcircMesh(QString fileName);
int writeAdcircHashMesh(QString fileName, adcirc_mesh &myMesh);
int sortAdcircHashes(adcirc_mesh &myMeshIn, adcirc_mesh &myMeshOut);
int createAdcircHashes(adcirc_mesh &myMesh);
adcirc_mesh readAdcircSha1Mesh(QString fileName);
int numberAdcircMesh(adcirc_mesh &myMesh);
int writeAdcircMesh(QString fileName, adcirc_mesh &myMesh);

#endif // ADCIRC_IO_H
