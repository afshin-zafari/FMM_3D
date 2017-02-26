#ifndef TYPES_DIST_HPP
#define TYPES_DIST_HPP
namespace FMM_3D{
    typedef long DTHandle;
    /*--------------------------------------------------------------------*/
    class DTBase{
    public:
        int M,N,lead_dim,host,type,g_index;
        double *mem;
        DTHandle *handle;
    };
    /*--------------------------------------------------------------------*/
    class DTTask{
    public:
        int key;
    };
    /*--------------------------------------------------------------------*/
    class FMMContext{
    public:
        enum TASKS{ MVP,Interpolation, Green_Translate, Green_Interpolate, Receiving};

        void add_mvp_task              (DTBase *M, DTBase *v, DTBase *x             ){}
        void add_interpolation_task    (DTBase *E, DTBase *I, DTBase *f1, DTBase *f2){}
        void add_green_translate_task  (DTBase *T, DTBase *F, DTBase *G             ){}
        void add_green_interpolate_task(DTBase *P, DTBase *E, DTBase *G1, DTBase *G2){}
        void add_receiving_task        (DTBase *R, DTBase *G, DTBase *V             ){}

        void runKernels(DTTask *t){
            switch(t->key){
            case MVP:
                break;
            case Interpolation:
                break;
            case Green_Translate:
                break;
            case Green_Interpolate:
                break;
            case Receiving:
                break;
            default:
                break;
            }
        }
    };
    /*--------------------------------------------------------------------*/
    extern FMMContext *fmm_engine;
    /*--------------------------------------------------------------------*/

}
#endif // TYPES_DIST_HPP
