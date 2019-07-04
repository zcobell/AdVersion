#ifndef PARTITION_H
#define PARTITION_H

#include "adcircmodules.h"

class Partition {
 public:
  Partition();
  void addNode(Adcirc::Geometry::Node *n);
  void addElement(Adcirc::Geometry::Element *e);
  void addBoundary(Adcirc::Geometry::Boundary *b);
  void write(const std::string &nodesFilename,
             const std::string &elementsFilename);
  void guessSizeNodes(size_t size);
  void guessSizeElements(size_t size);
  void guessSizeBoundaries(size_t size);
  size_t numNodes() const;
  size_t numElements() const;
  size_t numBoundaries() const;
  void sort();

 private:
  void sortNodes();
  void sortElements();
  void sortBoundaries();
  void writeNodes(const std::string &filename);
  void writeElements(const std::string &filename);

  std::vector<Adcirc::Geometry::Node *> m_nodes;
  std::vector<Adcirc::Geometry::Element *> m_elements;
  std::vector<Adcirc::Geometry::Boundary *> m_boundaries;
};

#endif  // PARTITION_H
