#include "adversionimpl.h"
#include <algorithm>
#include <iostream>
#include <numeric>
#include "boost/filesystem.hpp"
#include "boost/format.hpp"
#include "boostio.h"
#include "git2.h"
#include "kdtree.h"
#include "metis.h"

#define WRITE_NC 0

AdversionImpl::AdversionImpl(const std::string& meshFilename,
                             const std::string& rootDirectory)
    : m_meshFilename(meshFilename),
      m_rootDirectory(rootDirectory),
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

void AdversionImpl::readPartitionedMesh() {
  std::string meshHeader;
  Partition::Format fmt;
  size_t numNodes, numElements, numOpenBoundaries, numLandBoundaries;
  this->m_mesh.reset(new Adcirc::Geometry::Mesh());

  std::cout << "Reading system data..." << std::endl;
  this->readSystemInformation(fmt, numNodes, numElements, numOpenBoundaries,
                              numLandBoundaries, meshHeader);
  std::cout << numNodes << " " << numElements << std::endl;

  this->m_mesh->setMeshHeaderString(meshHeader);

  this->m_mesh->setNumNodes(numNodes);
  adunordered_map<std::string, size_t> nodeTable;
  nodeTable.reserve(numNodes);
  std::cout << "Reading nodes..." << std::endl;
  this->readMeshNodes(nodeTable);

  std::cout << "Reading elements..." << std::endl;
  this->m_mesh->setNumElements(numElements);
  this->readMeshElements(nodeTable);

  std::cout << "Reading open boundaries..." << std::endl;
  this->m_mesh->setNumOpenBoundaries(numOpenBoundaries);
  this->readOpenBoundaries(nodeTable);

  std::cout << "Reading land boundaries..." << std::endl;
  this->m_mesh->setNumLandBoundaries(numLandBoundaries);
  this->readLandBoundaries(nodeTable);

  return;
}

void AdversionImpl::readMeshNodes(
    adunordered_map<std::string, size_t>& nodeTable) {
  size_t id = 0;
  for (size_t i = 0; i < this->m_numPartitions; ++i) {
    std::string filename =
        boost::str(boost::format("%s/nodes/partition_%06i.node") %
                   this->m_rootDirectory % i);
    std::ifstream f;
    f.open(filename, std::ios::in);
    std::string line;
    std::getline(f, line);
    size_t nn = stoull(line);
    for (size_t i = 0; i < nn; ++i) {
      std::getline(f, line);
      double x, y, z;
      std::string hash;
      bool r = BoostIO::readNodeLine(line, hash, x, y, z);
      Adcirc::Geometry::Node nd(id + 1, x, y, z);
      this->m_mesh->addNode(id, nd);
      nodeTable[hash] = id;
      id++;
    }
    f.close();
  }
  return;
}

void AdversionImpl::readMeshElements(
    adunordered_map<std::string, size_t>& nodeTable) {
  size_t id = 0;
  for (size_t i = 0; i < this->m_numPartitions; ++i) {
    std::string filename =
        boost::str(boost::format("%s/elements/partition_%06i.element") %
                   this->m_rootDirectory % i);
    std::ifstream f;
    f.open(filename, std::ios::in);
    std::string line;
    std::getline(f, line);
    size_t nn = stoull(line);
    for (size_t i = 0; i < nn; ++i) {
      std::getline(f, line);
      std::string hash, n1, n2, n3;
      bool r = BoostIO::readElementLine(line, hash, n1, n2, n3);
      Adcirc::Geometry::Node* nn1 = this->m_mesh->node(nodeTable[n1]);
      Adcirc::Geometry::Node* nn2 = this->m_mesh->node(nodeTable[n2]);
      Adcirc::Geometry::Node* nn3 = this->m_mesh->node(nodeTable[n3]);
      Adcirc::Geometry::Element el(id + 1, nn1, nn2, nn3);
      this->m_mesh->addElement(id, el);
      id++;
    }
    f.close();
  }

  return;
}

