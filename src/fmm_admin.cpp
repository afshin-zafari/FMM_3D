#include "fmm_admin.hpp"
#include "fmm_taskbase.hpp"
namespace FMM_3D{
  FMMContext *fmm_engine;
  /*--------------------------------------------------------------------*/
  FMMContext::FMMContext():IContext("FMM3D"){
    counts = new long[NUM_TASK_KEYS+1];
    for ( int i=0;i<NUM_TASK_KEYS+1;i++)
      counts[i]=0;
  }
  /*--------------------------------------------------------------------*/
  FMMContext::~FMMContext(){
    tasks.clear();
    mu_tasks.clear();
    delete [] counts;
  }
  /*--------------------------------------------------------------------*/
  void FMMContext::add_task(SGTask *t){
    counts[t->key]++;
    mu_tasks.push_back(t);
  }
  /*--------------------------------------------------------------------*/
  void FMMContext::add_task(DTTask *t)
  {
    counts[t->key]++;
    tasks.push_back(t);
    dtEngine.register_task(t);
  }
  /*--------------------------------------------------------------------*/  
  void FMMContext::finalize(){
    dtEngine.finalize();
  }
  /*--------------------------------------------------------------------*/
  void FMMContext::add_mvp_task(Z_near *M,
				GeneralArray *v,
				GeneralArray *x)
  {
    //add_task(new MVPTaskOld(M,v,x));
  }
  /*--------------------------------------------------------------------*/
  void FMMContext::add_interpolation_task(Exponential *E,
					  Interpolation *I,
					  F_far_tilde *f1,
					  F_far_tilde *f2)
  {
    //add_task(new InterpTask(E,I,f1,f2));
  }
  /*--------------------------------------------------------------------*/
  void FMMContext::add_green_translate_task(Translator *T,
					    F_far_tilde*F,
					    Green*G )
  {
    //add_task(new GreenTransTask(T,F,G));
  }
  /*--------------------------------------------------------------------*/
  void FMMContext::add_green_interpolate_task(Interpolation*P,
					      Exponential*E,
					      Green*G1,
					      Green*G2)
  {
    //add_task(new GreenInterpTask (P,E,G1,G2));
  }
  /*--------------------------------------------------------------------*/
  void FMMContext::add_receiving_task(Receiving*R,
				      Green*G,
				      GeneralArray*V)
  {
    //add_task(new ReceivingTask(R,G,V));
  }
  /*--------------------------------------------------------------------*/
  void FMMContext::add_farf_task(F_far  *F,
				 GeneralArray*I,
				 F_far_tilde *F_t)
  {
    //add_task(new FarFieldTask(F,I,F_t));
  }
  /*--------------------------------------------------------------------*/
  void FMMContext::export_tasks(string){}
  /*--------------------------------------------------------------------*/
  void FMMContext::runKernels(DTTask *t)
  {
  }
  /*--------------------------------------------------------------------*/
}
