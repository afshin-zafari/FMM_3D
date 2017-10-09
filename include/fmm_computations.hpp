#ifndef FMM_COMPUTATIONS_HPP
#define FMM_COMPUTATIONS_HPP
#include "fmm_taskbase.hpp"
namespace FMM_3D{
  /*======================================================================*/
  class rcv_task : public SGTask{
  private:
    X2YList *batch;
    string name;
  public:
    string get_name(){return name;}

    rcv_task(X2YList *b,DTTask *p):batch(b){
      key = SG_rcv;
      parent = p;
      parent->child_count ++;
      name.assign("SG_RCV");
    }
    void run(TaskExecutor<Options> &);
  };
  /*======================================================================*/
  class nfl_task : public SGTask {
  private:
    X2YList *batch;
    string name;
  public:
    int bi,bj;
    string get_name(){return name;}
    nfl_task(X2YList * b,DTTask *p):batch(b){
      key = SG_nfl;
      parent = p;
      p->child_count ++;
      name.assign("SG_nfl");
    }
    void run(TaskExecutor<Options> &);
  };
  /*======================================================================*/
  class c2p_task : public SGTask{
  private:
    BoxList * batch;
    string name;
  public :
    string get_name(){return name;}
    
    c2p_task ( BoxList *b,DTTask *p ):batch(b){
      key = SG_c2p;
      parent = p;
      p->child_count ++;
      name.assign("SG_c2p");
    }
    void run(TaskExecutor<Options> &);
  };
  /*======================================================================*/
  class ffl_task : public SGTask{
    BoxList *batch;
    string name;
  public:
    string get_name(){return name;}
    ffl_task  (BoxList *b,DTTask *p):batch(b){
      key = SG_ffl;
      parent = p;
      p->child_count ++;
      name.assign("SG_ffl");
    }
    ~ffl_task  (){
      if ( !batch)
	return;
      batch->clear();
    }
    void run(TaskExecutor<Options> &);
  };
  /*======================================================================*/
  class xlt_task : public SGTask{
    F2GList* batch;
    string name;
  public:
    string get_name(){return name;}
    xlt_task (F2GList*b,DTTask *p):batch(b){
      key = SG_xlt;
      parent = p;
      p->child_count ++;
      name.assign("SG_xlt");
    }
    ~xlt_task (){
      if ( !batch)
	return;
      for ( F2GPair* p: *batch)
	delete p;
      batch->clear();
    }
    void run(TaskExecutor<Options> &);
  };
  /*======================================================================*/
  class p2c_task : public SGTask{
    BoxList *batch;
    string name;
  public:
    string get_name(){return name;}
    p2c_task (BoxList *b,DTTask *p ):batch(b){
      key = SG_p2c;
      parent = p;
      parent->child_count ++;
      name.assign("SG_p2c");
    }
    void run(TaskExecutor<Options> &);
  };
  
}
#endif //FMM_COMPUTATIONS_HPP
