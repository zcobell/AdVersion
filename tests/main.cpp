#include <QCoreApplication>
#include "libAdcircHash.h"
#include <QDebug>
#include <QPointer>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QString mesh("/home/zcobell/Development/adcirc_hashlib/cpra2017_v11i_S01Y10G00_chk.grd");
    QString outputFile("/home/zcobell/Development/adcirc_hashlib/ms-riv.adcsha1");

    QPointer<Adcirc_hashlib> partition = new Adcirc_hashlib();
    QPointer<Adcirc_hashlib> hashlib   = new Adcirc_hashlib();
    int ierr;

    qDebug() << "Partitioning...";
    ierr = partition->createPartitions(mesh,outputFile,512);
    delete partition;

    qDebug() << "";
    qDebug() << "Writing mesh...";
    ierr = hashlib->writePartitionedMesh(mesh,outputFile);

    return 0;
}
