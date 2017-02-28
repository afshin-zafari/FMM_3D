#ifndef FMM_3D_HPP
#define FMM_3D_HPP

#include <iostream>
#include <vector>
#include "types_dist.hpp"

typedef unsigned int uint;

using namespace std;
namespace FMM_3D{
    extern int L_max,L_min,*K,*M;
    /*--------------------------------------------------------------------*/
    struct Point { double x,y,z;};
    /*--------------------------------------------------------------------*/
    class GeneralArray;
    /*--------------------------------------------------------------------*/
    struct Box: public DTBase{
      Point         center;
      double        diagonal;
      int           level,index;
      vector<Box*>  nf_int_list,ff_int_list,children;
      GeneralArray  *I,*V;

      Box(int index_,int level_ );
      Box(){
          I = V= NULL;
      }

    };
    typedef vector<Box*> BoxList;
    /*--------------------------------------------------------------------*/
    class GeneralArray: public DTBase{
      long M,N;
    public:
      GeneralArray(Box &b){}
      GeneralArray(int M_, int N_):M(M_),N(N_){}
      void set_element(int i, int j , double d){}
      double get_element(int i, int j){return 0.0;}
      int rows_count(){return M;}
      int cols_count(){return N;}
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
    typedef vector<Z_near*> ZList;
    /*--------------------------------------------------------------------*/
    class Kappa_hat: public DTBase{
        int j,level;
    public:
         Kappa_hat(int j_, int level_):j(j_),level(level_){}
    };
    typedef vector<Kappa_hat*> KappaList;
    /*--------------------------------------------------------------------*/
    class F_far: public DTBase{
    public:
        F_far(Box &b,int level,Kappa_hat &k){}
    };
    typedef vector<F_far*> FList;
    /*--------------------------------------------------------------------*/
    class F_far_tilde: public DTBase{
    public:
        F_far_tilde(int m, int level,Kappa_hat &k){}
    };
    typedef vector<F_far_tilde *> F_tilde_List;
    /*--------------------------------------------------------------------*/
    class Interpolation: public DTBase{
    public:
        Interpolation(int from, int to, Kappa_hat &){}
    };
    /*--------------------------------------------------------------------*/
    class Exponential: public DTBase{
    public:
        Exponential( int j , int level, int box_idx1, int level1, int box_indx2, int level2){}
    };
    /*--------------------------------------------------------------------*/
    class Translator: public DTBase{
    public:
        Translator (int j, int box_idx1, int box_idx2, int level){}
    };
    typedef vector<Translator*> TList;
    /*--------------------------------------------------------------------*/
    class Green: public DTBase{
    public:
        Green(int m, int level, Kappa_hat){}
    };
    typedef vector<Green*> GList;
    /*--------------------------------------------------------------------*/
    class  Receiving: public DTBase{
    public:
        Receiving(Kappa_hat){}
    };
    typedef vector<Receiving*> RList;
    /*--------------------------------------------------------------------*/
    struct LevelBase{
        BoxList boxes;
        FList F;
        F_tilde_List Ft;
        TList T;
        GList G;
        RList R;
        double *K_x,*K_y,*K_z;
        int K_rows,K_cols;
        LevelBase(){
            K_rows = K_cols = 0;
        }
    };
    /*--------------------------------------------------------------------*/
    typedef vector<LevelBase*> LevelList;
    class Tree{
        public:
            LevelList Level;
            ~Tree(){
                cout << "Tree dtor"<< endl;
            }
    };
    extern Tree *tree;
    /*--------------------------------------------------------------------*/
    template<typename T>
    uint set_length(vector<T*> &L,int n){
        for(int i=0;i<n;i++){
            L.push_back(new T);
        }
        return L.size();
    }
    /*--------------------------------------------------------------------*/
    // Z,V,I,F,Ft,T,P,E,G,R,Kpa
    /*=================================================================================*/
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
