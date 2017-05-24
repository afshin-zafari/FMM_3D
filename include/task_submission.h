#ifndef TASK_SUBMISSION_HPP
#define TASK_SUBMISSION_HPP
#include "types_3d.hpp"
#include "types_dist.hpp"
namespace FMM_3D{
    extern double work_min;
    /*======================================================================*/
    class TopLayerData: public DTBase{
    private:
        int M,N,y,x;
        vector<TopLayerData*> groups,parts;
    /*---------------------------------------------------*/
        void make_groups(){//todo
            // for the root , create one group with one box
            /* for L in 1:Levels
                 for b in tree->Level(Level).boxes
                   for g in groups (L)
                    if nb< bpg && parent in D(L-1,g).boxes) or last g
                       g.boxes.push_back(b)
                       nb++
                    else
                       nb=0
                       next g
            */
            int ng  = N;
            int nb,bpg;
            TopLayerData &D=(*this)(0,0);
            LevelBase &Level=*tree->Level[0];
            D.boxes.push_back(Level.boxes[0]);
            for( int L =1;L <L_max; L++){
                Level=*tree->Level[L];
                nb = Level.boxes.size();
                bpg = nb/ ng;
                if( bpg !=0){
                    for(int bi=0;bi<nb;bi++){
                        D=(*this)(L,(bi/bpg)%ng);
                        D.boxes.push_back(Level.boxes[bi]);
                    }
                }
                else{
                    //error
                }
            }
        }
        void make_partitions(){//todo
            // assumptions : this is at the top layer
            // L = 0 , the root node
            // TopLayerData &D=(*this)(0,0);
            //LevelBase &Level=*tree->Level[0];
            // create a single part for the root
            //D.parts[0].boxes.push_back(Level.boxes[0]);

            /* for L in 1:Levels
                for g in groups(L)
                  create g.parts ( parts_count)
                  bpp = g.boxes.size() / np;
                  while i in parts_count:
                    for b in g.boxes
                      if nb < bpp && parent in D(L-1,g).parts[i].boxes
                        g.parts[i].boxes.push_back(b);
                        nb++;
                      else
                        i++
                        nb =0;

            */
        }
    /*---------------------------------------------------*/
    /*---------------------------------------------------*/
    public:
        BoxList boxes;
        TopLayerData(int row, int col):y(row),x(col){}
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
        void show_hierarchy(){//todo
            /* b.info is: level,index,children
            for L in Levels
                for g in groups(L)
                     for b in g.boxes
                        show b.info
                        for p in g
                            for bb in p.boxes
                                show bb.info
            */
        }

    };
    typedef TopLayerData GData;
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
        virtual void run();
        virtual void export_it(fstream &f){}
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
        virtual void run();
        virtual void export_it(fstream &f){}
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
        void run(){}
        void export_it(fstream &){}
    };
    /*======================================================================*/
    class NFLTask: public DTTask {
    public:
        GData *d0,*d1,*d2;
        NFLTask(GData & d0_,GData &d1_,GData &d2_){
            d0 = &d0_;d1 = &d1_;d2 = &d2_;
            key= DT_NFL;
        }
        void run();
        void export_it(fstream &){}
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
    };
    /*======================================================================*/
    class p2c_task : public SGTask{
        BoxList *batch;
    public:
        p2c_task (BoxList * ){}
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
