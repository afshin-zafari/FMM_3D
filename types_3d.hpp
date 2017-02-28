#ifndef FMM_3D_HPP
#define FMM_3D_HPP

#include <iostream>
#include <fstream>
#include <vector>

typedef unsigned int uint;

using namespace std;
namespace FMM_3D{
    extern int L_max,L_min,*K,*M;
    typedef long DTHandle;
    /*--------------------------------------------------------------------*/
    class DTBase{
    public:
        int M,N,lead_dim,host,type,g_index;
        double *mem;
        DTHandle *handle;
        string name;
        void export_it(fstream &f){}
    };
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

      Box(){I = V= NULL;name.assign("Box");}

    };
    typedef vector<Box*> BoxList;
    Box &get_box(int index,int level);
    /*--------------------------------------------------------------------*/
    class GeneralArray: public DTBase{
      long M,N;
      int level,index;
    public:
        GeneralArray(Box &b){
            level = b.level;
            index = b.index;
        }
        GeneralArray(int M_, int N_):M(M_),N(N_){}
        void set_element(int i, int j , double d){}
        double get_element(int i, int j){return 0.0;}
        int rows_count(){return M;}
        int cols_count(){return N;}
        void export_it(fstream &f){
            f << name << " "
              << "Box(" << index << "," << level << ")";
            DTBase::export_it(f);
        }
    };
    typedef GeneralArray I_vect;
    typedef GeneralArray V_vect;
    /*--------------------------------------------------------------------*/
    class GeneralMatrix: public DTBase{
    public:
        GeneralMatrix(){}
    };
    /*--------------------------------------------------------------------*/
    struct Z_near: public GeneralMatrix{
    public:
        int l1,i1,l2,i2;
        Z_near(){}
        Z_near ( Z_near &z){
            l1 = z.l1;
            i1=z.i1;
            l2=z.l2;
            i2=z.i2;
        }
        Z_near(Box b1, Box b2){
          name.assign("Z_near");
          l1 = b1.level;
          i1 = b1.index;
          l2 = b2.level;
          i2 = b2.index;
        }
        void export_it(fstream &f){
            f << name << " "
              << "Box(" << i1 << "," << l1 << ")->"
              << "Box(" << i2 << "," << l2 << ")" ;
              DTBase::export_it(f);
        }

    };
    typedef vector<Z_near*> ZList;
    /*--------------------------------------------------------------------*/
    class Kappa_hat: public DTBase{
        int j,level;
    public:
         Kappa_hat(int j_, int level_):j(j_),level(level_){
             name.assign("Kappa");
         }
         void export_it(fstream &f){
             f << name << "_{"
                << j << "," << level << "}";
         }
    };
    typedef vector<Kappa_hat*> KappaList;
    /*--------------------------------------------------------------------*/
    class F_far: public DTBase{
        Box b;
        int level;
        Kappa_hat k;
    public:
        F_far(Box &b_,int level_,Kappa_hat &k_):b(b_),level(level_),k(k_){name.assign("F");}
         void export_it(fstream &f){
             f << name << "_{"
                << b.index << "," << level <<"}(" ;
                k.export_it(f);
                f << ")";
                DTBase::export_it(f);
         }
    };
    typedef vector<F_far*> FList;
    /*--------------------------------------------------------------------*/
    class F_far_tilde: public DTBase{
        int m,level;
        Kappa_hat k;
    public:
        F_far_tilde(int m_, int level_,Kappa_hat &k_):m(m_),level(level_),k(k_){name.assign("F~");}
        void export_it(fstream &f){
             f << name << "_{"
                << m << "," << level <<"}(" ;
                k.export_it(f);
                f << ")";
                DTBase::export_it(f);
         }
    };
    typedef vector<F_far_tilde *> F_tilde_List;
    /*--------------------------------------------------------------------*/
    class Interpolation: public DTBase{
        int from, to;
        Kappa_hat k;
    public:
        Interpolation(int from_, int to_, Kappa_hat &k_):from(from_),to(to_),k(k_){name.assign("Interp");}
        void export_it(fstream &f){
             f << name << "_"
                << from << "^" << to << "(";
                k.export_it(f);
                f << ")";
                DTBase::export_it(f);
         }

    };
    /*--------------------------------------------------------------------*/
    class Exponential: public DTBase{
        int i1,l1,i2,l2,i3,l3;
    public:
        Exponential( int j , int level, int box_idx1, int level1, int box_idx2, int level2):
            i1(j) , l1(level), i2(box_idx1), l2(level1), i3(box_idx2), l3(level2){name.assign("Exponent");}
        void export_it(fstream &f){
             f << name << "_{"
                << i1 << "," << l1 << "}"
                << "Box(" << i2 <<"," << l2 << ")=>"
                << "Box(" << i3 <<"," << l3 << ")";
                DTBase::export_it(f);
         }
    };
    /*--------------------------------------------------------------------*/
    class Translator: public DTBase{
        int i1,l1,i2,l2,i3,l3;
    public:
        Translator (int j, int box_idx1, int box_idx2, int level):
            i1(j) , l1(level), i2(box_idx1), i3(box_idx2){name.assign("Translator");}

        void export_it(fstream &f){
             f << name << "_{"
                << i1 << "," << l1 << "}"
                << "Box(" << i2 <<"," << l2 << ")=>"
                << "Box(" << i3 <<"," << l3 << ")";
                DTBase::export_it(f);
         }    };
    typedef vector<Translator*> TList;
    /*--------------------------------------------------------------------*/
    class Green: public DTBase{
        int m, level;
        Kappa_hat &k;
    public:
        Green(int m_, int level_, Kappa_hat &k_):m(m_),level(level_),k(k_){name.assign("G");}
        void export_it(fstream &f){
             f << name << "_{"
                << m << "," << level <<"}(" ;
                k.export_it(f);
                f << ")";
                DTBase::export_it(f);
         }
    };
    typedef vector<Green*> GList;
    /*--------------------------------------------------------------------*/
    class  Receiving: public DTBase{
        Kappa_hat k;
    public:
        Receiving(Kappa_hat &k_):k(k_){name.assign("R");}
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
