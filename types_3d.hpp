#ifndef FMM_3D_HPP
#define FMM_3D_HPP

#include <iostream>
#include <fstream>
#include <vector>
#include <complex>

#define PRECISION float
#define COMPLEX
typedef unsigned int uint;

using namespace std;
namespace FMM_3D{
    extern int L_max,L_min,*K,*M;
    struct DTHandle{
        long id;
    };

    #ifdef COMPLEX
        typedef complex<PRECISION> ElementType;
    #else
        typedef PRECISION ElementType;
    #endif
    /*--------------------------------------------------------------------*/
    typedef enum dt_types{Box_type,Z,I,V,Kappa_type,F,F_tilde,G,R,T,E,Interpolation_type}DTTypes;
    class DTBase{
    public:
        static long last_handle;
        int M,N,lead_dim,host,type,g_index;
        union{
            ElementType *mem,*data;
        };
        DTHandle *handle;
        string name;
        void export_it(fstream &f){}
        DTBase(){
            handle = new DTHandle;
            handle->id = last_handle++;
        }
    };
    /*--------------------------------------------------------------------*/
    struct Point { double x,y,z;};
    /*--------------------------------------------------------------------*/
    class GeneralArray;
    class Box;
    /*--------------------------------------------------------------------*/
    struct Z_near: public DTBase{
    public:
        int l1,i1,l2,i2;

        Z_near(){}
        Z_near(Box b1, Box b2);
        Z_near(int ,int,bool);
        Z_near(int ,int);
        Z_near *get();
        void set_size(int,int);
        void set_element(int,int,ElementType);
        void export_it(fstream &f);
    };
    struct ZList{
        vector<int> indx;
        vector<Z_near*> data;
    };

    /*--------------------------------------------------------------------*/
    class F_far;
    class Receiving;
    class F_far_tilde;
    struct Box: public DTBase{
      Point         center;
      double        diagonal;
      int           level,index;
      vector<Box*>  nf_int_list,ff_int_list,children;
      vector<int >  nf_int_list_idx,ff_int_list_idx,children_idx,;
      GeneralArray  *I,*V;
      ZList         Z;
      F_far         *F;
      Receiving     *R;
      F_far_tilde   *Ft;
      Box();
    };
    typedef vector<Box*> BoxList;
    Box *get_box_dep(int index,int level);
    #define get_box(i,l)  tree->Level[(l)-1]->boxes[i-1]
    /*--------------------------------------------------------------------*/
    class GeneralArray: public DTBase{
    public:
        int level,index;
        GeneralArray(Box &b);
        GeneralArray(){}
        GeneralArray(int M_, int N_){
            M=M_;
            N=N_;
            data = new ElementType[M*N];
        }
        void        set_element(int i, int j , ElementType d);
        ElementType get_element(int i, int j);
        int         rows_count();
        int         cols_count();
        GeneralArray *get();

        void        export_it(fstream &f);
    };
    class I_vect : public GeneralArray {
    public:
        I_vect(int m, int n,bool):GeneralArray(m,n){}
        I_vect(Box &b);
    };
    class V_vect : public GeneralArray {
    public:
        V_vect(int m, int n,bool):GeneralArray(m,n){}
        V_vect(Box &b);
    };
    /*--------------------------------------------------------------------*/
    class GeneralMatrix: public DTBase{
    public:
        GeneralMatrix(){}
    };
    /*--------------------------------------------------------------------*/
    class Kappa_hat: public DTBase{
        int j,level;
    public:
         Kappa_hat(int j_, int level_);
         Kappa_hat(){}
         void export_it(fstream &f);
    };
    typedef vector<Kappa_hat*> KappaList;
    /*--------------------------------------------------------------------*/
    class F_far: public DTBase{
    public:
        Box        b;
        int        level;
        Kappa_hat  k;
        F_far(Box &b_,int level_,Kappa_hat &k_);
        F_far(int,int,bool);
        F_far(int,int);
        F_far *get();
        void set_size(int m, int n);
        void set_element(int , int , ElementType);
         void export_it(fstream &f);
    };
    typedef vector<F_far*> FList;
    /*--------------------------------------------------------------------*/
    class F_far_tilde: public DTBase{
        int m,level;
        Kappa_hat k;
    public:
        F_far_tilde(int m_, int level_,Kappa_hat &k_);
        F_far_tilde *get();
        void export_it(fstream &f);
    };
    typedef vector<F_far_tilde *> F_tilde_List;
    /*--------------------------------------------------------------------*/
    typedef struct {
        std::vector<std::pair<uint32_t,uint32_t>> index;
        std::vector<double> data;
    }SparseMat_t;
    typedef SparseMat_t SparseMat;
    class Interpolation: public DTBase{
        int from, to;
        Kappa_hat k;
        SparseMat *A1,*A2;
    public:
        Interpolation(int from_, int to_, Kappa_hat &k_);
        Interpolation *get();
        Interpolation(SparseMat *a, SparseMat *b):A1(a),A2(b){}
        void export_it(fstream &f);
    };
    /*--------------------------------------------------------------------*/
    class Exponential: public DTBase{
        int i1,l1,i2,l2,i3,l3;
    public:
        Exponential( int j, int lv, int b_idx1, int lv1, int b_idx2, int lv2);
        Exponential *get();
        void export_it(fstream &f);
    };
    /*--------------------------------------------------------------------*/
    class Translator: public DTBase{
        int i1,l1,i2,l2,i3,l3;
    public:
        Translator (int j, int box_idx1, int box_idx2, int level);
        Translator *get();
        Translator (int M, int N,ElementType *);
        void export_it(fstream &f);
     };
    typedef vector<Translator*> TList;
    /*--------------------------------------------------------------------*/
    class Green: public DTBase{
        int m, level;
        Kappa_hat k;
    public:
        Green(int m_, int level_, Kappa_hat &k_);
        void export_it(fstream &f);
    };
    typedef vector<Green*> GList;
    /*--------------------------------------------------------------------*/
    class  Receiving: public DTBase{
    public:
        Box b;
        Kappa_hat k;
        Receiving(Box b_,Kappa_hat &k_);
        Receiving(int,int);
        Receiving(int,int,bool);
        void set_size(int m, int n);
        void set_element(int , int , ElementType);
        void export_it(fstream &);
    };
    typedef vector<Receiving*> RList;
    /*--------------------------------------------------------------------*/
    struct LevelBase{
        BoxList         boxes;
        FList           F;
        F_tilde_List    Ft;
        TList           T;
        GList           G;
        RList           R;
        ElementType     *K_x,*K_y,*K_z;
        int             K_rows,K_cols;
        Interpolation   *P2C,*C2P;

        LevelBase(){K_rows = K_cols = 0;}
    };
    /*--------------------------------------------------------------------*/
    typedef vector<LevelBase*> LevelList;
    class Tree{
        public:
            LevelList Level;
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
