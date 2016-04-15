#ifndef AHL_ADCIRC_NODE_H
#define AHL_ADCIRC_NODE_H


#include <QCryptographicHash>
#include "adcirc_node.h"

class ahl_adcirc_node : public adcirc_node
{
public:
    explicit ahl_adcirc_node(QObject *parent = 0);

    int hashNode();

    QString positionHash;
    QString fullHash;

};

#endif // AHL_ADCIRC_NODE_H
