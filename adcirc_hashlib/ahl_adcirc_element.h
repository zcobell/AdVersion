#ifndef AHL_ADCIRC_ELEMENT_H
#define AHL_ADCIRC_ELEMENT_H

#include <QCryptographicHash>
#include "adcirc_element.h"
#include "ahl_adcirc_node.h"

class ahl_adcirc_element : public adcirc_element
{
public:
    ahl_adcirc_element();

    QVector<ahl_adcirc_node*> connections;

    int hashElement();

    QString hash;
};

#endif // AHL_ADCIRC_ELEMENT_H
