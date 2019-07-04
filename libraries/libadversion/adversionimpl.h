#ifndef ADVERSIONIMPL_H
#define ADVERSIONIMPL_H

#include <string>
#include <vector>
#include "adcircmodules.h"
#include "git2.h"
#include "metis.h"
#include "partition.h"
#include "rectangle.h"
#include "kdtree.h"

class AdversionImpl {
 public:
  AdversionImpl(const std::string &filename = std::string());

  void partitionMesh(size_t nPartitions = 0);

  std::string meshFilename() const;
  void setMeshFilename(const std::string &meshFilename);

  std::string nodalAttributesFilename() const;
  void setNodalAttributesFilename(const std::string &nodalAttributesFilename);

  size_t numPartitions() const;
  void setNumPartitions(const size_t &numPartitions);

 private:
  void metisPartition(std::vector<size_t> &nodePartition,
                      std::vector<size_t> &elementPartition);

  void buildRectangles(std::vector<size_t> &nodePartition,
                       std::vector<Rectangle> &rectangles);

  void placeNodesInRegions(std::vector<Rectangle> &rectangles,
                           std::vector<Partition> &partitions);

  void placeElementsInRegions(std::vector<Rectangle> &rectangles,
                              std::vector<Partition> &partitions);

  std::unique_ptr<Adcirc::Geometry::Mesh> m_mesh;

  size_t m_numPartitions;
  std::string m_meshFilename;
  std::string m_nodalAttributesFilename;

  std::vector<std::unique_ptr<Adcirc::Geometry::Element>>
  generateWeirConnectingElements();
  void generateRectangleKdtree(std::vector<Rectangle> &rectangles, Kdtree &k);
};

#endif  // ADVERSIONIMPL_H
