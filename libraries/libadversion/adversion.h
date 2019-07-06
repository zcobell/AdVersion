#ifndef ADVERSION_H
#define ADVERSION_H

#include <memory>
#include <string>

class AdversionImpl;

class AdVersion {
 public:
  AdVersion(const std::string &meshFilename, const std::string &rootDirectory);

  ~AdVersion();

  void partitionMesh(size_t nPartitions = 0);
  void readPartitionedMesh();

  std::string meshFilename() const;
  void setMeshFilename(const std::string &meshFilename);

  std::string nodalAttributesFilename() const;
  void setNodalAttributesFilename(const std::string &nodalAttributesFilename);

  size_t numPartitions() const;
  void setNumPartitions(const size_t &numPartitions);

  void writeAdcircMesh(const std::string &filename);

 private:
  std::unique_ptr<AdversionImpl> m_impl;
};

#endif  // ADVERSION_H
