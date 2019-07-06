#include "boostio.h"
#include "boost/algorithm/string/split.hpp"
#include "boost/algorithm/string/trim.hpp"
#include "boost/config/warning_disable.hpp"
#include "boost/spirit/include/phoenix.hpp"
#include "boost/spirit/include/qi.hpp"

namespace qi = boost::spirit::qi;
namespace ascii = boost::spirit::ascii;
namespace phoenix = boost::phoenix;

BoostIO::BoostIO() {}

Rectangle BoostIO::readRectangle(const std::string &line) {
  size_t p;
  double x1, y1, x2, y2;
  bool r = qi::phrase_parse(line.begin(), line.end(),
                            (qi::int_[phoenix::ref(p) = qi::_1] >>
                             qi::double_[phoenix::ref(x1) = qi::_1] >>
                             qi::double_[phoenix::ref(y1) = qi::_1] >>
                             qi::double_[phoenix::ref(x2) = qi::_1] >>
                             qi::double_[phoenix::ref(y2) = qi::_1]),
                            ascii::space);
  return Rectangle(x1, y1, x2, y2);
}

bool BoostIO::readNodeLine(const std::string &line, std::string &hash,
                           double &x, double &y, double &z) {
  return qi::parse(line.begin(), line.end(),
                   (+~qi::char_(' ') >> ' ' >> qi::double_ >> ' ' >>
                    qi::double_ >> ' ' >> qi::double_),
                   hash, x, y, z);
}

bool BoostIO::readElementLine(const std::string &line, std::string &hash,
                              std::string &n1, std::string &n2,
                              std::string &n3) {
  return qi::parse(line.begin(), line.end(),
                   (+~qi::char_(' ') >> ' ' >> +~qi::char_(' ') >> ' ' >>
                    +~qi::char_(' ') >> ' ' >> +~qi::char_(' ')),
                   hash, n1, n2, n3);
}

bool BoostIO::readBoundaryHeader(const std::string &line, int &code,
                                 size_t &size) {
  return qi::phrase_parse(line.begin(), line.end(),
                          (qi::ulong_[phoenix::ref(size) = qi::_1] >>
                           qi::int_[phoenix::ref(code) = qi::_1]),
                          ascii::space);
}

bool BoostIO::readBoundarySingleNode(const std::string &line, std::string &n1) {
  n1 = line;
  return true;
}

bool BoostIO::readBoundaryExternalWeirBoundary(const std::string &line,
                                               std::string &n1, double &crest,
                                               double &supercritical) {
  return qi::parse(
      line.begin(), line.end(),
      (+~qi::char_(' ') >> ' ' >> qi::double_ >> ' ' >> qi::double_), n1, crest,
      supercritical);
}

bool BoostIO::readBoundaryInternalWeirBoundary(const std::string &line,
                                               std::string &n1, std::string &n2,
                                               double &crest,
                                               double &supercritical,
                                               double &subcritical) {
  return qi::parse(line.begin(), line.end(),
                   (+~qi::char_(' ') >> ' ' >> +~qi::char_(' ') >> " " >>
                    qi::double_ >> " " >> qi::double_ >> " " >> qi::double_),
                   n1, n2, crest, subcritical, supercritical);
}

bool BoostIO::readBoundaryInternalWeirWithPipes(
    const std::string &line, std::string &n1, std::string &n2, double &crest,
    double &supercritical, double &subcritical, double &pipeDiam,
    double &pipeCoef, double &pipeHeight) {
  return qi::parse(
      line.begin(), line.end(),
      (+~qi::char_(' ') >> ' ' >> +~qi::char_(' ') >> ' ' >> qi::double_ >>
       ' ' >> qi::double_ >> ' ' >> qi::double_ >> ' ' >> qi::double_ >> ' ' >>
       qi::double_ >> ' ' >> qi::double_),
      n1, n2, crest, subcritical, supercritical, pipeHeight, pipeCoef,
      pipeDiam);
}
