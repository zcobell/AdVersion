#ifndef AHL_ADCIRC_MESH_H
#define AHL_ADCIRC_MESH_H

#include "adcirc_mesh.h"
#include "ahl_adcirc_node.h"
#include "ahl_adcirc_element.h"
#include "ahl_adcirc_boundary.h"

class ahl_adcirc_mesh : public adcirc_mesh
{
public:
    ahl_adcirc_mesh();

    QVector<ahl_adcirc_node*>       nodes;
    QVector<ahl_adcirc_element*>    elements;
    QVector<ahl_adcirc_boundary*>   openBC;
    QVector<ahl_adcirc_boundary*>   landBC;
    QString                         hash;

};

#endif // AHL_ADCIRC_MESH_H
