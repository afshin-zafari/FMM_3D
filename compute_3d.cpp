#include "fmm_3d.hpp"
namespace FMM_3D{

    int L_max,L_min,*K,*M;
    int box_count(int level){return  M[level-1];}
    int kappa_count(int level){return  K[level-1];}
    void import_setup();
    void export_setup();
    /*--------------------------------------------------------------------*/
    void MVP_ZI_to_V(Z_near &Z,GeneralArray &I,GeneralArray &V){
        fmm_engine->add_mvp_task(&Z,&V,&I);
    }
    /*--------------------------------------------------------------------*/
    void FF_FI_to_F_tilde(F_far &F,GeneralArray&I, F_far_tilde &F_t){
        fmm_engine->add_farf_task(&F,&I,&F_t);
    }
    /*--------------------------------------------------------------------*/
    void FF_interpolation(Exponential &E,Interpolation &I,F_far_tilde &F1, F_far_tilde&F2){
        fmm_engine->add_interpolation_task(&E,&I,&F1,&F2);
    }
    /*--------------------------------------------------------------------*/
    void FF_green(Translator &T,F_far_tilde&F, Green&G){
        fmm_engine->add_green_translate_task(&T,&F,&G);
    }
    /*--------------------------------------------------------------------*/
    void FF_green_interpolation(Interpolation&P,Exponential&E,Green&G1,Green &G2){
        fmm_engine->add_green_interpolate_task(&P,&E,&G1,&G2);
    }
    /*--------------------------------------------------------------------*/
    void NF_receiving(Receiving&R,Green&G,GeneralArray&V){
        fmm_engine->add_receiving_task(&R,&G,&V);
    }
    /*--------------------------------------------------------------------*/
    void init(){
        fmm_engine = new FMMContext;
        tree = new Tree;
        import_setup();
        export_setup();
        cout << "L max = " << L_max << endl;
        L_max = 1;
        L_min = 1;
        K[L_max-1]=1;
        M[L_max-1]=1;
    }
    /*--------------------------------------------------------------------*/
    void finalize(){
        cout << __FUNCTION__ << " " << __LINE__ << endl;
        fmm_engine->export_tasks("tasks.txt");
        delete [] K;
        delete [] M;
        delete tree;
        delete fmm_engine;
    }
    /*--------------------------------------------------------------------*/
    void compute_near_field(){
        cout << __FUNCTION__ << " " << __LINE__ << endl;
        int L  = L_max;
        for (int k=0;k< M[L-1];k++ ){
            Box b_k = get_box(k,L);
            BoxList nf = b_k.nf_int_list;
            for ( uint i=0;i< nf.size();i++){
              Box &b_l = *nf[i];
              Z_near Z_kl(b_k,b_l);
              I_vect I_l(b_l);
              V_vect V_k(b_k);
              MVP_ZI_to_V(Z_kl,I_l,V_k);
            }
        }

    }
    /*--------------------------------------------------------------------*/
    void compute_far_field(){
        cout << __FUNCTION__ << " " << __LINE__ << endl;
        int L  = L_max;
        for (int m=0;m< box_count(L);m++ ){
            Box b_m= get_box(m,L);
            for ( int j=0;j< K[L_max-1];j++){
              Kappa_hat k_hat(j,L);
              F_far F(b_m,L,k_hat);
              I_vect I_l(b_m);
              F_far_tilde F_tilde(m,L,k_hat);
              FF_FI_to_F_tilde(F,I_l,F_tilde);
            }
        }
    }
    /*--------------------------------------------------------------------*/
    void compute_interpolation(){
        cout << __FUNCTION__ << " " << __LINE__ << endl;
        for(int lambda=L_max-1; lambda>=L_min; lambda --){
            for(int m=0;m<box_count(lambda);m++){
                for(int j=0;j<kappa_count(lambda);j++){
                    Box b_m = get_box(m,lambda);
                    BoxList &children=b_m.children;
                    for(uint n=0;n< children.size();n++){
                        //Box &b_n=*children[n];
                        Kappa_hat k_hat(j,lambda);
                        F_far_tilde F_tilde_rhs(n,lambda+1,k_hat);//TODO kappa_hat in Martin's Lic. Is it Kappa_{j\lambda}^hat?
                        F_far_tilde F_tilde_lhs(m,lambda  ,k_hat);
                        Interpolation P(lambda+1, lambda,k_hat);
                        Exponential E(j,lambda, m, lambda, n, lambda +1);
                        FF_interpolation(E,P,F_tilde_rhs, F_tilde_lhs);
                    }
                }
            }
        }
    }
    /*--------------------------------------------------------------------*/
    void compute_green(){
        cout << __FUNCTION__ << " " << __LINE__ << endl;
        for(int lambda=L_min; lambda<=L_max; lambda ++){
            for(int m=0;m<box_count(lambda);m++){
                for(int j=0;j<kappa_count(lambda);j++){
                    Box b_m=get_box(m,lambda);
                    BoxList &ff=b_m.ff_int_list;
                    for(uint n=0;n< ff.size();n++){
                        Box          b_n=get_box(n,lambda);
                        Kappa_hat    k_hat(j,lambda);
                        F_far_tilde  F_tilde(n,lambda,k_hat);
                        Translator   T(j,m,n,lambda);
                        Green        G(m,lambda,k_hat);
                        FF_green(T,F_tilde, G);
                    }
                }
            }
        }
    }
    /*--------------------------------------------------------------------*/
    void compute_green_interpolation(){
        cout << __FUNCTION__ << " " << __LINE__ << endl;
        for(int lambda=L_min; lambda<=L_max-1; lambda ++){
            for(int m=0;m<box_count(lambda);m++){
                for(int j=0;j<kappa_count(lambda+1);j++){
                    Box b_m=get_box(m,lambda);
                    BoxList &children=b_m.children;
                    for(uint n=0;n< children.size();n++){
                        //Box           &b_n=*children[n];
                        Kappa_hat     k_hat       (j,lambda  );
                        Kappa_hat     k_hat_higher(j,lambda+1);
                        Interpolation P           (lambda,lambda+1,k_hat);
                        Exponential   E           (j,lambda,   n,lambda+1, m, lambda);
                        Translator    T           (j,m,n,lambda);
                        Green         G_higher    (n,lambda+1,k_hat_higher);
                        Green         G           (m,lambda  ,k_hat       );
                        FF_green_interpolation(P,E,G,G_higher);
                    }
                }
            }
        }
    }
    /*--------------------------------------------------------------------*/
    void box_NF_receiving(int m ,int lambda){
        cout << __FUNCTION__ << " " << __LINE__ << endl;
        Box b_m=get_box(m,lambda);
        for(int j=0;j<K[lambda-1];j++){
            V_vect    V(b_m);
            Kappa_hat k_hat(j,lambda);
            Green     G(m,lambda,k_hat);
            Receiving R(k_hat);
            NF_receiving(R,G,V);
        }
    }
    void compute_receiving(){
        cout << __FUNCTION__ << " " << __LINE__ << endl;
        int lambda = L_max;
        for(int m=0;m<box_count(lambda);m++){
            box_NF_receiving(m,lambda);
        }
    }
}
