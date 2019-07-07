#include "partition.h"
#include <algorithm>
#include <fstream>
#include "boost/format.hpp"

#if 0 
#include "netcdf.h"
#endif

bool nodeHashLessThan(Adcirc::Geometry::Node *n1, Adcirc::Geometry::Node *n2) {
  return n1->positionHash() < n2->positionHash();
}

bool elementHashLessThan(Adcirc::Geometry::Element *e1,
                         Adcirc::Geometry::Element *e2) {
  return e1->hash() < e2->hash();
}

bool boundaryHashLessThan(Adcirc::Geometry::Boundary *b1,
                          Adcirc::Geometry::Boundary *b2) {
  return b1->hash() < b2->hash();
}

Partition::Partition() {}

void Partition::addNode(Adcirc::Geometry::Node *n) {
  this->m_nodes.push_back(n);
}

void Partition::addElement(Adcirc::Geometry::Element *e) {
  this->m_elements.push_back(e);
}

void Partition::addBoundary(Adcirc::Geometry::Boundary *b) {
  this->m_boundaries.push_back(b);
}

void Partition::guessSizeNodes(size_t size) {
  if (this->m_nodes.size() < size) this->m_nodes.reserve(size);
}

void Partition::guessSizeElements(size_t size) {
  if (this->m_elements.size() < size) this->m_elements.reserve(size);
}

void Partition::guessSizeBoundaries(size_t size) {
  if (this->m_boundaries.size() < size) this->m_boundaries.reserve(size);
}

size_t Partition::numNodes() const { return this->m_nodes.size(); }

size_t Partition::numElements() const { return this->m_elements.size(); }

size_t Partition::numBoundaries() const { return this->m_boundaries.size(); }

void Partition::sort() {
  if (this->numNodes() != 0) this->sortNodes();
  if (this->numElements() != 0) this->sortElements();
  if (this->numBoundaries() != 0) this->sortBoundaries();
}

Adcirc::Geometry::Boundary *Partition::boundary(size_t index) {
  return this->m_boundaries[index];
}

Adcirc::Geometry::Element *Partition::element(size_t index) {
  return this->m_elements[index];
}

Adcirc::Geometry::Node *Partition::node(size_t index) {
  return this->m_nodes[index];
}

void Partition::sortNodes() {
  std::sort(this->m_nodes.begin(), this->m_nodes.end(), nodeHashLessThan);
}

void Partition::sortElements() {
  std::sort(this->m_elements.begin(), this->m_elements.end(),
            elementHashLessThan);
}

void Partition::sortBoundaries() {
  std::sort(this->m_boundaries.begin(), this->m_boundaries.end(),
            boundaryHashLessThan);
}

void Partition::write(Format writeFormat, const std::string &nodesFilename,
                      const std::string &elementsFilename) {
  if (writeFormat == NETCDF) {
    this->writeNetCDF(nodesFilename, elementsFilename);
  } else if (writeFormat == ASCII) {
    this->writeAscii(nodesFilename, elementsFilename);
  }
  return;
}

void Partition::writeAscii(const std::string &nodesFilename,
                           const std::string &elementsFilename) {
  this->writeNodesAscii(nodesFilename);
  this->writeElementsAscii(elementsFilename);
  return;
}

void Partition::writeBoundariesAscii(const std::string &rootDirectory) {
  for (size_t i = 0; i < this->m_boundaries.size(); ++i) {
    this->writeAdcircBoundaryAscii(rootDirectory, this->m_boundaries[i]);
  }
  return;
}

void Partition::writeNetCDF(const std::string &nodeFilename,
                            const std::string &elementsFilename) {
  this->writeNodesNetCDF(nodeFilename);
  this->writeElementsNetCDF(elementsFilename);
}

void Partition::writeNodesAscii(const std::string &filename) {
  std::ofstream f;
  f.open(filename.c_str(), std::ios::out);
  std::string n = boost::str(boost::format("%i\n") % this->numNodes());
  f.write(n.c_str(), n.size());
  for (auto &n : this->m_nodes) {
    std::string line =
        boost::str(boost::format("%s %+018.12e %+018.12e %+018.12e\n") %
                   n->positionHash() % n->x() % n->y() % n->z());
    f.write(line.c_str(), line.size());
  }
  f.close();
}

