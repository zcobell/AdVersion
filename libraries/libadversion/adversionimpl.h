#ifndef ADVERSIONIMPL_H
#define ADVERSIONIMPL_H

#include <string>
#include <vector>
#include "adcircmodules.h"
#include "git2.h"
#include "metis.h"

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
  void metisPartition(size_t nPartitions, std::vector<size_t> &nodePartition,
                      std::vector<size_t> &elementPartition);

  std::unique_ptr<Adcirc::Geometry::Mesh> m_mesh;

  size_t m_numPartitions;
  std::string m_meshFilename;
  std::string m_nodalAttributesFilename;

  std::vector<std::unique_ptr<Adcirc::Geometry::Element>>
  generateWeirConnectingElements();
};

#endif  // ADVERSIONIMPL_H
