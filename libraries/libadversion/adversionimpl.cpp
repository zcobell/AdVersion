#include "adversionimpl.h"
#include <algorithm>
#include <iostream>
#include <numeric>

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
  this->metisPartition(nodePartition, elemPartition);

  std::cout << "Building regions..." << std::endl;
  std::vector<Rectangle> rectangles;
  this->buildRectangles(nodePartition, rectangles);

  std::cout << "Placing nodes..." << std::endl;
  std::vector<Partition> partitions(this->m_numPartitions);
  this->placeNodesInRegions(rectangles, partitions);

  std::cout << "Placing elements..." << std::endl;
  this->placeElementsInRegions(rectangles, partitions);

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

void AdversionImpl::metisPartition(std::vector<size_t>& nodePartition,
                                   std::vector<size_t>& elementPartition) {
  std::vector<std::unique_ptr<Adcirc::Geometry::Element>> addedElements =
      generateWeirConnectingElements();

  idx_t nn = static_cast<idx_t>(this->m_mesh->numNodes());
  idx_t ne =
      static_cast<idx_t>(this->m_mesh->numElements() + addedElements.size());
  idx_t numflag = 0;
  idx_t numPartitions = static_cast<idx_t>(this->m_numPartitions);

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

void AdversionImpl::buildRectangles(std::vector<size_t>& nodePartition,
                                    std::vector<Rectangle>& rectangles) {
  struct minmax {
    double xmin = std::numeric_limits<double>::max();
    double xmax = -std::numeric_limits<double>::max();
    double ymin = std::numeric_limits<double>::max();
    double ymax = -std::numeric_limits<double>::max();
  };

  std::vector<minmax> bounds;
  bounds.resize(this->m_numPartitions);

  for (size_t i = 0; i < this->m_mesh->numNodes(); ++i) {
    size_t p = nodePartition[i];
    double x = this->m_mesh->node(i)->x();
    double y = this->m_mesh->node(i)->y();
    if (x < bounds[p].xmin) bounds[p].xmin = x;
    if (x > bounds[p].xmax) bounds[p].xmax = x;
    if (y < bounds[p].ymin) bounds[p].ymin = y;
    if (y > bounds[p].ymax) bounds[p].ymax = y;
  }

  rectangles.resize(this->m_numPartitions);
  for (size_t i = 0; i < this->m_numPartitions; ++i) {
    rectangles[i].setRectangle(bounds[i].xmin, bounds[i].ymax, bounds[i].xmax,
                               bounds[i].ymin);
  }

  std::sort(rectangles.begin(), rectangles.end(), Rectangle::areaLessThan);

  return;
}

void AdversionImpl::placeNodesInRegions(std::vector<Rectangle>& rectangles,
                                        std::vector<Partition>& partitions) {
  std::vector<unsigned short> found(this->m_mesh->numNodes());
  std::fill(found.begin(), found.end(), 0);

  for (size_t i = 0; i < this->m_numPartitions; ++i) {
    partitions[i].guessSizeNodes(
        2 * (this->m_mesh->numNodes() / this->m_numPartitions));
  }

#pragma omp parallel for shared(rectangles, partitions, found, m_mesh)
  for (size_t i = 0; i < this->m_mesh->numNodes(); ++i) {
    for (size_t j = 0; j < this->m_numPartitions; ++j) {
      if (rectangles[j].isInside(this->m_mesh->node(i)->x(),
                                 this->m_mesh->node(i)->y())) {
#pragma omp critical
        partitions[j].addNode(this->m_mesh->node(i));
        found[i] = 1;
        break;
      }
    }
  }

  //...We need to account for funky geometries here
  if (std::accumulate(found.begin(), found.end(), 0) !=
      this->m_mesh->numNodes()) {
    Kdtree k;
    this->generateRectangleKdtree(rectangles, k);
    for (size_t i = 0; i < this->m_mesh->numNodes(); ++i) {
      if (found[i] == 0) {
        size_t idx = k.findNearest(this->m_mesh->node(i)->x(),
                                   this->m_mesh->node(i)->y());
        partitions[idx].addElement(this->m_mesh->element(i));
        std::cout << this->m_mesh->node(i)->x() << " "
                  << this->m_mesh->node(i)->y() << " " << idx << std::endl;
      }
    }
  }
  return;
}

void AdversionImpl::generateRectangleKdtree(std::vector<Rectangle>& rectangles,
                                            Kdtree& k) {
  std::vector<double> x, y;
  x.reserve(this->m_numPartitions);
  y.reserve(this->m_numPartitions);

  for (auto& r : rectangles) {
    x.push_back((r.topLeft().first + r.topRight().first) / 2.0);
    y.push_back((r.topLeft().second + r.bottomLeft().second) / 2.0);
  }
  k.build(x, y);
  return;
}

void AdversionImpl::placeElementsInRegions(std::vector<Rectangle>& rectangles,
                                           std::vector<Partition>& partitions) {
  std::vector<unsigned short> found(this->m_mesh->numElements());
  std::fill(found.begin(), found.end(), 0);

  for (size_t i = 0; i < this->m_numPartitions; ++i) {
    partitions[i].guessSizeElements(
        2 * (this->m_mesh->numElements() / this->m_numPartitions));
  }

#pragma omp parallel for shared(rectangles, partitions, found, m_mesh)
  for (size_t i = 0; i < this->m_mesh->numElements(); ++i) {
    double x, y;
    this->m_mesh->element(i)->getElementCenter(x, y);
    for (size_t j = 0; j < this->m_numPartitions; ++j) {
      if (rectangles[j].isInside(x, y)) {
#pragma omp critical
        partitions[j].addElement(this->m_mesh->element(i));
        found[i] = 1;
        break;
      }
    }
  }

  //...We need to account for funky geometries here
  if (std::accumulate(found.begin(), found.end(), 0) !=
      this->m_mesh->numElements()) {
    size_t n = std::min(200ul, this->m_mesh->numElements());
    this->m_mesh->buildElementalSearchTree();
    Kdtree* k = this->m_mesh->elementalSearchTree();
    for (size_t i = 0; i < this->m_mesh->numElements(); ++i) {
      if (found[i] == 0) {
        double xc, yc;
        this->m_mesh->element(i)->getElementCenter(xc, yc);
        std::vector<size_t> near = k->findXNearest(xc, yc, n);
        for (size_t j = 1; j < this->m_mesh->numElements(); ++j) {
          if (found[near[j]] == 1) {
            partitions[near[j]].addElement(this->m_mesh->element(i));
          }
        }
      }
    }
  }

  if (std::accumulate(found.begin(), found.end(), 0) !=
      this->m_mesh->numElements()) {
    std::cerr << "fuck really?" << std::endl;
  }

  return;
}
