#ifndef RECTANGLE_H
#define RECTANGLE_H

#include "boost/geometry.hpp"

class Rectangle {
 public:
  Rectangle();
  Rectangle(double xTopLeft, double yTopLeft, double xBottomRight,
            double yBottomRight);

  void setRectangle(double xTopLeft, double yTopLeft, double xBottomRight,
                    double yBottomRight);


  static bool areaLessThan(const Rectangle &r1, const Rectangle &r2);
  static bool areaGreaterThan(const Rectangle &r1, const Rectangle &r2);

  std::pair<double, double> topLeft();
  std::pair<double, double> topRight();
  std::pair<double, double> bottomLeft();
  std::pair<double, double> bottomRight();

  void setTopLeft(double x, double y);
  void setTopRight(double x, double y);
  void setBottomLeft(double x, double y);
  void setBottomRight(double x, double y);

  std::pair<double, double> centroid();

  bool isInside(double x, double y);

  double area() const;

 private:
  void generatePolygon(double xTopLeft, double yTopLeft, double xBottomRight,
                       double yBottomRight);
  void buildPolygon();
  void calculateArea();

  typedef boost::geometry::model::point<double, 2,
                                        boost::geometry::cs::cartesian>
      point_t;
  typedef boost::geometry::model::polygon<point_t> polygon_t;

  double m_area;
  point_t m_topLeft;
  point_t m_topRight;
  point_t m_bottomLeft;
  point_t m_bottomRight;
  polygon_t m_polygon;

};

#endif  // RECTANGLE_H