void AdversionImpl::readSystemInformation(Partition::Format& fmt,
                                          size_t& numNodes, size_t& numElements,
                                          size_t& numOpenBoundaries,
                                          size_t& numLandBoundaries,
                                          std::string& meshHeader) {
  std::string systemPath = this->m_rootDirectory + "/system";
  std::string metadataPath = systemPath + "/metadata.control";
  std::string partitionPath = systemPath + "/partitions.control";

  std::ifstream m;
  std::string line;
  m.open(metadataPath.c_str(), std::ios::in);
  std::getline(m, meshHeader);
  std::getline(m, line);
  if (line == "ASCII") {
    fmt = Partition::Format::ASCII;
  } else if (line == "NETCDF") {
    fmt = Partition::Format::NETCDF;
  }

  std::getline(m, line);
  numNodes = stoull(line);

  std::getline(m, line);
  numElements = stoull(line);

  std::getline(m,line);
  numOpenBoundaries = stoull(line);

  std::getline(m,line);
  numLandBoundaries = stoull(line);

  m.close();

  std::ifstream p;
  p.open(partitionPath.c_str(), std::ios::in);
  std::getline(p, line);
  this->m_numPartitions = stoull(line);

  this->m_rectangles.clear();
  this->m_rectangles.reserve(this->m_numPartitions);

  for (size_t i = 0; i < this->m_numPartitions; ++i) {
    std::string l;
    std::getline(p, l);
    this->m_rectangles.push_back(BoostIO::readRectangle(l));
  }
  p.close();

  return;
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
  this->buildRectangles(nodePartition, this->m_rectangles);

  std::cout << "Placing nodes..." << std::endl;
  std::vector<Partition> partitions(this->m_numPartitions);
  this->placeNodesInRegions(this->m_rectangles, partitions);

  std::cout << "Placing elements..." << std::endl;
  this->placeElementsInRegions(this->m_rectangles, partitions);

  std::cout << "Sorting..." << std::endl;
  for (auto& p : partitions) {
    p.sort();
  }

  std::cout << "Developing directory structure..." << std::endl;
  this->makeDirectoryStructure(this->m_rootDirectory);

  std::cout << "Writing..." << std::endl;
  this->writePartitionedMesh(this->m_rootDirectory, partitions);
  this->writeSystemInformation(this->m_rootDirectory, this->m_rectangles);
  this->writeBoundaries(this->m_rootDirectory);

  std::cout << "Initializing git structure..." << std::endl;
  this->gitInit();

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
  std::vector<size_t> part(this->m_mesh->numNodes());
  std::fill(found.begin(), found.end(), 0);
  std::fill(part.begin(), part.end(), std::numeric_limits<size_t>::max());

  for (size_t i = 0; i < this->m_numPartitions; ++i) {
    partitions[i].guessSizeNodes(
        2 * (this->m_mesh->numNodes() / this->m_numPartitions));
  }

  //#pragma omp parallel for shared(rectangles, partitions, found, m_mesh)
  for (size_t i = 0; i < this->m_mesh->numNodes(); ++i) {
    for (size_t j = 0; j < this->m_numPartitions; ++j) {
      if (rectangles[j].isInside(this->m_mesh->node(i)->x(),
                                 this->m_mesh->node(i)->y())) {
        //#pragma omp critical
        partitions[j].addNode(this->m_mesh->node(i));
        found[i] = 1;
        part[i] = j;
        break;
      }
    }
  }

  //...We need to account for funky geometries here
  if (std::accumulate(found.begin(), found.end(), 0) !=
      this->m_mesh->numNodes()) {
    this->placeMissingNodes(found, part, partitions);
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

void AdversionImpl::placeMissingNodes(std::vector<unsigned short>& found,
                                      std::vector<size_t>& part,
                                      std::vector<Partition>& partitions) {
  size_t n = std::min(1000ul, this->m_mesh->numNodes());

  if (!this->m_mesh->nodalSearchTreeInitialized())
    this->m_mesh->buildNodalSearchTree();

  Kdtree* k = this->m_mesh->nodalSearchTree();
  for (size_t i = 0; i < this->m_mesh->numNodes(); ++i) {
    if (found[i] == 0) {
      Adcirc::Geometry::Node* nd = this->m_mesh->node(i);
      std::vector<size_t> near = k->findXNearest(nd->x(), nd->y(), n);
      for (size_t j = 1; j < n; ++j) {
        if (found[near[j]] == 1) {
          partitions[part[near[j]]].addNode(nd);
          found[i] = 1;
          part[i] = part[near[j]];
          break;
        }
      }
    }
  }
  return;
}

void AdversionImpl::placeMissingElements(std::vector<unsigned short>& found,
                                         std::vector<size_t>& part,
                                         std::vector<Partition>& partitions) {
  size_t n = std::min(1000ul, this->m_mesh->numElements());

  if (!this->m_mesh->elementalSearchTreeInitialized())
    this->m_mesh->buildElementalSearchTree();

  Kdtree* k = this->m_mesh->elementalSearchTree();
  for (size_t i = 0; i < this->m_mesh->numElements(); ++i) {
    if (found[i] == 0) {
      double xc, yc;
      Adcirc::Geometry::Element* e = this->m_mesh->element(i);
      e->getElementCenter(xc, yc);
      std::vector<size_t> near = k->findXNearest(xc, yc, n);
      for (size_t j = 1; j < n; ++j) {
        if (found[near[j]] == 1) {
          partitions[part[near[j]]].addElement(e);
          found[i] = 1;
          part[i] = part[near[j]];
          break;
        }
      }
    }
  }
  return;
}

void AdversionImpl::placeElementsInRegions(std::vector<Rectangle>& rectangles,
                                           std::vector<Partition>& partitions) {
  std::vector<unsigned short> found(this->m_mesh->numElements());
  std::vector<size_t> part(this->m_mesh->numElements());
  std::fill(found.begin(), found.end(), 0);
  std::fill(part.begin(), part.end(), std::numeric_limits<size_t>::max());

  for (size_t i = 0; i < this->m_numPartitions; ++i) {
    partitions[i].guessSizeElements(
        2 * (this->m_mesh->numElements() / this->m_numPartitions));
  }

  //#pragma omp parallel for shared(rectangles, partitions, found, m_mesh)
  for (size_t i = 0; i < this->m_mesh->numElements(); ++i) {
    double x, y;
    this->m_mesh->element(i)->getElementCenter(x, y);
    for (size_t j = 0; j < this->m_numPartitions; ++j) {
      if (rectangles[j].isInside(x, y)) {
        //#pragma omp critical
        partitions[j].addElement(this->m_mesh->element(i));
        found[i] = 1;
        part[i] = j;
        break;
      }
    }
  }

  //...We need to account for funky geometries here
  if (std::accumulate(found.begin(), found.end(), 0) !=
      this->m_mesh->numElements()) {
    this->placeMissingElements(found, part, partitions);
  }

  return;
}

void AdversionImpl::makeDirectoryStructure(const std::string& rootPath) {
  std::string nodeFolder = rootPath + "/nodes";
  std::string elementFolder = rootPath + "/elements";
  std::string boundaryFolder = rootPath + "/boundaries";
  std::string systemFolder = rootPath + "/system";

  if (!boost::filesystem::exists(rootPath)) {
    boost::filesystem::create_directory(rootPath);
  }
  if (!boost::filesystem::exists(nodeFolder)) {
    boost::filesystem::create_directory(nodeFolder);
  }
  if (!boost::filesystem::exists(elementFolder)) {
    boost::filesystem::create_directory(elementFolder);
  }
  if (!boost::filesystem::exists(boundaryFolder)) {
    boost::filesystem::create_directory(boundaryFolder);
  }
  if (!boost::filesystem::exists(systemFolder)) {
    boost::filesystem::create_directory(systemFolder);
  }
  return;
}

std::string AdversionImpl::rootDirectory() const {
  return this->m_rootDirectory;
}

void AdversionImpl::setRootDirectory(const std::string& rootDirectory) {
  this->m_rootDirectory = rootDirectory;
}

void AdversionImpl::writePartitionedMesh(const std::string& rootPath,
                                         std::vector<Partition>& partitions) {
#if WRITE_NC == 1
  Partition::Format f = Partition::Format::NETCDF;
#else
  Partition::Format f = Partition::Format::ASCII;
#endif
  for (size_t i = 0; i < this->m_numPartitions; ++i) {
    std::string nodesFile = boost::str(
        boost::format("%s/nodes/partition_%06i.node") % rootPath % i);
    std::string elementsFile = boost::str(
        boost::format("%s/elements/partition_%06i.element") % rootPath % i);
    partitions[i].write(f, nodesFile, elementsFile);
  }
  return;
}

void AdversionImpl::gitInit() {
  std::string gitdir = this->m_rootDirectory + "/.git";
  if (boost::filesystem::exists(gitdir)) return;

  git_repository* repo;
  git_libgit2_init();
  git_repository_init(&repo, gitdir.c_str(), 0);
  git_repository_free(repo);
  git_libgit2_shutdown();
  return;
}

void AdversionImpl::writeSystemInformation(const std::string& rootPath,
                                           std::vector<Rectangle>& rect) {
  std::string partitionFile = rootPath + "/system/partitions.control";
  std::string metaFile = rootPath + "/system/metadata.control";

  std::ofstream pFile;
  pFile.open(partitionFile, std::ios::out);
  std::string nDomains =
      boost::str(boost::format("%12i\n") % this->m_numPartitions);
  pFile.write(nDomains.c_str(), nDomains.size());

  for (size_t i = 0; i < rect.size(); ++i) {
    Rectangle* r = &rect[i];
    std::string line = boost::str(
        boost::format("%6i %+018.12e %+018.12e %+018.12e %+018.12e\n") % i %
        r->topLeft().first % r->topLeft().second % r->bottomRight().first %
        r->bottomRight().second);
    pFile.write(line.c_str(), line.size());
  }

  pFile.close();

  std::ofstream mFile;
  mFile.open(metaFile, std::ios::out);
  std::string header = this->m_mesh->meshHeaderString() + "\n";
  mFile.write(header.c_str(), header.size());

#if WRITE_NC == 1
  mFile.write("NETCDF\n", 7);
#else
  mFile.write("ASCII\n", 6);
#endif

  std::string nodes =
      boost::str(boost::format("%12i\n") % this->m_mesh->numNodes());
  std::string elements =
      boost::str(boost::format("%12i\n") % this->m_mesh->numElements());
  std::string openBoundaries =
      boost::str(boost::format("%12i\n") % this->m_mesh->numOpenBoundaries());
  std::string landBoundaries =
      boost::str(boost::format("%12i\n") % this->m_mesh->numLandBoundaries());
  mFile.write(nodes.c_str(), nodes.size());
  mFile.write(elements.c_str(), elements.size());
  mFile.write(openBoundaries.c_str(), openBoundaries.size());
  mFile.write(landBoundaries.c_str(), landBoundaries.size());
  mFile.close();
  return;
}

void AdversionImpl::writeAdcircMesh(const std::string& filename) {
  this->m_mesh->write(filename, Adcirc::Geometry::MeshAdcirc);
}

void AdversionImpl::writeBoundaries(const std::string& rootPath) {
  std::string openBoundariesControl =
      rootPath + "/boundaries/openBoundaries.control";
  std::string landBoundariesControl =
      rootPath + "/boundaries/landBoundaries.control";

  Partition obp;
  for (size_t i = 0; i < this->m_mesh->numOpenBoundaries(); ++i) {
    obp.addBoundary(this->m_mesh->openBoundary(i));
  }
  obp.sort();

  Partition lbp;
  for (size_t i = 0; i < this->m_mesh->numLandBoundaries(); ++i) {
    lbp.addBoundary(this->m_mesh->landBoundary(i));
  }
  lbp.sort();

  std::ofstream obc;
  obc.open(openBoundariesControl);
  std::string nob =
      boost::str(boost::format("%12i\n") % this->m_mesh->numOpenBoundaries());
  obc.write(nob.c_str(), nob.size());
  for (size_t i = 0; i < this->m_mesh->numOpenBoundaries(); ++i) {
    std::string line =
        boost::str(boost::format("%s\n") % obp.boundary(i)->hash());
    obc.write(line.c_str(), line.size());
  }
  obc.close();
  obp.writeBoundariesAscii(this->m_rootDirectory);

  std::ofstream lbc;
  lbc.open(landBoundariesControl);
  std::string nlb =
      boost::str(boost::format("%12i\n") % this->m_mesh->numLandBoundaries());
  lbc.write(nlb.c_str(), nlb.size());
  for (size_t i = 0; i < this->m_mesh->numLandBoundaries(); ++i) {
    std::string line =
        boost::str(boost::format("%s\n") % lbp.boundary(i)->hash());
    lbc.write(line.c_str(), line.size());
  }
  lbc.close();
  lbp.writeBoundariesAscii(this->m_rootDirectory);
  return;
}

void AdversionImpl::readOpenBoundaries(
    adunordered_map<std::string, size_t>& nodeTable) {
  std::ifstream obc;
  obc.open(this->m_rootDirectory + "/boundaries/openBoundaries.control",
           std::ios::in);
  std::string header;
  std::getline(obc, header);
  size_t nobc = stoull(header);
  if (nobc != this->m_mesh->numOpenBoundaries()) {
    std::cerr << "Wrong number of boundaries" << std::endl;
    std::cerr << nobc << " " << this->m_mesh->numOpenBoundaries() << std::endl;
    return;
  }
  for (size_t i = 0; i < this->m_mesh->numOpenBoundaries(); ++i) {
    std::string hash;
    std::getline(obc, hash);
    std::string filename =
        this->m_rootDirectory + "/boundaries/" + hash + ".bnd";
    std::ifstream file;
    file.open(filename, std::ios::in);
    std::string line;
    std::getline(file, line);

    int boundaryCode;
    size_t boundarySize;
    BoostIO::readBoundaryHeader(line, boundaryCode, boundarySize);
    Adcirc::Geometry::Boundary b(boundaryCode, boundarySize);

    for (size_t j = 0; j < boundarySize; ++j) {
      std::getline(file, line);
      Adcirc::Geometry::Node* n = this->m_mesh->node(nodeTable[line]);
      b.setNode1(j, n);
    }
    file.close();
    this->m_mesh->addOpenBoundary(i, b);
  }
  return;
}

void AdversionImpl::readLandBoundaries(
    adunordered_map<std::string, size_t>& nodeTable) {
  std::ifstream lbc;
  lbc.open(this->m_rootDirectory + "/boundaries/landBoundaries.control",
           std::ios::in);
  std::string header;
  std::getline(lbc, header);
  size_t nlbc = stoull(header);
  if (nlbc != this->m_mesh->numLandBoundaries()) {
    std::cerr << "Wrong number of boundaries" << std::endl;
    return;
  }
  for (size_t i = 0; i < this->m_mesh->numLandBoundaries(); ++i) {
    std::string hash;
    std::getline(lbc, hash);
    std::string filename =
        this->m_rootDirectory + "/boundaries/" + hash + ".bnd";
    std::ifstream file;
    file.open(filename, std::ios::in);
    std::string line;
    std::getline(file, line);

    int boundaryCode;
    size_t boundarySize;
    BoostIO::readBoundaryHeader(line, boundaryCode, boundarySize);
    Adcirc::Geometry::Boundary b(boundaryCode, boundarySize);

    for (size_t j = 0; j < boundarySize; ++j) {
      std::getline(file, line);
      if (b.isExternalWeir()) {
        std::string n1;
        double crest, super;
        BoostIO::readBoundaryExternalWeirBoundary(line, n1, crest, super);
        Adcirc::Geometry::Node* n = this->m_mesh->node(nodeTable[n1]);
        b.setNode1(j, n);
        b.setCrestElevation(j, crest);
        b.setSupercriticalWeirCoefficient(j, super);
      } else if (b.isInternalWeirWithoutPipes()) {
        std::string n1, n2;
        double crest, super, sub;
        BoostIO::readBoundaryInternalWeirBoundary(line, n1, n2, crest, super,
                                                  sub);
        Adcirc::Geometry::Node* nfront = this->m_mesh->node(nodeTable[n1]);
        Adcirc::Geometry::Node* nback = this->m_mesh->node(nodeTable[n2]);
        b.setNode1(j, nfront);
        b.setNode2(j, nback);
        b.setCrestElevation(j, crest);
        b.setSupercriticalWeirCoefficient(j, super);
        b.setSubcriticalWeirCoefficient(j, sub);
      } else if (b.isInternalWeirWithPipes()) {
        std::string n1, n2;
        double crest, super, sub, diam, coef, ht;
        BoostIO::readBoundaryInternalWeirWithPipes(line, n1, n2, crest, super,
                                                   sub, diam, coef, ht);
        Adcirc::Geometry::Node* nfront = this->m_mesh->node(nodeTable[n1]);
        Adcirc::Geometry::Node* nback = this->m_mesh->node(nodeTable[n2]);
        b.setNode1(j, nfront);
        b.setNode2(j, nback);
        b.setCrestElevation(j, crest);
        b.setSupercriticalWeirCoefficient(j, super);
        b.setSubcriticalWeirCoefficient(j, sub);
        b.setPipeHeight(j, ht);
        b.setPipeDiameter(j, diam);
        b.setPipeCoefficient(j, coef);
      } else {
        std::string n1;
        BoostIO::readBoundarySingleNode(line, n1);
        Adcirc::Geometry::Node* n = this->m_mesh->node(nodeTable[n1]);
        b.setNode1(j, n);
      }
    }
    file.close();
    this->m_mesh->addLandBoundary(i, b);
  }
}
