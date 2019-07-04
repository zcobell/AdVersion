#include "rectangleimpl.h"

RectangleImpl::RectangleImpl() {}

RectangleImpl::RectangleImpl(double xTopLeft, double yTopLeft,
                             double xBottomRight, double yBottomRight) {
  this->generatePolygon(xTopLeft, yTopLeft, xBottomRight, yBottomRight);
}

void RectangleImpl::setRectangle(double xTopLeft, double yTopLeft,
                                 double xBottomRight, double yBottomRight) {
  this->generatePolygon(xTopLeft, yTopLeft, xBottomRight, yBottomRight);
}

void RectangleImpl::generatePolygon(double xTopLeft, double yTopLeft,
                                    double xBottomRight, double yBottomRight) {
  this->m_topLeft = point_t(xTopLeft, yTopLeft);
  this->m_topRight = point_t(xBottomRight, yTopLeft);
  this->m_bottomRight = point_t(xBottomRight, yBottomRight);
  this->m_bottomLeft = point_t(xTopLeft, yBottomRight);
  this->buildPolygon();
  return;
}

void RectangleImpl::buildPolygon() {
  this->m_polygon.clear();
  boost::geometry::append(this->m_polygon, this->m_topLeft);
  boost::geometry::append(this->m_polygon, this->m_topRight);
  boost::geometry::append(this->m_polygon, this->m_bottomRight);
  boost::geometry::append(this->m_polygon, this->m_bottomLeft);
  boost::geometry::append(this->m_polygon, this->m_topLeft);
  return;
}

std::pair<double, double> RectangleImpl::topLeft() {
  return std::pair<double, double>(this->m_topLeft.get<0>(),
                                   this->m_topLeft.get<1>());
}

std::pair<double, double> RectangleImpl::topRight() {
  return std::pair<double, double>(this->m_topRight.get<0>(),
                                   this->m_topRight.get<1>());
}

std::pair<double, double> RectangleImpl::bottomLeft() {
  return std::pair<double, double>(this->m_bottomLeft.get<0>(),
                                   this->m_bottomLeft.get<1>());
}

std::pair<double, double> RectangleImpl::bottomRight() {
  return std::pair<double, double>(this->m_bottomRight.get<0>(),
                                   this->m_bottomRight.get<1>());
}

void RectangleImpl::setTopLeft(double x, double y) {
  this->m_topLeft = point_t(x, y);
  this->buildPolygon();
}

void RectangleImpl::setTopRight(double x, double y) {
  this->m_topRight = point_t(x, y);
  this->buildPolygon();
}

void RectangleImpl::setBottomLeft(double x, double y) {
  this->m_bottomLeft = point_t(x, y);
  this->buildPolygon();
}

void RectangleImpl::setBottomRight(double x, double y) {
  this->m_bottomRight = point_t(x, y);
  this->buildPolygon();
}

bool RectangleImpl::isInside(double x, double y) {
  return boost::geometry::covered_by(point_t(x, y), this->m_polygon);
}

double RectangleImpl::area() { return boost::geometry::area(this->m_polygon); }
