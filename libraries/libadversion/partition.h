#ifndef PARTITION_H
#define PARTITION_H

#include "adcircmodules.h"

class Partition {
 public:
  enum Format { ASCII, NETCDF };

  Partition();
  void addNode(Adcirc::Geometry::Node *n);
  void addElement(Adcirc::Geometry::Element *e);
  void addBoundary(Adcirc::Geometry::Boundary *b);
  void write(Format writeFormat, const std::string &nodesFilename,
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
  void writeNodesAscii(const std::string &filename);
  void writeElementsAscii(const std::string &filename);
  void writeNodesNetCDF(const std::string &filename);
  void writeElementsNetCDF(const std::string &filename);
  void writeAscii(const std::string &nodesFilename,
                  const std::string &elementsFilename);
  void writeNetCDF(const std::string &nodeFilename,
                   const std::string &elementsFilename);

  std::vector<Adcirc::Geometry::Node *> m_nodes;
  std::vector<Adcirc::Geometry::Element *> m_elements;
  std::vector<Adcirc::Geometry::Boundary *> m_boundaries;
};

#endif  // PARTITION_H
