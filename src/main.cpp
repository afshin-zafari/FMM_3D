#include "fmm_3d.hpp"
#include <fstream>
using namespace std;
using namespace FMM_3D;
void FMM_main(){
//    tree = new Tree;
//    LevelBase *l=new LevelBase;
//    tree->Level.resize(10);
//    tree->Level[0] =l;
//    cout << tree->Level.size() << endl;
//    cout << tree->Level[0] <<" " << l<< endl;
//    tree->Level[0]->boxes.resize(5);
//    Box *b = new Box;
//    tree->Level[0]->boxes[0] = b;
//    b->index = 11;
//    cout << b->index << endl;
//    return;

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
