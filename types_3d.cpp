#include "fmm_3d.hpp"
namespace FMM_3D{
    Tree *tree;
    FMMContext *fmm_engine;
    /*---------------------------------------------------------------------------------------*/
    Box &get_box(int index, int level){return *tree->Level[level-1]->boxes[index];}
    /*---------------------------------------------------------------------------------------*/
}
