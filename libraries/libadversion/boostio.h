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
  static bool readBoundaryHeader(const std::string &line, int &code,
                                 size_t &size);
  static bool readBoundarySingleNode(const std::string &line, std::string &n1);
  static bool readBoundaryExternalWeirBoundary(const std::string &line,
                                               std::string &n1, double &crest,
                                               double &supercritical);
  static bool readBoundaryInternalWeirBoundary(const std::string &line,
                                               std::string &n1, std::string &n2,
                                               double &crest,
                                               double &supercritical,
                                               double &subcritical);
  static bool readBoundaryInternalWeirWithPipes(
      const std::string &line, std::string &n1, std::string &n2, double &crest,
      double &supercritical, double &subcritical, double &pipeDiam,
      double &pipeCoef, double &pipeHeight);
};

#endif  // BOOSTIO_H
