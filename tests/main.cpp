#include <QCoreApplication>
#include "libAdVersion.h"
#include <QDebug>
#include <QPointer>
#include <QElapsedTimer>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    bool doPartition = false;
    bool doWriteMesh = true;


//    QString mesh("/home/zcobell/Development/cpra2017_v11i_S01Y10G00_chk.grd");
//    QString outputFile("/home/zcobell/Development/adcirc_hashlib/ms-riv.adcsha1");
    QString mesh("C:/Users/zcobell/Desktop/test.14");
    QString outputFile("C:/Users/zcobell/Desktop/cpra2017.adcsha1");

    QPointer<AdVersion> partition = new AdVersion();
    QPointer<AdVersion> hashlib   = new AdVersion();
    int ierr;

    if(doPartition)
    {
        qDebug() << "Partitioning...";
        QElapsedTimer partitionTimer;
        partitionTimer.start();
        ierr = partition->createPartitions(mesh,outputFile,256);
        qDebug() << partitionTimer.elapsed()/1000 << "seconds to generate partitioning rectangles";
        delete partition;
    }

    qDebug() << "";

    if(doWriteMesh)
    {
        qDebug() << "Writing mesh...";
        QElapsedTimer writeTimer;
        writeTimer.start();
        ierr = hashlib->writePartitionedMesh(mesh,outputFile);
        qDebug() << writeTimer.elapsed()/1000 << "seconds to partition and write the mesh.";
        delete hashlib;
    }

    return 0;
}
