#include "partition.h"

Partition::Partition() {}

void Partition::addNode(Adcirc::Geometry::Node *n) {
  this->m_nodes.push_back(n);
}

void Partition::addElement(Adcirc::Geometry::Element *e) {
  this->m_elements.push_back(e);
}

void Partition::addBoundary(Adcirc::Geometry::Boundary *b) {
  this->m_boundaries.push_back(b);
}

void write() { return; }
