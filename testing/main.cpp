#include <string>
#include "adversion.h"

int main(int argc, char *argv[]) {

  std::string filename =
      "/Users/zcobell/Documents/Code/ADCIRCModules/build/"
      "cpra2017_v15a-CurrentConditions-openIHNC_chk.grd";
//  filename =
//      "/Users/zcobell/Documents/Code/ADCIRCModules/build/"
//      "sl18_2007storm_v1_20190506_chk.grd";
  std::string rootDirectory = "/Users/zcobell/Documents/Code/AdVersion2/build/cpra";
  AdVersion adv(filename,rootDirectory);
  adv.partitionMesh(256);

  return 0;
}
