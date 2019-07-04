#ifndef RECTANGLEIMPL_H
#define RECTANGLEIMPL_H

#include "boost/geometry.hpp"

class RectangleImpl {
 public:
  RectangleImpl();
  RectangleImpl(double xTopLeft, double yTopLeft, double xBottomRight,
                double yBottomRight);

  void setRectangle(double xTopLeft, double yTopLeft, double xBottomRight,
                    double yBottomRight);

  std::pair<double, double> topLeft();
  std::pair<double, double> topRight();
  std::pair<double, double> bottomLeft();
  std::pair<double, double> bottomRight();

  void setTopLeft(double x, double y);
  void setTopRight(double x, double y);
  void setBottomLeft(double x, double y);
  void setBottomRight(double x, double y);

  bool isInside(double x, double y);

  double area();

 private:
  void generatePolygon(double xTopLeft, double yTopLeft, double xBottomRight,
                       double yBottomRight);
  void buildPolygon();

  typedef boost::geometry::model::point<double, 2,
                                        boost::geometry::cs::cartesian>
      point_t;
  typedef boost::geometry::model::polygon<point_t> polygon_t;

  point_t m_topLeft;
  point_t m_topRight;
  point_t m_bottomLeft;
  point_t m_bottomRight;
  polygon_t m_polygon;

};

#endif  // RECTANGLEIMPL_H
