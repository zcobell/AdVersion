#include <QCoreApplication>
#include "libAdcircHash.h"
#include <QDebug>
#include <QPointer>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QString mesh("/home/zcobell/Development/adcirc_hashlib/ms-riv.grd");
    QString outputFile("C:/Users/zcobell/Documents/Codes/adcirc_hashlib/wetting_and_drying_test.adcsha1");

    QPointer<Adcirc_hashlib> hashlib = new Adcirc_hashlib();

    qDebug() << "Hashing...";
    int ierr = hashlib->hashAdcircMesh(mesh,outputFile);

    return 0;
}
