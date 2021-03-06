#ifndef TYPES_DIST_HPP
#define TYPES_DIST_HPP
#include <string>
#include <list>
#include <fstream>
#include <atomic>
#include "types_3d.hpp"

using namespace std;
namespace FMM_3D
{
typedef enum axs {Read,Write} Access;
typedef enum key {MVP,
        Interpolation_Key,
        Green_Translate,
        Green_Interpolate,
        Receiving_Key,
        FarField_key, //5
        DT_FFL,        DT_C2P,        DT_XLT,        DT_P2C,        DT_NFL,        DT_RCV,//11
        DT_ffl,        DT_c2p,        DT_xlt,        DT_p2c,        DT_nfl,        DT_rcv,//17
        SG_ffl,        SG_c2p,        SG_xlt,        SG_p2c,        SG_nfl,        SG_rcv,//23
        NUM_TASK_KEYS
    } TaskKey;
/*--------------------------------------------------------------------*/
class DTTask
{
public:
    int key;
    std::string name;
    int axs[4];
    DTBase *args[4];
    DTTask *parent;
    std::atomic<size_t> child_count;
    virtual void run() = 0 ;
    virtual void finished(){}
    virtual void export_it(fstream&)=0;
};
class SGTask {
public:
    double work;
    int key;
    DTTask *parent;
    virtual void run()=0;
};
/*-------------------------MVP Task ------------------------------------*/
class MVPTask : public DTTask
{
private:
    Z_near *A;
    GeneralArray *x,*y;
public:
    MVPTask(Z_near *M, GeneralArray *v,GeneralArray *z)
    {
        A = M->get();
        x = v->get();
        y = z->get();
        DTTask::name.assign("MVP");
        args[0] = A;
        args[1] = x;
        args[2] = y;
        args[3] = nullptr;
        axs[0]=axs[1]=Access::Read;
        axs[2]=Access::Write;
        key = MVP;
    }
    virtual void run();
    virtual void export_it(fstream &);
};
/*----------------------InterpTask -----------------------------------------*/
class InterpTask : public DTTask
{
private:
    Exponential    *E;
    Interpolation  *I;
    F_far_tilde    *f1,*f2;
public:
    InterpTask(Exponential *E_,Interpolation *I_,F_far_tilde *f1_, F_far_tilde *f2_)    {
        f1 = f1_->get();
        f2 = f2_->get();
        I  =  I_->get();
        E  =  E_->get();
        DTTask::name.assign("interp");
        args[0] = E;
        args[1] = I;
        args[2] = f1;
        args[3] = f2;
        axs[0]=axs[1]=axs[2]= Access::Read;
        axs[3]=Access::Write;
        key = Interpolation_Key;
    }
    virtual void run();
    virtual void export_it(fstream &);
};
/*-----------------GreenTransTask -------------------------------------*/
class GreenTransTask : public DTTask
{
private:
    Translator   *T;
    F_far_tilde  *F;
    Green        *G;
public:
    GreenTransTask(Translator *T_,F_far_tilde*F_, Green*G_  )
    {
        T = T_->get();
        F = F_->get();
        G = G_->get();
        DTTask::name.assign("GreenTrans");
        args[0] = T;
        args[1] = F;
        args[2] = G;
        args[3] = nullptr;
        axs[0]=axs[1]=Access::Read;
        axs[2]=Access::Write;
        key = Green_Translate;
    }
    virtual void run();
    virtual void export_it(fstream &);
};
/*-----------------GreenInterpTask ------------------------------------*/
class GreenInterpTask : public DTTask
{
private:
    Interpolation *P;
    Exponential   *E;
    Green         *G1,*G2 ;
public:
    GreenInterpTask(Interpolation*P_,Exponential*E_,Green*G1_,Green *G2_ )
    {
        P= P_->get();
        E= E_->get();
        G1= G1_->get();
        G2= G2_->get();
        DTTask::name.assign("GreenInterp");
        args[0] = P;
        args[1] = E;
        args[2] = G1;
        args[3] = G2;
        axs[0]=axs[1]=axs[2]=Access::Read;
        axs[3]=Access::Write;
        key = Green_Interpolate;
    }
    virtual void run();
    virtual void export_it(fstream &);
};
/*-----------------ReceivingTask---------------------------------------*/
class ReceivingTask: public DTTask
{
private:
    Receiving    *R;
    Green        *G;
    GeneralArray *V;
public:
    ReceivingTask(Receiving*R_,Green*G_,GeneralArray*V_)
    {
        R = R_->get();
        G = G_->get();
        V = V_->get();
        DTTask::name.assign("Receiving");
        args[0] = R;
        args[1] = G;
        args[2] = V;
        args[3] = nullptr;
        axs[0]=axs[1]=Access::Read;
        axs[2]=Access::Write;
        key = Receiving_Key;
    }
    virtual void run();
    virtual void export_it(fstream &);
};
/*-----------------FarFieldTask---------------------------------------*/
class FarFieldTask: public DTTask
{
private:
    F_far        *F;
    GeneralArray *I;
    F_far_tilde  *Ft;
public:
    FarFieldTask(F_far  *F_, GeneralArray*I_, F_far_tilde *F_t_){
        I = I_->get();
        F = F_->get();
        Ft =  F_t_->get();
        DTTask::name.assign("FarField");
        args[0] = F;
        args[1] = I;
        args[2] = Ft;
        args[3] = nullptr;
        axs[0]=axs[1]=Access::Read;
        axs[2]=Access::Write;
        key  = FarField_key;
    }
    virtual void run();
    virtual void export_it(fstream &);
};
/*--------------------------------------------------------------------*/
class FMMContext
{
private:
    list<DTTask*> tasks;
    list<SGTask*> mu_tasks;
public:
    //enum TASKS{ MVP,Interpolation_Key, Green_Translate, Green_Interpolate, Receiving_Key};
    long counts[NUM_TASK_KEYS+1];
    ~FMMContext(){
        tasks.clear();
        mu_tasks.clear();
    }
    FMMContext(){
        for ( int i=0;i<NUM_TASK_KEYS+1;i++)
            counts[i]=0;
    }
    /*--------------------------------------------------------------------*/
    void add_task(SGTask *t){
        counts[t->key]++;
        mu_tasks.push_back(t);
    }
    /*--------------------------------------------------------------------*/
    void add_task(DTTask *t)
    {
        counts[t->key]++;
        tasks.push_back(t);
    }
    /*--------------------------------------------------------------------*/
    void add_mvp_task(Z_near *M,
                      GeneralArray *v,
                      GeneralArray *x)
    {
        add_task(new MVPTask(M,v,x));
    }
    /*--------------------------------------------------------------------*/
    void add_interpolation_task(Exponential *E,
                                Interpolation *I,
                                F_far_tilde *f1,
                                F_far_tilde *f2)
    {
        add_task(new InterpTask(E,I,f1,f2));
    }
    /*--------------------------------------------------------------------*/
    void add_green_translate_task(Translator *T,
                                  F_far_tilde*F,
                                  Green*G )
    {
        add_task(new GreenTransTask(T,F,G));
    }
    /*--------------------------------------------------------------------*/
    void add_green_interpolate_task(Interpolation*P,
                                    Exponential*E,
                                    Green*G1,
                                    Green*G2)
    {
        add_task(new GreenInterpTask (P,E,G1,G2));
    }
    /*--------------------------------------------------------------------*/
    void add_receiving_task(Receiving*R,
                            Green*G,
                            GeneralArray*V)
    {
        add_task(new ReceivingTask(R,G,V));
    }
    /*--------------------------------------------------------------------*/
    void add_farf_task(F_far  *F,
                       GeneralArray*I,
                       F_far_tilde *F_t)
    {
        add_task(new FarFieldTask(F,I,F_t));
    }
    /*--------------------------------------------------------------------*/

    void export_tasks(string);

    void runKernels(DTTask *t)
    {
        switch(t->key)
        {
        case MVP:
            break;
        case Interpolation_Key:
            break;
        case Green_Translate:
            break;
        case Green_Interpolate:
            break;
        case Receiving_Key:
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
