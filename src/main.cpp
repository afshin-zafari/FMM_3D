#include "fmm_3d.hpp"
#include <fstream>
using namespace std;
using namespace FMM_3D;
/*-------------------------------------------------*/
void fmm_main(int argc, char *argv[]){
    init(argc, argv);
    //fmm_taskified();
    finalize();
    return;
}
/*-------------------------------------------------*/
int main(int argc, char *argv[])
{
    fmm_main(argc, argv);
}
