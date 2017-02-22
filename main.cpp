#include <iostream>
#include "fmm_3d.hpp"
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
int main()
{
    FMM_main();
}
