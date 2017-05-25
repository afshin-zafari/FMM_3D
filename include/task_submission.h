#ifndef TASK_SUBMISSION_HPP
#define TASK_SUBMISSION_HPP
#include "types_3d.hpp"
#include "types_dist.hpp"
namespace FMM_3D{
    extern double work_min;
    /*======================================================================*/
    class TopLayerData: public DTBase{
    private:
        int M,N,y,x,part_no;
        vector<TopLayerData*> groups,parts;
        typedef TopLayerData GData;
    /*---------------------------------------------------*/
        void make_groups(){
            uint32_t ng  = N;
            int nb=0,bpg,nbl;
            TopLayerData &D=(*this)(0,0);
            LevelBase &Level=*tree->Level[0];
            D.boxes.push_back(Level.boxes[0]);

            for(int L=1;L<=L_max;L++){
                GData &parent = (*this)(L-1,x);
                nbl =tree->Level[L]->boxes.size();
                bpg = nbl/ng+1;
                nb = 0;
                uint32_t gi=0;
                for(Box *b : parent.boxes ){
                    for(Box *c: b->children){
                        GData &child  = (*this)(L,gi);
                        child.boxes.push_back(c);
                        nb++;
                        if ( nb > bpg and L == 1){
                            nb=0;
                            if (gi< ng-1)
                                gi++;
                        }
                    }
                    if ( nb > bpg ){
                        nb = 0;
                        if ( gi< ng-1 )
                            gi++;
                    }
                }
            }
        }
        void make_partitions(){
            // assumptions : this is at the top layer
            TopLayerData &D=(*this)(0,0);
            LevelBase &Level=*tree->Level[0];
            //create a single part for the root
            D.parts.push_back(new GData(0,0,0,true));
            D.parts[0]->boxes.push_back(Level.boxes[0]);
            int np=Parameters.part_count,nb=0;
            for (int L=1;L<L_max;L++){
                int ng = Parameters.group_count;
                for (int gi=0;gi<ng;gi++){
                    GData &parent = (*this)(L-1,gi);
                    int bpp = parent.boxes.size() / np;
                    for (int pi=0;pi<np;pi++ ){
                        GData &dc = (*this)(L,gi);
                        dc.parts.push_back(new GData (L,gi,pi,true) );
                        BoxList &pbx=parent.parts[pi]->boxes;
                        for(Box * pb:pbx){
                            for(Box *cb: pb->children){
                                dc.parts[pi]->boxes.push_back(cb);
                                nb++;
                                if (nb > bpp and L == 1){
                                    nb = 0;
                                    if ( pi < np-1)
                                        pi++;
                                }
                            }
                            if (nb > bpp){
                                nb = 0;
                                if ( pi < np-1)
                                    pi++;
                            }
                        }

                    }


                }
            }

        }
    /*---------------------------------------------------*/
    /*---------------------------------------------------*/
    public:
        BoxList boxes;
        TopLayerData(int row, int col):y(row),x(col){}
        TopLayerData(int row, int col, int part, bool):y(row),x(col),part_no(part){}
        int get_row(){return y;}
        int get_col(){return x;}
    /*---------------------------------------------------*/
        TopLayerData(int M_, int N_,int ng):M(M_),N(N_){
            for(int j=0;j<N;j++){
                for(int i=0;i<M;i++){
                    groups.push_back(new TopLayerData(i,j));
                }
            }
            make_groups();
            make_partitions();
        }
    /*---------------------------------------------------*/
        TopLayerData &operator()(int i,int j){
            return *groups[j*M+i];
        }
    /*---------------------------------------------------*/
        bool contains(Box *b){
            for( Box *my_b: boxes){
                if (my_b->index == b->index)
                    return true;
            }
            return false;
        }
    /*---------------------------------------------------*/
        TopLayerData &operator[](int i){
            return *parts[i];
        }
    /*---------------------------------------------------*/
        void show_hierarchy(){
            /* b.info is: level,index,children
            for L in Levels
                for g in groups(L)
                     for b in g.boxes
                        show b.info
                        for p in g
                            for bb in p.boxes
                                show bb.info
            */
            for (int L=0;L<=L_max;L++ ){
                for (int gi=0;gi<Parameters.groups;gi++){
                    TopLayerData &D=(*this)(L,gi);
                    for(Box *b:D.boxes){
                        D.show_box(b);
                    }
                    for (uint32_t pi=0;pi<D.parts.size();pi++ ){
                        for( Box *bp:D[pi].boxes){
                            D[pi].show_box(bp);
                        }
                    }
                }
            }
        }
    /*---------------------------------------------------*/
        void show_box(Box *b){
            cout << b->level << "," << b->index << endl;
        }
    /*---------------------------------------------------*/

    };
    typedef TopLayerData GData;
    /*---------------------------------------------------*/
    extern FMM_3D::GData *mainF,*mainG;
    extern int g,part_count;
    /*======================================================================*/
    class FFLTask: public DTTask{
    public:
        GData *d1,*d2;
    /*----------------------------------------------------------------------*/
        FFLTask(GData &d1_,GData &d2_){
            d1 = &d1_;d2 = &d2_;
            key= DT_FFL;
        }
        FFLTask(GData &d1_){
            d2 = &d1_;d1 = nullptr;
            key= DT_FFL;
        }
        void run();
        void export_it(fstream &f){}
    };
    /*======================================================================*/
    class C2PTask: public DTTask{
    public:
        GData *d1,*d2,*d3;
    /*----------------------------------------------------------------------*/
        C2PTask(GData &d1_,GData &d2_, GData &d3_){
            d1 = &d1_;d2 = &d2_;d3 = &d3_;
            key= DT_C2P;
        }
        C2PTask(GData &d1_,GData &d2_){
            d2 = &d1_;d3 = &d2_;d1 = nullptr;
            key= DT_C2P;
        }
        void run();
        void export_it(fstream &f){}
    };
    /*======================================================================*/
    class XLTTask: public DTTask {
    public:
        GData *d1,*d2;
        XLTTask(GData &d1_,GData &d2_){
            d1 = &d1_;d2 = &d2_;
            key= DT_XLT;
        }
        void run();
        void export_it(fstream &){}
    };
    /*======================================================================*/
    class P2CTask: public DTTask {
    public:
        GData *d1,*d2;
        P2CTask(GData &d1_,GData &d2_){
            d1 = &d1_;d2 = &d2_;
            key= DT_P2C;
        }
        void run();
        void export_it(fstream &){}
    };
    /*======================================================================*/
    class RCVTask: public DTTask {
    public:
        GData *d1,*d2,*d3;
        RCVTask(GData &d1_,GData &d2_){
            d3 = &d2_;
            d2 = &d1_;
            d1 = nullptr;
            key= DT_RCV;
        }

