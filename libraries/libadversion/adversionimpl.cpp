#include "adversionimpl.h"
#include <iostream>

AdversionImpl::AdversionImpl(const std::string& filename)
    : m_meshFilename(filename),
      m_nodalAttributesFilename(std::string()),
      m_mesh(nullptr) {}

std::string AdversionImpl::meshFilename() const { return this->m_meshFilename; }

void AdversionImpl::setMeshFilename(const std::string& meshFilename) {
  this->m_meshFilename = meshFilename;
}

std::string AdversionImpl::nodalAttributesFilename() const {
  return m_nodalAttributesFilename;
}

void AdversionImpl::setNodalAttributesFilename(
    const std::string& nodalAttributesFilename) {
  m_nodalAttributesFilename = nodalAttributesFilename;
}

void AdversionImpl::partitionMesh(size_t nPartitions) {
  if (this->m_meshFilename == std::string()) return;

  if (nPartitions != 0) {
    this->setNumPartitions(nPartitions);
  }

  if (this->m_numPartitions == 0) return;

  std::cout << "Reading..." << std::endl;
  this->m_mesh.reset(new Adcirc::Geometry::Mesh(this->m_meshFilename));
  this->m_mesh->read();

  std::vector<size_t> nodePartition;
  std::vector<size_t> elemPartition;

  std::cout << "Partitioning..." << std::endl;
  this->metisPartition(this->m_numPartitions, nodePartition, elemPartition);

  return;
}

std::vector<std::unique_ptr<Adcirc::Geometry::Element>>
AdversionImpl::generateWeirConnectingElements() {
  std::vector<std::unique_ptr<Adcirc::Geometry::Element>> connections;

  connections.reserve(2 * this->m_mesh->totalLandBoundaryNodes());
  size_t start = this->m_mesh->numElements();

  size_t count = 0;
  for (size_t i = 0; i < this->m_mesh->numLandBoundaries(); ++i) {
    if (this->m_mesh->landBoundary(i)->isInternalWeir()) {
      for (size_t j = 0; j < this->m_mesh->landBoundary(i)->size() - 1; j++) {
        Adcirc::Geometry::Node* n1 = this->m_mesh->landBoundary(i)->node1(j);
        Adcirc::Geometry::Node* n2 = this->m_mesh->landBoundary(i)->node2(j);
        Adcirc::Geometry::Node* n3 =
            this->m_mesh->landBoundary(i)->node1(j + 1);
        Adcirc::Geometry::Node* n4 =
            this->m_mesh->landBoundary(i)->node2(j + 1);

        count++;
        connections.push_back(std::unique_ptr<Adcirc::Geometry::Element>(
            new Adcirc::Geometry::Element(start + count, n1, n2, n3)));
        count++;
        connections.push_back(std::unique_ptr<Adcirc::Geometry::Element>(
            new Adcirc::Geometry::Element(start + count, n4, n3, n2)));
      }
    }
  }
  return connections;
}

void AdversionImpl::metisPartition(size_t nPartitions,
                                   std::vector<size_t>& nodePartition,
                                   std::vector<size_t>& elementPartition) {
  std::vector<std::unique_ptr<Adcirc::Geometry::Element>> addedElements =
      generateWeirConnectingElements();

  idx_t nn = static_cast<idx_t>(this->m_mesh->numNodes());
  idx_t ne =
      static_cast<idx_t>(this->m_mesh->numElements() + addedElements.size());
  idx_t numflag = 0;
  idx_t numPartitions = static_cast<idx_t>(nPartitions);

  idx_t* eptr = new idx_t[ne + 1];
  idx_t* eind = new idx_t[ne * 3];
  idx_t* npart = new idx_t[nn];
  idx_t* epart = new idx_t[ne];

  idx_t index = 0;
  for (idx_t i = 0; i < this->m_mesh->numElements(); ++i) {
    eind[index] = this->m_mesh->element(i)->node(0)->id() - 1;
    eind[index + 1] = this->m_mesh->element(i)->node(1)->id() - 1;
    eind[index + 2] = this->m_mesh->element(i)->node(2)->id() - 1;
    eptr[i] = index;
    index += 3;
  }
  for (idx_t i = 0; i < addedElements.size(); ++i) {
    eind[index] = addedElements[i]->node(0)->id() - 1;
    eind[index + 1] = addedElements[i]->node(1)->id() - 1;
    eind[index + 2] = addedElements[i]->node(2)->id() - 1;
    eptr[this->m_mesh->numElements() + i] = index;
    index += 3;
  }
  eptr[ne] = index;

  idx_t metis_options[METIS_NOPTIONS];
  int ierr = METIS_SetDefaultOptions(metis_options);

  idx_t *xadj, *adj;
  idx_t nEdgeCut;

  ierr = METIS_MeshToNodal(&ne, &nn, eptr, eind, &numflag, &xadj, &adj);
  ierr = METIS_PartMeshNodal(&ne, &nn, eptr, eind, nullptr, nullptr,
                             &numPartitions, nullptr, metis_options, &nEdgeCut,
                             epart, npart);

  nodePartition.resize(this->m_mesh->numNodes());
  for (size_t i = 0; i < this->m_mesh->numNodes(); ++i) {
    nodePartition[i] = npart[i];
  }

  elementPartition.resize(this->m_mesh->numElements());
  for (size_t i = 0; i < this->m_mesh->numElements(); ++i) {
    elementPartition[i] = epart[i];
  }

  delete[] eptr;
  delete[] eind;
  delete[] npart;
  delete[] epart;

  METIS_Free(xadj);
  METIS_Free(adj);

  return;
}

size_t AdversionImpl::numPartitions() const { return m_numPartitions; }

void AdversionImpl::setNumPartitions(const size_t& numPartitions) {
  this->m_numPartitions = numPartitions;
}
