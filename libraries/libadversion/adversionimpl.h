#ifndef ADVERSIONIMPL_H
#define ADVERSIONIMPL_H

#include <string>
#include <vector>
#include "adcircmodules.h"
#include "partition.h"
#include "rectangle.h"

#ifdef USE_GOOGLE_FLAT_MAP
#include "absl/container/flat_hash_map.h"
#else
#include <unordered_map>
#endif

//...Alias for using the Google hashtable if
//   available. Prevents using ifdefs throughout
//   source files
template <typename T1, typename T2>
#ifdef USE_GOOGLE_FLAT_MAP
using adunordered_map = absl::flat_hash_map<T1, T2>;
#else
using adunordered_map = std::unordered_map<T1, T2>;
#endif

class AdversionImpl {
 public:
  AdversionImpl(const std::string &meshFilename,
                const std::string &rootDirectory);

  void partitionMesh(size_t nPartitions = 0);
  void readPartitionedMesh();

  std::string meshFilename() const;
  void setMeshFilename(const std::string &meshFilename);

  std::string nodalAttributesFilename() const;
  void setNodalAttributesFilename(const std::string &nodalAttributesFilename);

  size_t numPartitions() const;
  void setNumPartitions(const size_t &numPartitions);

  std::string rootDirectory() const;
  void setRootDirectory(const std::string &rootDirectory);

  void writeAdcircMesh(const std::string &filename);

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

  void writeBoundaries(const std::string &rootPath);

  void writeAdcircBoundary(Adcirc::Geometry::Boundary *b);

  void writeSystemInformation(const std::string &rootPath,
                              std::vector<Rectangle> &rect);

  void readSystemInformation(Partition::Format &fmt, size_t &numNodes,
                             size_t &numElements, size_t &numOpenBoundaries,
                             size_t &numLandBoundaries,
                             std::string &meshHeader);

  void readMeshNodes(adunordered_map<std::string, size_t> &nodeTable);
  void readMeshElements(adunordered_map<std::string, size_t> &nodeTable);
  void readOpenBoundaries(adunordered_map<std::string,size_t> &nodeTable);
  void readLandBoundaries(adunordered_map<std::string,size_t> &nodeTable);

  void gitInit();

  std::unique_ptr<Adcirc::Geometry::Mesh> m_mesh;
  std::vector<Rectangle> m_rectangles;

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
