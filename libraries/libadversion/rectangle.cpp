#include "rectangle.h"

Rectangle::Rectangle() {}

Rectangle::Rectangle(double xTopLeft, double yTopLeft, double xBottomRight,
                     double yBottomRight) {
  this->generatePolygon(xTopLeft, yTopLeft, xBottomRight, yBottomRight);
}

bool Rectangle::areaLessThan(Rectangle &r1, Rectangle &r2) {
  return r1.area() < r2.area();
}

bool Rectangle::areaGreaterThan(Rectangle &r1, Rectangle &r2) {
  return r1.area() > r2.area();
}

void Rectangle::setRectangle(double xTopLeft, double yTopLeft,
                             double xBottomRight, double yBottomRight) {
  this->generatePolygon(xTopLeft, yTopLeft, xBottomRight, yBottomRight);
}

void Rectangle::generatePolygon(double xTopLeft, double yTopLeft,
                                double xBottomRight, double yBottomRight) {
  this->m_topLeft = point_t(xTopLeft, yTopLeft);
  this->m_topRight = point_t(xBottomRight, yTopLeft);
  this->m_bottomRight = point_t(xBottomRight, yBottomRight);
  this->m_bottomLeft = point_t(xTopLeft, yBottomRight);
  this->buildPolygon();
  return;
}

void Rectangle::buildPolygon() {
  this->m_polygon.clear();
  boost::geometry::append(this->m_polygon, this->m_topLeft);
  boost::geometry::append(this->m_polygon, this->m_topRight);
  boost::geometry::append(this->m_polygon, this->m_bottomRight);
  boost::geometry::append(this->m_polygon, this->m_bottomLeft);
  boost::geometry::append(this->m_polygon, this->m_topLeft);
  return;
}

std::pair<double, double> Rectangle::topLeft() {
  return std::pair<double, double>(this->m_topLeft.get<0>(),
                                   this->m_topLeft.get<1>());
}

std::pair<double, double> Rectangle::topRight() {
  return std::pair<double, double>(this->m_topRight.get<0>(),
                                   this->m_topRight.get<1>());
}

std::pair<double, double> Rectangle::bottomLeft() {
  return std::pair<double, double>(this->m_bottomLeft.get<0>(),
                                   this->m_bottomLeft.get<1>());
}

std::pair<double, double> Rectangle::bottomRight() {
  return std::pair<double, double>(this->m_bottomRight.get<0>(),
                                   this->m_bottomRight.get<1>());
}

void Rectangle::setTopLeft(double x, double y) {
  this->m_topLeft = point_t(x, y);
  this->buildPolygon();
}

void Rectangle::setTopRight(double x, double y) {
  this->m_topRight = point_t(x, y);
  this->buildPolygon();
}

void Rectangle::setBottomLeft(double x, double y) {
  this->m_bottomLeft = point_t(x, y);
  this->buildPolygon();
}

void Rectangle::setBottomRight(double x, double y) {
  this->m_bottomRight = point_t(x, y);
  this->buildPolygon();
}

bool Rectangle::isInside(double x, double y) {
  return boost::geometry::covered_by(point_t(x, y), this->m_polygon);
}

double Rectangle::area() { return boost::geometry::area(this->m_polygon); }

std::pair<double, double> Rectangle::centroid() {
  double x = (this->topLeft().first + this->topRight().first) / 2.0;
  double y = (this->topLeft().second + this->bottomLeft().second) / 2.0;
  return std::pair<double, double>(x, y);
}
