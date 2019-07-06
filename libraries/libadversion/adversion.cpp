#include "adversion.h"
#include "adversionimpl.h"

AdVersion::AdVersion(const std::string &meshFilename,
                     const std::string &rootDirectory)
    : m_impl(new AdversionImpl(meshFilename, rootDirectory)) {}

AdVersion::~AdVersion() {}

void AdVersion::partitionMesh(size_t nPartitions) {
  this->m_impl->partitionMesh(nPartitions);
};

std::string AdVersion::meshFilename() const {
  return this->m_impl->meshFilename();
}
void AdVersion::setMeshFilename(const std::string &meshFilename) {
  this->m_impl->setMeshFilename(meshFilename);
}

std::string AdVersion::nodalAttributesFilename() const {
  return this->m_impl->nodalAttributesFilename();
}
void AdVersion::setNodalAttributesFilename(
    const std::string &nodalAttributesFilename) {
  this->m_impl->setNodalAttributesFilename(nodalAttributesFilename);
}

size_t AdVersion::numPartitions() const {
  return this->m_impl->numPartitions();
}
void AdVersion::setNumPartitions(const size_t &numPartitions) {
  this->m_impl->setNumPartitions(numPartitions);
}

void AdVersion::readPartitionedMesh() { this->m_impl->readPartitionedMesh(); }

void AdVersion::writeAdcircMesh(const std::string &filename){
    this->m_impl->writeAdcircMesh(filename);
}