void Partition::writeElementsAscii(const std::string &filename) {
  std::ofstream f;
  f.open(filename.c_str(), std::ios::out);
  std::string n = boost::str(boost::format("%i\n") % this->numElements());
  f.write(n.c_str(), n.size());
  for (auto &e : this->m_elements) {
    std::string line =
        boost::str(boost::format("%s %s %s %s\n") % e->hash() %
                   e->node(0)->positionHash() % e->node(1)->positionHash() %
                   e->node(2)->positionHash());
    f.write(line.c_str(), line.size());
  }
  f.close();
}

void Partition::writeNodesNetCDF(const std::string &filename) {
#if 0
  int ncid, dimid_numnodes, dimid_hashlen;
  int varid_x, varid_y, varid_z, varid_hash;
  const size_t hashsize = this->m_nodes[0]->positionHash().size();
  int ierr = nc_create(filename.c_str(), NC_NETCDF4, &ncid);
  ierr = nc_def_dim(ncid, "numNodes", this->numNodes(), &dimid_numnodes);
  ierr = nc_def_dim(ncid, "hashlength", hashsize, &dimid_hashlen);
  ierr = nc_def_var(ncid, "x", NC_DOUBLE, 1, &dimid_numnodes, &varid_x);
  ierr = nc_def_var(ncid, "y", NC_DOUBLE, 1, &dimid_numnodes, &varid_y);
  ierr = nc_def_var(ncid, "z", NC_DOUBLE, 1, &dimid_numnodes, &varid_z);
  const int dims[2] = {dimid_numnodes, dimid_hashlen};
  ierr = nc_def_var(ncid, "hash", NC_CHAR, 2, dims, &varid_hash);
  ierr = nc_def_var_deflate(ncid, varid_x, 1, 1, 2);
  ierr = nc_def_var_deflate(ncid, varid_y, 1, 1, 2);
  ierr = nc_def_var_deflate(ncid, varid_z, 1, 1, 2);
  ierr = nc_def_var_deflate(ncid, varid_hash, 1, 1, 2);
  ierr = nc_enddef(ncid);

  double *x = new double[this->numNodes()];
  double *y = new double[this->numNodes()];
  double *z = new double[this->numNodes()];

  for (size_t i = 0; i < this->numNodes(); ++i) {
    x[i] = this->m_nodes[i]->x();
    y[i] = this->m_nodes[i]->y();
    z[i] = this->m_nodes[i]->z();
  }

  ierr = nc_put_var_double(ncid, varid_x, x);
  ierr = nc_put_var_double(ncid, varid_y, y);
  ierr = nc_put_var_double(ncid, varid_z, z);

  delete[] x;
  delete[] y;
  delete[] z;

  char *buf = new char[this->numNodes() * hashsize];
  memset(buf, '\0', this->numNodes() * hashsize);

  for (size_t i = 0; i < this->numNodes(); ++i) {
    strncpy(&buf[i * hashsize], this->m_nodes[i]->positionHash().c_str(),
            hashsize);
  }
  ierr = nc_put_var_text(ncid, varid_hash, buf);

  delete[] buf;

  ierr = nc_close(ncid);
#endif
}

