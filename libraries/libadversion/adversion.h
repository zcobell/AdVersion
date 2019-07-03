#ifndef ADVERSION_H
#define ADVERSION_H

#include <memory>
#include <string>

class AdversionImpl;

class AdVersion
{
public:
    AdVersion(std::string filename = std::string());

    ~AdVersion();

    void partitionMesh(size_t nPartitions = 0);

    std::string meshFilename() const;
    void setMeshFilename(const std::string &meshFilename);

    std::string nodalAttributesFilename() const;
    void setNodalAttributesFilename(const std::string &nodalAttributesFilename);

    size_t numPartitions() const;
    void setNumPartitions(const size_t &numPartitions);
private:
    std::unique_ptr<AdversionImpl> m_impl;
};

#endif // ADVERSION_H
