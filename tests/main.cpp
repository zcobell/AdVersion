#include <QCoreApplication>
#include "libAdcircHash.h"
#include <QDebug>
#include <QPointer>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QString mesh("C:/Users/zcobell/Documents/Codes/adcirc_hashlib/wetting_and_drying_test.14");
    QString outputFile("C:/Users/zcobell/Documents/Codes/adcirc_hashlib/wetting_and_drying_test.adcsha1");

    Adcirc_hashlib *hashlib = new Adcirc_hashlib();


    qDebug() << "Hashing...";
    hashlib->hashAdcircMesh(mesh,outputFile);

    delete hashlib;

    return 0;
}
