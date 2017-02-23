#ifndef FMM_3D_HPP
#define FMM_3D_HPP
#include <vector>
typedef unsigned int uint;
using namespace std;
namespace FMM_3D{
extern int L_max;
/*--------------------------------------------------------------------*/
struct Point { double x,y,z;};
/*--------------------------------------------------------------------*/
class GeneralArray;
typedef long DTHandle;
class DTBase{
public:
    int M,N,lead_dim,host,type,g_index;
    double *mem;
    DTHandle *handle;
};
/*--------------------------------------------------------------------*/
struct Box: public DTBase{
  Point         center;
  int           level,index;
  vector<Box*>  nf_int_list,ff_int_list,children;
  GeneralArray  *I,*V;

  Box(int index_,int level_ ){
  }
};
typedef vector<Box*> BoxList;
/*--------------------------------------------------------------------*/
class GeneralArray: public DTBase{
  long M,N;
public:
  GeneralArray(Box &b){}
  GeneralArray(int  _M, int N_){}
};
typedef GeneralArray I_vect;
typedef GeneralArray V_vect;
/*--------------------------------------------------------------------*/
class GeneralMatrix: public DTBase{
public:
    GeneralMatrix(){}
};
/*--------------------------------------------------------------------*/
class Z_near: public GeneralMatrix{
public:
  Z_near(Box b1, Box b2){
  }
};
/*--------------------------------------------------------------------*/
class Kappa_hat: public DTBase{
public:
     Kappa_hat(int level, int j){}
};
/*--------------------------------------------------------------------*/
class F_far: public DTBase{
public:
    F_far(Box &b,int level,Kappa_hat &k){}
};

/*--------------------------------------------------------------------*/
class F_far_tilde: public DTBase{
public:
    F_far_tilde(int m, int level,Kappa_hat &k){}
};
class Interpolation: public DTBase{
public:
    Interpolation(int from, int to, Kappa_hat &){}
};
class Exponential: public DTBase{
public:
    Exponential( int j , int level, int box_idx1, int level1, int box_indx2, int level2){}
};
class Translator: public DTBase{
public:
    Translator (int j, int box_idx1, int box_idx2, int level){}
};
class Green: public DTBase{
public:
    Green(int m, int level, Kappa_hat){}
};
class  Receiving: public DTBase{
public:
    Receiving(Kappa_hat){}
};
// Z,V,I,F,Ft,T,P,E,G,R,Kpa
class DTTask{
public:
    int key;
};
class FMMContext{
public:
    enum TASKS{ MVP,Interpolation, Green_Translate, Green_Interpolate, Receiving};
    void add_mvp_task(DTBase *M,DTBase *v, DTBase *x){}
    void add_interpolation_task(DTBase *E,DTBase *I,DTBase * f1,DTBase *f2){}
    void add_green_translate_task(DTBase *T,DTBase *F,DTBase *G){}
    void add_green_interpolate_task(DTBase *P, DTBase *E, DTBase *G1 , DTBase *G2){}
    void add_receiving_task(DTBase *R, DTBase *G, DTBase *V){}
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
int box_count(int level);
void init();
void finalize();

void compute_near_field();
void MVP_ZI_to_V(GeneralMatrix &,GeneralArray &,GeneralArray &);

void compute_far_field();
void FF_FI_to_F_tilde(F_far &,GeneralArray &, F_far_tilde &);

void  compute_interpolation();
void FF_interpolation(Exponential&,Interpolation&,F_far_tilde&, F_far_tilde&);

void compute_green();
void FF_green(Translator &,F_far_tilde&, Green&);

void compute_green_interpolation();
void FF_green_interpolation(Interpolation&,Exponential&,Green&,Green &);

void compute_receiving();
void NF_receiving(Receiving&,Green&,GeneralArray&);
}
#endif // FMM_3D_HP;
