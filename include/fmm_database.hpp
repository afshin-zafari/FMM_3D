#ifndef FMM_DTATABASE_HPP
#define FMM_DTATABASE_HPP

#include <iostream>
#include <fstream>
#include <vector>
#include <complex>

#undef COMPLEX
#define LOG_FMM 1
typedef unsigned int uint;

using namespace std;
namespace FMM_3D{
    extern int L_max,L_min,*K,*M;
    /*---------------------------------------------------------------*/
    typedef struct{
        union{// no. of blocks in first level
            int B1,groups,g,group_count;
        };
        union {// no. of blocks in second level
            int B2,parts,part_count;
        };
        union {// interpolation points
            int interp_points,m;
        };
        double work_min;
        int iter_batch_count;
	char data_path[200];
    }Parameters_t;
    extern Parameters_t Parameters;
    struct DTHandle{
        long id;
    };
    typedef complex<double> ComplexElementType;
    typedef double ElementType;
    /*--------------------------------------------------------------------*/
    typedef enum dt_types{Box_type,Z,I,V,Kappa_type,F,F_tilde,G,R,T,E,Interpolation_type}DTTypes;
  class DTBase{
    public:
        static long last_handle,data_count;
        int M,N,lead_dim,host,type,g_index;
        union{
            ElementType *mem,*data;
        };
        ComplexElementType *complex_data;
        DTHandle *handle;
        string name;
        void get_dims(int &m , int &n){
            m = M;
            n = N;
        }
        void export_it(fstream &f){}
        DTBase(){
            handle = new DTHandle;
            data_count ++;
            handle->id = last_handle++;
        }
        ~DTBase(){
            data_count --;
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
    class Green;
    class Exponential;
    /*--------------------------------------------------------------------*/
    struct Box: public DTBase{
      Point         center;
      double        diagonal;
      int           level,index,edges,group,part;
      Box           *parent;
      vector<Box*>  nf_int_list,ff_int_list,children;
      vector<int >  nf_int_list_idx,ff_int_list_idx,children_idx;
      GeneralArray  *I,*V;
      ZList          Z;
      F_far         *F;
      Receiving     *R;
      Green         *G;
      F_far_tilde   *Ft;
      Exponential   *E;
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
        F_far_tilde(int m_, int level_,bool);
        F_far_tilde *get();
        void export_it(fstream &f);
        ElementType &get_element(int i , int j);
        void set_element(int i, int j, ElementType v);
        void expand();
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
    public:
        SparseMat *A1,*A2;
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
        Exponential(int , int , bool);
        Exponential *get();
        void export_it(fstream &f);
    };
    /*--------------------------------------------------------------------*/
    class Translator: public DTBase{
        int i1,i2,l1,d[3];
    public:
        Translator (int j, int box_idx1, int box_idx2, int level);
        Translator *get();
        Translator (int32_t d[],int M, int N,ComplexElementType *);
        void export_it(fstream &f);
     };
    typedef vector<Translator*> TList;
    /*--------------------------------------------------------------------*/
    class Green: public DTBase{
        int m, level;
        Kappa_hat k;
    public:
        Green(int m_, int level_, Kappa_hat &k_);
        Green(int m_, int level_, bool);
        Green *get();
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
        Receiving *get();
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
        ElementType     *K_theta,*K_phi;
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
    /*=================================================================================*/
    int box_count(int level);
  void init(int , char *[]);
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
#endif // FMM_DTATABASE_HPP
