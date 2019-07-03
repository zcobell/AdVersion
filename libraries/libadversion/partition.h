#ifndef PARTITION_H
#define PARTITION_H

#include "adcircmodules.h"

class Partition
{
public:
    Partition();
    void addNode(Adcirc::Geometry::Node *n);
    void addElement(Adcirc::Geometry::Element *e);
    void addBoundary(Adcirc::Geometry::Boundary *b);
    void write();
private:

    std::vector<Adcirc::Geometry::Node*> m_nodes;
    std::vector<Adcirc::Geometry::Element*> m_elements;
    std::vector<Adcirc::Geometry::Boundary*> m_boundaries;
};

#endif // PARTITION_H