        RCVTask(GData &d1_,GData &d2_,GData &d3_){
            d1 = &d1_;d2 = &d2_;d3 = &d3_;
            key= DT_RCV;
        }
        void run();
        void export_it(fstream &){}
    };
    /*======================================================================*/
    class rcvTask: public DTTask {
    public:
        GData *d1,*d2,*d3;
        rcvTask(GData &d1_,GData &d2_){
            d3 = &d2_;
            d2 = &d1_;
            d1 = nullptr;
            key= DT_RCV;
        }

        rcvTask(GData &d1_,GData &d2_,GData &d3_){
            d1 = &d1_;d2 = &d2_;d3 = &d3_;
            key= DT_RCV;
        }
        void run();
        void export_it(fstream &){}
    };

    /*======================================================================*/
    class rcv_task : public SGTask{
    public:
        rcv_task(GData *,GData *){}
        void run();
    };
    /*======================================================================*/
    class NFLTask: public DTTask {
    public:
        GData *d;
        int group;
        NFLTask(GData & d_,int g ):group(g){
            d = &d_;
            key= DT_NFL;
        }
        void run();
        void export_it(fstream &){}
    };
    /*======================================================================*/
    class nflTask : public DTTask {
    public:
        int group,part;
        nflTask(int g,int i):group(g),part(i){}
        void run();
        void export_it(fstream &){}
    };
    /*======================================================================*/
    class nfl_task : public SGTask {
    public:
        int bi,bj;
        nfl_task(int i,int j):bi(i),bj(j){}
        void run();
    };
    /*======================================================================*/
    class fflTask: public DTTask{
    public:
        GData *d1,*d2;
        fflTask(GData &, GData &){}
        void run();
        void export_it ( fstream &){}
    };
    /*======================================================================*/
    class c2pTask: public DTTask{
    public:
        GData *d1,*d2;
        c2pTask(GData &, GData &){}
        void run();
        void export_it ( fstream &){}
    };
    /*======================================================================*/
    typedef std::pair<Box *,Box*> F2GPair;
    typedef std::pair<Box *,Box*> BoxPair;
    typedef std::vector<F2GPair*>  F2GList;
    class xltTask: public DTTask{
    public:
    GData *d1,*d2,*d3;
        xltTask(GData &, GData &){}
        void run();
        void export_it ( fstream &){}
    };
    /*======================================================================*/
    class p2cTask: public DTTask{
    public:
        GData *d1,*d2,*d3;
        p2cTask(GData &, GData &){}
        void run();
        void export_it ( fstream &){}
    };
    /*======================================================================*/
    class c2p_task : public SGTask{
    public :
        c2p_task ( BoxList * ){}
        void run();
    };
    /*======================================================================*/
    class ffl_task : public SGTask{
        BoxList *batch;
    public:
        ffl_task  (BoxList *){}
        ~ffl_task  (){
            if ( !batch)
                return;
            batch->clear();
        }
        void run();
    };
    /*======================================================================*/
    class xlt_task : public SGTask{
        F2GList* batch;
    public:
        xlt_task (F2GList*b):batch(b){}
        ~xlt_task (){
            if ( !batch)
                return;
            for ( F2GPair* p: *batch)
                delete p;
            batch->clear();
        }
        void run();
    };
    /*======================================================================*/
    class p2c_task : public SGTask{
        BoxList *batch;
    public:
        p2c_task (BoxList * ){}
        void run();
    };
    /*======================================================================*/
    double get_c2p_work(Box *);
    double get_xlt_work(F2GPair *);
    double get_p2c_work(Box *);
    double get_ffl_work(Box *);
    void fmm_taskified();
    void NFL_tasks ( GData & dep,int chunk_no, int chunks_count);
}
#endif // TASK_SUBMISSION_HPP
