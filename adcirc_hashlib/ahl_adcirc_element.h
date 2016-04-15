#ifndef AHL_ADCIRC_ELEMENT_H
#define AHL_ADCIRC_ELEMENT_H

#include <QCryptographicHash>
#include "adcirc_element.h"

class ahl_adcirc_element : public adcirc_element
{
public:
    ahl_adcirc_element();

    int hashElement();

    QString hash;
};

#endif // AHL_ADCIRC_ELEMENT_H
