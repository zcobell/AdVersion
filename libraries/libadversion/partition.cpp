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

void Partition::guessSizeNodes(size_t size) {
  if (this->m_nodes.size() < size) this->m_nodes.reserve(size);
}

void Partition::guessSizeElements(size_t size) {
  if (this->m_elements.size() < size) this->m_elements.reserve(size);
}

void Partition::guessSizeBoundaries(size_t size) {
  if (this->m_boundaries.size() < size) this->m_boundaries.reserve(size);
}

void write() { return; }
