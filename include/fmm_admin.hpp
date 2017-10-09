#ifndef FMM_ADMIN_HPP
#define FMM_ADMIN_HPP
#include <string>
#include <list>
#include <fstream>
#include <atomic>
#include "ductteip.hpp"
#include "fmm_database.hpp"

using namespace std;
namespace FMM_3D{
  class DTTask;
  class SGTask;
  /*--------------------------------------------------------------------*/
  class FMMContext : public IContext  {
  private:
    list<DTTask*> tasks;
    list<SGTask*> mu_tasks;
  public:
    //enum TASKS{ MVP,Interpolation_Key, Green_Translate, Green_Interpolate, Receiving_Key};
    long *counts;
    ~FMMContext();
    FMMContext();
    void finalize();
    string getTaskName(unsigned long ){return string("");}
    void taskFinished(IDuctteipTask *,TimeUnit){}
    /*--------------------------------------------------------------------*/
    void add_task(SGTask *t);
    /*--------------------------------------------------------------------*/
    void add_task(DTTask *t);
    /*--------------------------------------------------------------------*/
    void add_mvp_task(Z_near *M,
                      GeneralArray *v,
                      GeneralArray *x);
    /*--------------------------------------------------------------------*/
    void add_interpolation_task(Exponential *E,
                                Interpolation *I,
                                F_far_tilde *f1,
                                F_far_tilde *f2);
    /*--------------------------------------------------------------------*/
    void add_green_translate_task(Translator *T,
                                  F_far_tilde*F,
                                  Green*G );
    /*--------------------------------------------------------------------*/
    void add_green_interpolate_task(Interpolation*P,
                                    Exponential*E,
                                    Green*G1,
                                    Green*G2);
    /*--------------------------------------------------------------------*/
    void add_receiving_task(Receiving*R,
                            Green*G,
                            GeneralArray*V);
    /*--------------------------------------------------------------------*/
    void add_farf_task(F_far  *F,
                       GeneralArray*I,
                       F_far_tilde *F_t);
    /*--------------------------------------------------------------------*/

    void export_tasks(string);
    void runKernels(DTTask *t);
  };
  /*--------------------------------------------------------------------*/
  extern FMMContext *fmm_engine;
}
#endif // FMM_ADMIN_HPP
