#include "fmm_3d.hpp"
namespace FMM_3D{
    Tree tree;
    FMMContext *fmm_engine;
    Box::Box(int i, int level_){
        Box b = *tree.Level[level_]->boxes[i];
        index = i;
        level = level_;
    }

}
