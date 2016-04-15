#ifndef AHL_ADCIRC_MESH_H
#define AHL_ADCIRC_MESH_H

#include <QObject>
#include <QCryptographicHash>
#include "adcirc_mesh.h"
#include "ahl_adcirc_node.h"
#include "ahl_adcirc_element.h"
#include "ahl_adcirc_boundary.h"

class ahl_adcirc_mesh : public adcirc_mesh
{
public:
    explicit ahl_adcirc_mesh(QObject *parent = 0);

    QVector<ahl_adcirc_node*>       nodes;
    QVector<ahl_adcirc_element*>    elements;
    QVector<ahl_adcirc_boundary*>   openBC;
    QVector<ahl_adcirc_boundary*>   landBC;
    QString                         hash;

    int hashMesh();

};

#endif // AHL_ADCIRC_MESH_H
