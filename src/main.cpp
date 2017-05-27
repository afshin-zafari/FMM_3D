#include "fmm_3d.hpp"
#include <fstream>
using namespace std;
using namespace FMM_3D;
void FMM_main(){

    freopen("prg_out.txt","w",stdout);

    init();
    fmm_taskified();
    finalize();
    return;
    compute_near_field();
    compute_far_field();
    compute_interpolation();
    compute_green();
    compute_green_interpolation();
    compute_receiving();
    finalize();
}
using namespace std;
int main()
{
    FMM_main();
}
