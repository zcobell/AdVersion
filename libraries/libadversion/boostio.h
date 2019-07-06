#ifndef BOOSTIO_H
#define BOOSTIO_H

#include <string>
#include "rectangle.h"

class BoostIO {
 public:
  BoostIO();
  static Rectangle readRectangle(const std::string &line);
  static bool readNodeLine(const std::string &line, std::string &hash,
                           double &x, double &y, double &z);
  static bool readElementLine(const std::string &line, std::string &hash,
                              std::string &n1, std::string &n2,
                              std::string &n3);
};

#endif  // BOOSTIO_H
