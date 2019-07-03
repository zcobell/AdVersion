#include <string>
#include "adversion.h"

int main(int argc, char *argv[])
{
    std::string filename = "/Users/zcobell/Documents/Code/ADCIRCModules/build/cpra2017_v15a-CurrentConditions-openIHNC_chk.grd";

    AdVersion adv(filename);
    adv.partitionMesh(12);

    return 0;
}
