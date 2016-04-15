#ifndef ADCIRC_HASHLIB_H
#define ADCIRC_HASHLIB_H

#include <QDir>
#include "libAdcircHash_global.h"
#include "QADCModules.h"
#include "ahl_adcirc_node.h"


class LIBADCIRCHASHSHARED_EXPORT Adcirc_hashlib
{

public:
    Adcirc_hashlib();

    int buildDirectoryTree(QDir directory);

};



#endif // ADCIRC_HASHLIB_H
