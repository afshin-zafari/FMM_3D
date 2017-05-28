#ifndef TASK_SUBMISSION_HPP
#define TASK_SUBMISSION_HPP
#include "types_3d.hpp"
#include "types_dist.hpp"
namespace FMM_3D{
    extern double work_min;
    typedef std::pair<Box *,Box*> F2GPair;
    typedef std::pair<Box *,Box*> BoxPair;
    typedef std::vector<F2GPair*>  F2GList;
    typedef std::vector<BoxPair*>  X2YList;
    typedef void (*BoxPairFcn)(string , BoxPair *);
    typedef void (*BoxFcn)(string , Box *);
    /*======================================================================*/
    class IterTask: public DTTask{
    public:
        int iter;
        static double norm_V_diff;
        static int last_iter_no;
        IterTask(){
            iter = last_iter_no++;
        }
        void finished(){
            if ( norm_V_diff > 1e-6)
                fmm_engine->add_task ( new IterTask () );
        }
        void run();
        void export_it(fstream &){}
    };
    /*======================================================================*/
    class TopLayerData: public DTBase{
    private:
        int M,N,y,x;
        uint32_t part_no,ng,np;
        vector<TopLayerData*> groups,parts;
        typedef TopLayerData GData;
    /*---------------------------------------------------*/
    public:
        BoxList boxes;
        TopLayerData(int row, int col):y(row),x(col){}
        TopLayerData(int row, int col, int part, bool):y(row),x(col),part_no(part){}
        int get_row(){return y;}
        int get_col(){return x;}
        int part_count(){return parts.size();}
        void make_groups();
        void make_partitions();
    /*---------------------------------------------------*/
    // End user program interface for defining GData
        TopLayerData(int M_, int N_,int np_):M(M_),N(N_),ng(N_),np(np_){
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
            int n = parts.size();
            (void)n;
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
            for (int L=0;L<L_max;L++ ){
                for (int gi=0;gi<Parameters.groups;gi++){
                    TopLayerData &D=(*this)(L,gi);
                    cout << "Boxes in D("<< L << "," << gi<<")\n" ;
                    for(Box *b:D.boxes){
                        D.show_box(b);
                    }
                    for (uint32_t pi=0;pi<D.parts.size();pi++ ){
                        cout << "Boxes in D("<< L << "," << gi<<"," << pi << ")\n" ;
                        for( Box *bp:D[pi].boxes){
                            D[pi].show_box(bp);
                        }
                    }
                }
            }
        }
    /*---------------------------------------------------*/
        void show_box(Box *b){
            cout << "B("<< b->level << "," << b->index << ")" << endl;
        }
    /*---------------------------------------------------*/

    };
    typedef TopLayerData GData;
    /*---------------------------------------------------*/
    extern FMM_3D::GData *mainF,*mainG;
    /*======================================================================*/
    class FFLTask: public DTTask{
    public:
        GData *d1,*d2;
    /*----------------------------------------------------------------------*/
        FFLTask(GData &d1_,GData &d2_,DTTask *p){
            d1 = &d1_;d2 = &d2_;
            key= DT_FFL;
            parent = p;
            parent->child_count ++;
        }
        FFLTask(GData &d1_,DTTask *p){
            d2 = &d1_;d1 = nullptr;
            key= DT_FFL;
            parent = p;
            parent ->child_count ++;
        }

