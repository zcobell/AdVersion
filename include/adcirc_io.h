#ifndef ADCIRCIO_H
#define ADCIRCIO_H

#include <QObject>
#include <QVector>
#include <QString>
#include <QFile>
#include <QTextStream>
#include <QCryptographicHash>
#include <QDebug>

//int hashMethod = QCryptographicHash::Sha1;

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

struct adcirc_node
{
    QString locationHash;
    double x,y,z;
};

struct adcirc_element
{
    QString elementHash;
    int c1,c2,c3;
    QString h1,h2,h3;
};

//...Structure for housing an entire adcirc mesh and its hashes
struct adcirc_mesh
{
    int status;                                    //...error status
    QString header;                                //...file header
    int NumNodes;                                  //...Number of nodes in mesh
    int NumElements;                               //...Number of elements in mesh
    QString mesh_hash;                          //...hash for the entire mesh
    QVector<adcirc_node> node;                     //...adcirc node vector
    QVector<adcirc_element> element;               //...adcirc element vector
};

class adcirc_io : public QObject
{
    Q_OBJECT
public:
    explicit adcirc_io(QObject *parent = 0);

    adcirc_mesh readAdcircMesh(QString fileName);
    adcirc_mesh readAdcircSha1Mesh(QString fileName);

    int writeAdcircHashMesh(QString fileName, adcirc_mesh &myMesh);
    int sortAdcircHashes(adcirc_mesh &myMesh);
    int createAdcircHashes(adcirc_mesh &myMesh);
    int numberAdcircMesh(adcirc_mesh &myMesh);
    int writeAdcircMesh(QString fileName, adcirc_mesh &myMesh);

signals:

public slots:
};

#endif // ADCIRCIO_H
