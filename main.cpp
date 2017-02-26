#include "fmm_3d.hpp"

using namespace FMM_3D;
void FMM_main(){
    init();
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