        void finished(){
            if (--parent->child_count ==0)
                parent->finished();
        }
        void run();
        void export_it(fstream &f){}
    };
    /*======================================================================*/
    class C2PTask: public DTTask{
    public:
        GData *d1,*d2,*d3;
    /*----------------------------------------------------------------------*/
        C2PTask(GData &d1_,GData &d2_, GData &d3_,DTTask *p){
            d1 = &d1_;d2 = &d2_;d3 = &d3_;
            key= DT_C2P;
            parent = p;
            parent->child_count ++;
        }
        C2PTask(GData &d1_,GData &d2_,DTTask *p){
            d2 = &d1_;d3 = &d2_;d1 = nullptr;
            key= DT_C2P;
            parent = p;
            parent->child_count ++;
        }
        void run();
        void export_it(fstream &f){}
        void finished(){
        if(--parent->child_count==0)
            parent->finished();
        }
    };
    /*======================================================================*/
    class XLTTask: public DTTask {
    public:
        GData *d1,*d2;
        XLTTask(GData &d1_,GData &d2_,DTTask *p){
            d1 = &d1_;d2 = &d2_;
            key= DT_XLT;
            parent = p;
            parent->child_count ++;
        }
        void run();
        void export_it(fstream &){}
        void finished(){
            if(--parent->child_count ==0)
                parent->finished();
        }
    };
    /*======================================================================*/
    class P2CTask: public DTTask {
    public:
        GData *d1,*d2;
        P2CTask(GData &d1_,GData &d2_,DTTask *p){
            d1 = &d1_;d2 = &d2_;
            key= DT_P2C;
            parent =p;
            parent->child_count ++;
        }
        void run();
        void export_it(fstream &){}
        void finished(){
        if(--parent->child_count ==0)
            parent->finished();
        }
    };
    /*======================================================================*/
    class RCVTask: public DTTask {
    public:
        GData *d1,*d2,*d3;
        RCVTask(GData &d1_,GData &d2_, DTTask *p){
            d3 = &d2_;
            d2 = &d1_;
            d1 = nullptr;
            key= DT_RCV;
            parent=p;;
            parent->child_count++;
        }

        RCVTask(GData &d1_,GData &d2_,GData &d3_,DTTask *p){
            d1 = &d1_;d2 = &d2_;d3 = &d3_;
            key= DT_RCV;
            parent = p;
            parent->child_count ++;
        }
        void run();
        void export_it(fstream &){}
        void finished(){
            if(--parent->child_count == 0)
                parent->finished();
        }
    };
    /*======================================================================*/
    class NVDTask: public DTTask {
    public:
        GData *d1,*d2,*d3;
        NVDTask(GData &d1_,GData &d2_,DTTask *p){
            d3 = &d2_;
            d2 = &d1_;
            d1 = nullptr;
            key= DT_rcv;
            parent = p;
            p->child_count++;
        }

        void finished(){
            if(-parent->child_count ==0)
                parent->finished();
        }
        void run(){}
        void export_it(fstream &){}
    };
    /*======================================================================*/
    class rcvTask: public DTTask {
    public:
        GData *d1,*d2,*d3;
        rcvTask(GData &d1_,GData &d2_,DTTask *p){
            d3 = &d2_;
            d2 = &d1_;
            d1 = nullptr;
            key= DT_rcv;
            parent = p;
            p->child_count++;
        }

        rcvTask(GData &d1_,GData &d2_,GData &d3_,DTTask *p){
            d1 = &d1_;d2 = &d2_;d3 = &d3_;
            key= DT_rcv;
            parent = p;
            p->child_count++;
        }
        void finished(){
            if(-parent->child_count ==0)
                parent->finished();
        }
        void run();
        void export_it(fstream &){}
    };

