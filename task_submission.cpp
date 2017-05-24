#include "task_submission.h"

namespace FMM_3D{
    GData *mainF,*mainG;
    int g,part_count;
    double work_min;
    /*------------------------------------------------------------------------*/
    void fmm_taskified(){
        mainF = new GData(L_max,g);
        mainG = new GData(L_max,g);
        GData &F = *mainF;
        GData &G = *mainG;

        for(int gi = 0;gi<g; gi++){
            if ( gi ==0)
                fmm_engine->add_task( new FFLTask (              F(L_max,gi)));
            else
                fmm_engine->add_task( new FFLTask (F(L_max,gi-1),F(L_max,gi)));
        }
        for(int L=L_max-1;L>0;L--){
            for(int gi = 0;gi<g; gi++){
                if ( gi ==0)
                    fmm_engine->add_task( new C2PTask (          F(L,gi),F(L-1,gi)));
                else
                    fmm_engine->add_task( new C2PTask (F(L,gi-1),F(L,gi),F(L-1,gi)));
                for(int gj = 0;gj<g; gj++){
                    fmm_engine->add_task( new XLTTask(F(L,gj),G(L,gi)));
                }
            }
        }
        for(int L=1;L<L_max;L++){
            for(int gi = 0;gi<g; gi++){
                fmm_engine->add_task ( new P2CTask(G(L-1,gi),G(L,gi)));
            }
        }
        for(int gi = 0;gi<g; gi++){//todo: How can we make it to run only when no other tasks can run?
            for(int gj = 0;gj<g; gj++){
                fmm_engine->add_task ( new NearFTask(G(L_max,gi),G(L_max,gj)));
            }
        }
    }
    /*------------------------------------------------------------------------*/
    void FFLTask::run(){
        GData &A = *d1;
        GData &B = *d2;
        for(int i=0;i<part_count;i++){
            fmm_engine->add_task( new fflTask(A[i],B[i]));
        }
    }
    /*------------------------------------------------------------------------*/
    void fflTask::run(){
        BoxList * batch = new BoxList;
        double work=0.0;

        for (Box *b: d2->boxes){
            work += get_ffl_work(b);
            if ( work < work_min){
                batch->push_back(b);
            }
            else{
                fmm_engine->add_task (new ffl_task (batch) );
                work = 0.0;
                batch = new BoxList;
            }
        }
    }
    /*------------------------------------------------------------------------*/
    void C2PTask::run(){
        GData &A = *d2;
        GData &B = *d3;
        for(int i=0;i<part_count;i++){
            fmm_engine->add_task( new c2pTask(A[i],B[i]));
        }
    }
    /*------------------------------------------------------------------------*/
    void c2pTask::run(){// DuctTeip level, every parent box can be a SG task

        BoxList &parents= d2->boxes;
        BoxList *batch = new BoxList;
        double batch_work = 0;
        for(Box *parent : parents ){

            if(batch_work < work_min){
                batch->push_back(parent);
                batch_work += get_c2p_work(parent);
            }
            else{
                fmm_engine->add_task ( new c2p_task(batch));
                batch = new BoxList;
                batch_work=0.0;
            }
        }
    }
    /*------------------------------------------------------------------------*/
    void XLTTask::run(){
        GData &A = *d1;
        GData &B = *d2;
        for ( int i=0;i<part_count;i++){
            for(int j=0;j<part_count;j++){
                fmm_engine->add_task( new xltTask(A[i],B[j]));
            }
        }
    }
    /*------------------------------------------------------------------------*/
    void xltTask::run(){//DT Level , every target G can be separate SG task

        F2GList *batch = new F2GList;
        double batch_work = 0.0;
        for(Box *xx: d2->boxes){
            Box &x = *xx;
            for(uint f=0;f<x.ff_int_list.size();f++){
                Box &ff=*x.ff_int_list[f];
                if ( x.index == ff.index){
                    F2GPair *f2gp= new F2GPair(&ff,&x);
                    batch->push_back(f2gp);
                    batch_work += get_xlt_work(f2gp);
                }
            }
            if ( batch_work > work_min){
                fmm_engine->add_task ( new xlt_task (batch)) ;
                batch = new F2GList;
                batch_work = 0.0;
            }
        }
    }
    /*------------------------------------------------------------------------*/
    void P2CTask::run(){
        GData &A = *d1;
        GData &B = *d2;
        for(int i=0;i<part_count;i++){
            fmm_engine->add_task ( new p2cTask(A[i],B[i]));
        }
    }
    /*------------------------------------------------------------------------*/
    void NearFTask::run(){}//todo
    /*------------------------------------------------------------------------*/
    void p2cTask::run( ){

        BoxList &parents= d1->boxes;
        BoxList *batch = new BoxList;
        double batch_work = 0;

        for(Box* parent:parents){
            for ( Box * c: parent->children){
                if(batch_work < work_min){
                    batch->push_back(c);
                    batch_work += get_p2c_work(c);
                }
                else{
                    fmm_engine->add_task ( new p2c_task(batch));
                    batch = new BoxList;
                    batch_work=0.0;
                }
            }
        }
    }
    /*------------------------------------------------------------------------*/
    double get_c2p_work(Box *c){
        LevelBase &L=*tree->Level[c->level];
        int nnz1=L.C2P->A1->data.size();
        int nnz2=L.C2P->A2->data.size();
        int m,n;
        c->Ft->get_dims(m,n);
        return m*n*nnz1*nnz2;
    }
    /*----------------------------------------------------------------------*/
    double get_xlt_work(F2GPair *boxes){
        LevelBase &L = *tree->Level[boxes->first->level];
        int m,n,k,p;
        L.T[0]->get_dims(m,n);
        boxes->first->F->get_dims(k,p);
        return (double)(m*n*k)/3.0;
    }
    /*----------------------------------------------------------------------*/
    double get_p2c_work(Box *c){
        LevelBase &L=*tree->Level[c->level];
        int nnz1=L.P2C->A1->data.size();
        int nnz2=L.P2C->A2->data.size();
        int m,n;
        c->Ft->get_dims(m,n);
        return m*n*nnz1*nnz2;

    }
    /*----------------------------------------------------------------------*/
    double get_ffl_work(Box *b){
        int M,N,K;
        b->F->get_dims(M,N);
        b->I->get_dims(N,K);
        return (double)(M*N*K)/3.0;
    }
    /*----------------------------------------------------------------------*/
    double get_nfl_work(BoxPair* boxes){
        int M,N,K,P;
        boxes->first->Z.data[boxes->second->index]->get_dims(M,N);
        boxes->first->I->get_dims(K,P);
        return (double)(M*N*K)/3.0;
    }
}
