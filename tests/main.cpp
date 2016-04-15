#include <QCoreApplication>
#include "libAdcircHash.h"
#include <QDebug>
#include <QPointer>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QString mesh("C:/Users/zcobell/Documents/Codes/adcirc_hashlib/wetting_and_drying_test.14");

    Adcirc_hashlib *hashlib = new Adcirc_hashlib();


    qDebug() << "Hashing...";
    hashlib->hashAdcircMesh(mesh);

    delete hashlib;

    return 0;
}
