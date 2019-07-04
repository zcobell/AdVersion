#include "partition.h"
#include <algorithm>
#include <fstream>
#include "boost/format.hpp"

bool nodeHashLessThan(Adcirc::Geometry::Node *n1, Adcirc::Geometry::Node *n2) {
  return n1->positionHash() < n2->positionHash();
}

bool elementHashLessThan(Adcirc::Geometry::Element *e1,
                         Adcirc::Geometry::Element *e2) {
  return e1->hash() < e2->hash();
}

bool boundaryHashLessThan(Adcirc::Geometry::Boundary *b1,
                          Adcirc::Geometry::Boundary *b2) {
  return b1->hash() < b2->hash();
}

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

size_t Partition::numNodes() const { return this->m_nodes.size(); }

size_t Partition::numElements() const { return this->m_elements.size(); }

size_t Partition::numBoundaries() const { return this->m_boundaries.size(); }

void Partition::sort() {
  this->sortNodes();
  this->sortElements();
}

void Partition::sortNodes() {
  std::sort(this->m_nodes.begin(), this->m_nodes.end(), nodeHashLessThan);
}

void Partition::sortElements() {
  std::sort(this->m_elements.begin(), this->m_elements.end(),
            elementHashLessThan);
}

void Partition::sortBoundaries() {
  std::sort(this->m_boundaries.begin(), this->m_boundaries.end(),
            boundaryHashLessThan);
}

void Partition::write(const std::string &nodesFilename,
                      const std::string &elementsFilename) {
  this->writeNodes(nodesFilename);
  this->writeElements(elementsFilename);
  return;
}

void Partition::writeNodes(const std::string &filename) {
  std::ofstream f;
  f.open(filename.c_str(), std::ios::out);
  std::string n = boost::str(boost::format("%i\n")%this->numNodes());
  f.write(n.c_str(),n.size());
  for (auto &n : this->m_nodes) {
    std::string line =
        boost::str(boost::format("%s %+018.12e %+018.12e %+018.12e\n") %
                   n->positionHash() % n->x() % n->y() % n->z());
    f.write(line.c_str(), line.size());
  }
  f.close();
}

void Partition::writeElements(const std::string &filename) {
  std::ofstream f;
  f.open(filename.c_str(), std::ios::out);
  std::string n = boost::str(boost::format("%i\n")%this->numElements());
  f.write(n.c_str(),n.size());
  for (auto &e : this->m_elements) {
    std::string line =
        boost::str(boost::format("%s %s %s %s\n") % e->hash() %
                   e->node(0)->positionHash() % e->node(1)->positionHash() %
                   e->node(2)->positionHash());
    f.write(line.c_str(), line.size());
  }
  f.close();
}