void Partition::writeElementsNetCDF(const std::string &filename) {
#if 0
  int ncid, dimid_numelements, dimid_hashlen;
  int varid_hash, varid_n1, varid_n2, varid_n3;
  const size_t hashsize = this->m_elements[0]->hash().size();
  int ierr = nc_create(filename.c_str(), NC_NETCDF4, &ncid);
  ierr =
      nc_def_dim(ncid, "numElements", this->numElements(), &dimid_numelements);
  ierr = nc_def_dim(ncid, "hashlength", hashsize, &dimid_hashlen);
  const int dims[2] = {dimid_numelements, dimid_hashlen};
  ierr = nc_def_var(ncid, "hash", NC_CHAR, 2, dims, &varid_hash);
  ierr = nc_def_var(ncid, "n1", NC_CHAR, 2, dims, &varid_n1);
  ierr = nc_def_var(ncid, "n2", NC_CHAR, 2, dims, &varid_n2);
  ierr = nc_def_var(ncid, "n3", NC_CHAR, 2, dims, &varid_n3);
  ierr = nc_def_var_deflate(ncid, varid_hash, 1, 1, 2);
  ierr = nc_def_var_deflate(ncid, varid_n1, 1, 1, 2);
  ierr = nc_def_var_deflate(ncid, varid_n2, 1, 1, 2);
  ierr = nc_def_var_deflate(ncid, varid_n3, 1, 1, 2);
  ierr = nc_enddef(ncid);

  char *buf = new char[this->numElements() * hashsize];
  memset(buf, '\0', this->numElements() * hashsize);

  for (size_t i = 0; i < this->numElements(); ++i) {
    strncpy(&buf[i * hashsize], this->m_elements[i]->hash().c_str(), hashsize);
  }
  ierr = nc_put_var_text(ncid, varid_hash, buf);

  for (size_t i = 0; i < this->numElements(); ++i) {
    strncpy(&buf[i * hashsize],
            this->m_elements[i]->node(0)->positionHash().c_str(), hashsize);
  }
  ierr = nc_put_var_text(ncid, varid_n1, buf);

  for (size_t i = 0; i < this->numElements(); ++i) {
    strncpy(&buf[i * hashsize],
            this->m_elements[i]->node(1)->positionHash().c_str(), hashsize);
  }
  ierr = nc_put_var_text(ncid, varid_n2, buf);

  for (size_t i = 0; i < this->numElements(); ++i) {
    strncpy(&buf[i * hashsize],
            this->m_elements[i]->node(2)->positionHash().c_str(), hashsize);
  }
  ierr = nc_put_var_text(ncid, varid_n3, buf);

  delete[] buf;

  nc_close(ncid);
#endif
}

void Partition::writeAdcircBoundaryAscii(const std::string &rootDirectory,
                                         Adcirc::Geometry::Boundary *b) {
  std::string fname = rootDirectory + "/boundaries/" + b->hash() + ".bnd";
  std::ofstream f;
  f.open(fname, std::ios::out);
  std::string header =
      boost::str(boost::format("%12i %12i\n") % b->size() % b->boundaryCode());
  f.write(header.c_str(), header.size());

  for (size_t i = 0; i < b->length(); ++i) {
    std::string line;
    if (b->boundaryCode() == -1 || b->isSingleNodeBoundary()) {
      line = boost::str(boost::format("%s\n") % b->node1(i)->positionHash());
    } else if (b->isExternalWeir()) {
      line = boost::str(boost::format("%s %+018.12e %+018.12e\n") %
                        b->node1(i)->positionHash() % b->crestElevation(i) %
                        b->supercriticalWeirCoefficient(i));
    } else if (b->isInternalWeirWithoutPipes()) {
      line =
          boost::str(boost::format("%s %s %+018.12e %+018.12e %+018.12e\n") %
                     b->node1(i)->positionHash() % b->node2(i)->positionHash() %
                     b->crestElevation(i) % b->subcriticalWeirCoefficient(i) %
                     b->supercriticalWeirCoefficient(i));
    } else if (b->isInternalWeirWithPipes()) {
      line =
          boost::str(boost::format("%s %s %+018.12e %+018.12e %+018.12e "
                                   "%+018.12e %+018.12e %+018.12e\n") %
                     b->node1(i)->positionHash() % b->node2(i)->positionHash() %
                     b->crestElevation(i) % b->subcriticalWeirCoefficient(i) %
                     b->supercriticalWeirCoefficient(i) % b->pipeHeight(i) %
                     b->pipeCoefficient(i) % b->pipeDiameter(i));
    } else {
      line = boost::str(boost::format("%s\n") % b->node1(i)->hash());
    }
    f.write(line.c_str(), line.size());
  }
  f.close();
  return;
}
