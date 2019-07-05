#ifndef ADVERSIONIMPL_H
#define ADVERSIONIMPL_H

#include <string>
#include <vector>
#include "adcircmodules.h"
#include "git2.h"
#include "kdtree.h"
#include "metis.h"
#include "partition.h"
#include "rectangle.h"

class AdversionImpl {
 public:
  AdversionImpl(const std::string &meshFilename,
                const std::string &rootDirectory);

  void partitionMesh(size_t nPartitions = 0);

  std::string meshFilename() const;
  void setMeshFilename(const std::string &meshFilename);

  std::string nodalAttributesFilename() const;
  void setNodalAttributesFilename(const std::string &nodalAttributesFilename);

  size_t numPartitions() const;
  void setNumPartitions(const size_t &numPartitions);

  std::string rootDirectory() const;
  void setRootDirectory(const std::string &rootDirectory);

 private:
  void metisPartition(std::vector<size_t> &nodePartition,
                      std::vector<size_t> &elementPartition);

  void buildRectangles(std::vector<size_t> &nodePartition,
                       std::vector<Rectangle> &rectangles);

  void placeNodesInRegions(std::vector<Rectangle> &rectangles,
                           std::vector<Partition> &partitions);

  void placeElementsInRegions(std::vector<Rectangle> &rectangles,
                              std::vector<Partition> &partitions);

  void makeDirectoryStructure(const std::string &rootPath);

  void writePartitionedMesh(const std::string &rootPath,
                            std::vector<Partition> &partitions);

  void writeSystemInformation(const std::string &rootPath,
                              std::vector<Rectangle> &rect);

  void gitInit();

  std::unique_ptr<Adcirc::Geometry::Mesh> m_mesh;

  size_t m_numPartitions;
  std::string m_meshFilename;
  std::string m_nodalAttributesFilename;
  std::string m_rootDirectory;

  std::vector<std::unique_ptr<Adcirc::Geometry::Element>>
  generateWeirConnectingElements();
  void generateRectangleKdtree(std::vector<Rectangle> &rectangles, Kdtree &k);

  void placeMissingNodes(std::vector<unsigned short> &found,
                         std::vector<size_t> &part,
                         std::vector<Partition> &partitions);

  void placeMissingElements(std::vector<unsigned short> &found,
                            std::vector<size_t> &part,
                            std::vector<Partition> &partitions);
};

#endif  // ADVERSIONIMPL_H
