#ifndef ADCIRCIO_H
#define ADCIRCIO_H

#include <QApplication>
#include <QObject>
#include <QVector>
#include <QString>
#include <QFile>
#include <QTextStream>
#include <QCryptographicHash>
#include <QDebug>
#include <QProgressDialog>
#include <QTime>
#include <QMessageBox>

#define ERR_NOERR    -9000
#define ERR_CANCELED -9990
#define ERR_NOFILE   -9991
#define ERR_UNKNOWN  -9999

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
    QVector<adcirc_boundary> openBoundary;         //...Adcirc Open Boundaries vector
    QVector<adcirc_boundary> landBoundary;         //...Adcirc Land Boundaries vector
};

class adcirc_io : public QObject
{
    Q_OBJECT
public:
    explicit adcirc_io(QObject *parent = 0);

    int readAdcircMesh(QString fileName, adcirc_mesh &myMesh, QProgressDialog &dialog, int &counter);
    int readAdcircSha1Mesh(QString fileName, adcirc_mesh &myMesh, QProgressDialog &dialog, int &counter);
    int writeAdcircHashMesh(QString fileName, adcirc_mesh &myMesh, QProgressDialog &dialog, int &counter);
    int sortAdcircHashes(adcirc_mesh &myMesh, QProgressDialog &dialog, int &counter);
    int createAdcircHashes(adcirc_mesh &myMesh, QProgressDialog &dialog, int &counter);
    int numberAdcircMesh(adcirc_mesh &myMesh,  QProgressDialog &dialog, int &counter);
    int writeAdcircMesh(QString fileName, adcirc_mesh &myMesh,  QProgressDialog &dialog, int &counter);
    static int process_a2s(QString inputFile,QString outputFile);
    static int process_s2a(QString inputFile,QString outputFile);
    void updateProgress(int &count, QProgressDialog &dialog);

signals:

public slots:
};

extern QTime polling;
extern int progressUpdateInterval;

#endif // ADCIRCIO_H
