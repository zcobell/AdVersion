#ifndef AHL_ADCIRC_BOUNDARY_H
#define AHL_ADCIRC_BOUNDARY_H

#include <QCryptographicHash>
#include "ahl_adcirc_node.h"
#include "adcirc_boundary.h"

class ahl_adcirc_boundary : public adcirc_boundary
{
public:
    ahl_adcirc_boundary(QObject *parent = 0);

    int hashBonudary();

    QString hash;

    QVector<ahl_adcirc_node*>    n1;
    QVector<ahl_adcirc_node*>    n2;

};

#endif // AHL_ADCIRC_BOUNDARY_H
