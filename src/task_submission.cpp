#include "task_submission.h"

namespace FMM_3D{
    GData *mainF,*mainG,*mainV;
    /*------------------------------------------------------------------------*/
    void fmm_taskified(){
        int np   = Parameters.part_count;
        int g    = Parameters.group_count;
        mainF    = new GData(L_max,g,np);
        mainG    = new GData(L_max,g,np);
        mainV    = new GData(L_max,g,np);
        GData &F = *mainF;
        GData &G = *mainG;
        GData &V = *mainV;

        for(int gi = 0;gi<g; gi++){
            if ( gi ==0)
                fmm_engine->add_task( new FFLTask (              F(L_max-1,gi)));
            else
                fmm_engine->add_task( new FFLTask (F(L_max,gi-1),F(L_max-1,gi)));
        }
        for(int L=L_max-2;L>0;L--){
            for(int gi = 0;gi<g; gi++){
                if ( gi ==0)
                    fmm_engine->add_task( new C2PTask (          F(L,gi),F(L-1,(L==1)?0:gi)));
                else
                    fmm_engine->add_task( new C2PTask (F(L,gi-1),F(L,gi),F(L-1,(L==1)?0:gi)));
                for(int gj = 0;gj<g; gj++){
                    fmm_engine->add_task( new XLTTask (F(L,gi),G(L,gj)));
                }
            }
        }
        for(int L=1;L<L_max;L++){
            for(int gi = 0;gi<g; gi++){
                fmm_engine->add_task ( new P2CTask(G(L-1,gi),G(L,gi)));
            }
        }
        for(int gi = 0;gi<g; gi++){
            if ( gi ==0)
                fmm_engine->add_task( new RCVTask (                G(L_max-1,gi),V(L_max-1,gi)));
            else
                fmm_engine->add_task( new RCVTask (G(L_max-1,gi-1),G(L_max-1,gi),V(L_max-1,gi)));
            fmm_engine->add_task( new NFLTask (F(L_max/2,gi),gi));
        }

    }
    /*------------------------------------------------------------------------*/
    void FFLTask::run(){
        GData &B = *d2;
        for(int i=0;i<B.part_count() ;i++){
            fmm_engine->add_task( new fflTask(B[i]));
        }
    }
    /*------------------------------------------------------------------------*/
    void fflTask::run(){
        BoxList * batch = new BoxList;
        double work=0.0;

        for (Box *b: d->boxes){
            work += get_ffl_work(b);
            if ( work < Parameters.work_min){
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
    void ffl_task::run(){//todo
    }
    /*------------------------------------------------------------------------*/
    void C2PTask::run(){
        GData &A = *d2;
        GData &B = *d3;
        int n = A.part_count() ;
        for(int i=0;i<n;i++){
            int j=i;
            if ( B.get_row() ==0 ) { // if top most level
                j = 0;
            }
            fmm_engine->add_task( new c2pTask(A[i],B[j]));
        }
    }
    /*------------------------------------------------------------------------*/
    void c2pTask::run(){// DuctTeip level, every parent box can be a SG task

        BoxList &parents= d2->boxes;
        BoxList *batch = new BoxList;
        double batch_work = 0;
        for(Box *parent : parents ){

            if(batch_work < Parameters.work_min){
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
    void c2p_task::run(){//todo
    }
    /*------------------------------------------------------------------------*/
    void XLTTask::run(){
        GData &A = *d1;
        GData &B = *d2;
        for ( int i=0;i<A.part_count();i++){
            for(int j=0;j<B.part_count();j++){
                int n1 = A[i].boxes.size();
                int n2 = B[j].boxes.size();
                if (n1 != 0 and n2 != 0 ){
                    fmm_engine->add_task( new xltTask(A[i],B[j]));
                }
            }
        }
    }
    /*------------------------------------------------------------------------*/
    void xltTask::run(){//DT Level , every target G can be separate SG task

        F2GList *batch = new F2GList;
        double batch_work = 0.0;
        for(Box *b1: d1->boxes){
            for(Box *b2: d2->boxes){
                if ( is_box_in_list(b2->ff_int_list,b1)){
                    F2GPair *f2gp= new F2GPair(b1,b2);
                    batch->push_back(f2gp);
                    batch_work += get_xlt_work(f2gp);
                }
                if ( batch_work > Parameters.work_min){
                    fmm_engine->add_task ( new xlt_task (batch)) ;
                    int len = batch->size() ;
                    cout << "Batch  len " << len << endl;
                    batch = new F2GList;
                    batch_work = 0.0;
                }
            }
        }
        if ( batch_work > 0.0){
            fmm_engine->add_task ( new xlt_task (batch)) ;
            int len = batch->size() ;
            cout << "Batch  len " << len << endl;
        }
    }
    /*------------------------------------------------------------------------*/
    void xlt_task::run(){//todo
    }
    /*------------------------------------------------------------------------*/
    void P2CTask::run(){
        GData &A = *d1;
        GData &B = *d2;
        for(int i=0;i<A.part_count();i++){
            fmm_engine->add_task ( new p2cTask(A[i],B[i]));
        }
    }
    /*------------------------------------------------------------------------*/
    void RCVTask::run(){
        GData &A = *d2;
        GData &B = *d3;
        for(int i=0;i<A.part_count();i++){
            fmm_engine->add_task ( new rcvTask(A[i],B[i]));
        }
    }
    /*------------------------------------------------------------------------*/
    void rcvTask::run(){//todo
        fmm_engine->add_task(new rcv_task(d1,d2));
    }
    /*------------------------------------------------------------------------*/
    void rcv_task::run(){//todo
    }
    /*------------------------------------------------------------------------*/
    void NFLTask::run(){
        for (int i=0;i<Parameters.part_count ;i++){
            fmm_engine->add_task(new nflTask(group,i));
        }
    }
    /*------------------------------------------------------------------------*/
    void nflTask::run(){
        /* for b1 in F(L_max,group)[part].boxes
            for b2 in b1.near_field
                add task VZI (b1,b2)
        */
        GData &F = *mainF;
        BoxList &boxes =  F(L_max-1,group)[part].boxes;
        for ( Box *b1: boxes){
            for (Box *b2 : b1->nf_int_list ){
                fmm_engine->add_task( new nfl_task(b1->index,b2->index));
            }
        }
    }
    /*------------------------------------------------------------------------*/
    void nfl_task::run(){//todo
    }
    /*------------------------------------------------------------------------*/
    void p2cTask::run( ){

        BoxList &parents= d1->boxes;
        BoxList *batch = new BoxList;
        double batch_work = 0;

        for(Box* parent:parents){
            for ( Box * c: parent->children){
                if(batch_work < Parameters.work_min){
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
        if ( batch_work > 0.0)
            fmm_engine->add_task ( new p2c_task(batch));
    }
    /*------------------------------------------------------------------------*/
    void p2c_task::run(){//todo
    }
    /*------------------------------------------------------------------------*/
    double get_c2p_work(Box *c){
        LevelBase &L=*tree->Level[c->level-1];
        int nnz1=L.C2P->A1->data.size();
        int nnz2=L.C2P->A2->data.size();
        int m,n;
        c->Ft->get_dims(m,n);
        return m*n*nnz1*nnz2;
    }
    /*----------------------------------------------------------------------*/
    double get_xlt_work(F2GPair *boxes){
        int l = boxes->first->level-1;
        LevelBase &L = *tree->Level[l];
        int m,n,k,p;
        int z = L.T.size();
        if ( z )
            L.T[0]->get_dims(m,n);
        boxes->first->Ft->get_dims(k,p);
        return (double)(m*n*k)/3.0;
    }
    /*----------------------------------------------------------------------*/
    double get_p2c_work(Box *c){
        LevelBase &L=*tree->Level[c->level-1];
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
    /*----------------------------------------------------------------------*/
    void NFL_tasks ( GData & dep,int chunk_no, int chunks_count){
    }
    /*----------------------------------------------------------------------*/
    void TopLayerData::make_groups(){
        int bpg,nbl;
        TopLayerData &Root=(*this)(0,0);
        LevelBase &TopLevel=*tree->Level[0];
        Root.boxes.push_back(TopLevel.boxes[0]);
        TopLevel.boxes[0]->group = 0;
        int L = 1;
        for ( uint32_t gi=0;gi<ng;gi++){
            nbl = Root.boxes[0]->children.size();
            bpg = nbl/ng+1;
            for (int bi=0;bi<nbl;bi++){
                Box *c=Root.boxes[0]->children[bi];
                GData &cd=(*this)(L,bi/bpg);
                cd.boxes.push_back(c);
                c->group = bi/bpg;
            }
        }
        for(L=2;L<L_max;L++){
            for ( Box *pb: tree->Level[L-1]->boxes){
                for( Box *cb: pb->children){
                    int pg=cb->parent->group;
                    GData &node=(*this)(L,pg);
                    node.boxes.push_back(cb);
                    cb->group = pg;
                }
            }
        }
    }
/*---------------------------------------------------*/
    void TopLayerData::make_partitions(){
        // assumptions : this is at the top layer
        TopLayerData &Root=(*this)(0,0);
        LevelBase &Level=*tree->Level[0];
        //create a single part for the root
        Root.parts.push_back(new GData(0,0,0,true));
        Root.parts[0]->boxes.push_back(Level.boxes[0]);
        Level.boxes[0]->part  = 0;
        int L = 1;
        for (uint32_t gi=0;gi<ng;gi++){
            GData &node = (*this)(L,gi);
            for ( uint32_t ci = 0; ci<np;ci++)
                node.parts.push_back(new GData (L,gi,ci,true) );
            int nb=node.boxes.size();
            int bpp = nb/np+1;
            for(int bi=0;bi<nb;bi++){
                Box *b=node.boxes[bi];
                node.parts[bi/bpp]->boxes.push_back(b);
                b->part = bi/bpp;
            }
        }
        for (L=2;L<L_max;L++){
            for (uint32_t gi=0;gi<ng;gi++){
                GData &cd = (*this)(L  ,gi);
                for ( uint32_t ci = 0; ci<np;ci++)
                    cd.parts.push_back(new GData (L,gi,ci,true) );
                for(Box *cb: cd.boxes){
                    int pp=cb->parent->part;
                    cd.parts[pp]->boxes.push_back(cb);
                    cb->part = pp;
                }
            }
        }
    }
    /*---------------------------------------------------*/
    bool is_box_in_list(BoxList &boxes, Box * b){
        for ( Box * lb : boxes){
            if ( b->index == lb-> index )
                return true;
        }
        return false;
    }
    /*---------------------------------------------------*/


}
