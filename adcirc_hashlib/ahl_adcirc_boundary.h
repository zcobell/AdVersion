#ifndef AHL_ADCIRC_BOUNDARY_H
#define AHL_ADCIRC_BOUNDARY_H

#include <QCryptographicHash>
#include "adcirc_boundary.h"

class ahl_adcirc_boundary : public adcirc_boundary
{
public:
    ahl_adcirc_boundary(int code, int size, QObject *parent = 0);

    int hashBonudary();

    QString hash;

};

#endif // AHL_ADCIRC_BOUNDARY_H
