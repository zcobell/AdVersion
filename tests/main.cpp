#include <QCoreApplication>
#include "libAdcircHash.h"
#include <QDir>
#include <QPointer>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QString directory("C:/Users/zcobell/Documents/Codes/adcirc_hashlib/testmesh");

    QPointer<Adcirc_hashlib> hashlib = new Adcirc_hashlib();

    return 0;
}
