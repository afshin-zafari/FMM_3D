#include "fmm_3d.hpp"
namespace FMM_3D{

int L_max,*K,*M;
FMMContext *fmm_engine;
int box_count(int level){return  M[level];}
void add_task(DTBase *){}

/*--------------------------------------------------------------------*/
void MVP_ZI_to_V(GeneralMatrix &Z,GeneralArray &I,GeneralArray &V){
    fmm_engine->add_mvp_task(&Z,&V,&I);
}
/*--------------------------------------------------------------------*/
void FF_FI_to_F_tilde(F_far &F,GeneralArray&I, F_far_tilde &F_t){
    fmm_engine->add_mvp_task(&F,&I,&F_t);
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
    L_max = 1;
    K = new int [L_max];
    M = new int [L_max];
}
/*--------------------------------------------------------------------*/
void finalize(){
    delete [] K;
    delete [] M;
}
/*--------------------------------------------------------------------*/
void compute_near_field(){
    int L  = L_max;
    for (int k=0;k< box_count(L);k++ ){
        Box b_k(k,L);
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
    int L  = L_max;
    for (int m=0;m< box_count(L);m++ ){
        Box b_m(m,L);
        for ( int j=0;j< K[L_max];j++){
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
    for(int lambda=L_max-1; lambda>=0; lambda --){
        for(int m=0;m<box_count(lambda);m++){
            for(int j=0;j<K[lambda];j++){
                Box b_m(m,lambda);
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
    for(int lambda=0; lambda<L_max; lambda ++){
        for(int m=0;m<M[lambda];m++){
            for(int j=0;j<K[lambda];j++){
                Box b_m(m,lambda);
                BoxList &ff=b_m.ff_int_list;
                for(uint n=0;n< ff.size();n++){
                    Box          b_n(n,lambda);
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
    for(int lambda=0; lambda<L_max-1; lambda ++){
        for(int m=0;m<M[lambda];m++){
            for(int j=0;j<K[lambda];j++){
                Box b_m(m,lambda);
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
    Box b_m(m,lambda);
    for(int j=0;j<K[lambda];j++){
        V_vect    V(b_m);
        Kappa_hat k_hat(j,lambda);
        Green     G(m,lambda,k_hat);
        Receiving R(k_hat);
        NF_receiving(R,G,V);
    }
}
void compute_receiving(){
    int lambda = L_max;
    for(int m=0;m<M[lambda];m++){
        box_NF_receiving(m,lambda);
    }
}
}