    /*======================================================================*/
    class rcv_task : public SGTask{
    private:
        X2YList *batch;
    public:
        rcv_task(X2YList *b,DTTask *p):batch(b){
            key = SG_rcv;
            parent = p;
            parent->child_count ++;
        }
        void run();
    };
    /*======================================================================*/
    class NFLTask: public DTTask {
    public:
        GData *d1,*d2;
        int group;
        NFLTask(GData & d1_,GData &d2_,int g,DTTask *p ):group(g){
            d1 = &d1_;
            d2 = &d2_;
            key= DT_NFL;
            parent = p;
            parent->child_count ++;
        }
        void run();
        void export_it(fstream &){}
        void finished(){
        if(--parent->child_count ==0)
            parent->finished();
        }
    };
    /*======================================================================*/
    class nflTask : public DTTask {
    public:
        int group,part;
        nflTask(int g,int i,DTTask *p):group(g),part(i){
            key= DT_nfl;
            parent = p;
            parent->child_count ++;
        }
        void run();
        void finished(){
            if (--parent->child_count == 0 )
                parent->finished();
        }
        void export_it(fstream &){}
    };
    /*======================================================================*/
    class nfl_task : public SGTask {
    private:
        X2YList *batch;
    public:
        int bi,bj;
        nfl_task(X2YList * b,DTTask *p):batch(b){
            key = SG_nfl;
            parent = p;
            p->child_count ++;
        }
        void run();
    };
    /*======================================================================*/
    class fflTask: public DTTask{
    public:
        GData *d;
        fflTask(GData &d_,DTTask *p){
            d = &d_;
            key = DT_ffl;
            p->child_count++;
            parent = p;
        }
        void finished(){
            if (--parent->child_count ==0)
                parent->finished();
        }
        void run();
        void export_it ( fstream &){}
    };
    /*======================================================================*/
    class c2pTask: public DTTask{
    public:
        GData *d1,*d2;
        c2pTask(GData &c, GData &p,DTTask *father){
            d1 = &c;
            d2 = &p;
            key = DT_c2p;
            parent = father;
            father->child_count ++;
        }
        void finished(){
            if (--parent->child_count ==0)
                parent->finished();
        }
        void run();
        void export_it ( fstream &){}
    };
    /*======================================================================*/
    class xltTask: public DTTask{
    public:
        GData *d1,*d2;
        xltTask(GData &from, GData &to,DTTask *p){
            d1 = &from;
            d2 = &to;
            key = DT_xlt;
            p->child_count ++;
            parent = p;
        }
        void run();
        void finished(){
            if (--parent->child_count==0)
                parent->finished();
        }
        void export_it ( fstream &){}
    };
    /*======================================================================*/
    class p2cTask: public DTTask{
    public:
        GData *d1,*d2;
        p2cTask(GData &p, GData &c,DTTask *father){
            d1 = &p;
            d2 = &c;
            key = DT_p2c;
            parent = father;
            father->child_count ++;
        }
        void run();
        void finished(){
            if ( --parent->child_count ==0)
                parent->finished();
        }
        void export_it ( fstream &){}
    };
    /*======================================================================*/
    class c2p_task : public SGTask{
    private:
        BoxList * batch;
    public :
        c2p_task ( BoxList *b,DTTask *p ):batch(b){
            key = SG_c2p;
            parent = p;
            p->child_count ++;
        }
        void run();
    };
    /*======================================================================*/
    class ffl_task : public SGTask{
        BoxList *batch;
    public:
        ffl_task  (BoxList *b,DTTask *p):batch(b){
            key = SG_ffl;
            parent = p;
            p->child_count ++;
        }
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
        xlt_task (F2GList*b,DTTask *p):batch(b){
            key = SG_xlt;
            parent = p;
            p->child_count ++;
        }
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
        p2c_task (BoxList *b,DTTask *p ):batch(b){
            key = SG_p2c;
            parent = p;
            parent->child_count ++;
        }
        void run();
    };
    /*======================================================================*/
    double get_c2p_work(Box *);
    double get_xlt_work(F2GPair *);
    double get_p2c_work(Box *);
    double get_ffl_work(Box *);
    double get_rcv_work(Box *);
    double get_nfl_work(BoxPair *);
    void fmm_taskified();
    void NFL_tasks ( GData & dep,int chunk_no, int chunks_count);
    void for_each_pair_in_batch(X2YList *b, BoxPairFcn func,string);
    void for_each_box_in_batch(BoxList *b, BoxFcn func,string );
    void print_box_pair(string prefix , BoxPair *bp);
    void print_box(string prefix , Box *bp);

    bool is_box_in_list(BoxList &boxes, Box * b);
}
#endif // TASK_SUBMISSION_HPP
