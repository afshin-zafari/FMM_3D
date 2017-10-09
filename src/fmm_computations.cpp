#include "fmm_computations.hpp"

namespace FMM_3D{
    /*------------------------------------------------------------------------*/
    void ffl_task::run(TaskExecutor<Options> &){
        for_each_box_in_batch(batch,print_box,"ffl_task");
        if ( --parent->child_count ==0)
            parent->finished();
    }
    /*------------------------------------------------------------------------*/
    void c2p_task::run(TaskExecutor<Options> &){
        for_each_box_in_batch(batch,print_box,"c2p_task");
        if (--parent->child_count ==0)
            parent->finished();
    }
    /*------------------------------------------------------------------------*/
    void xlt_task::run(TaskExecutor<Options> &){
        for_each_pair_in_batch(batch,print_box_pair,"xlt_task");
        if (--parent->child_count ==0)
            parent->finished();
    }
    /*------------------------------------------------------------------------*/
    void rcv_task::run(TaskExecutor<Options> &){
        for_each_pair_in_batch(batch,print_box_pair,"rcv_task");
        if ( --parent->child_count ==0)
            parent->finished();
    }
    /*------------------------------------------------------------------------*/
    void nfl_task::run(TaskExecutor<Options> &){
        for_each_pair_in_batch(batch,print_box_pair,"nfl_task");
        if (--parent->child_count  ==0 )
            parent->finished();
    }
    /*------------------------------------------------------------------------*/
    void p2c_task::run(TaskExecutor<Options> &){
        for_each_box_in_batch(batch,print_box,"p2c_task");
        if (--parent->child_count ==0)
            parent->finished();
    }
}
