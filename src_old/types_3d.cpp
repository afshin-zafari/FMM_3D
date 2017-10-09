#include <assert.h>
#include <cmath>
#include "fmm_3d.hpp"
namespace FMM_3D{
    long DTBase::last_handle=0;
    long DTBase::data_count=0;
    Tree *tree;
    Parameters_t Parameters;
    FMMContext *fmm_engine;
    /*---------------------------------------------------------------------------------------*/
    Box *get_box_dep(int index, int level){
        assert(level >= 1);
        assert(index >= 0);
        assert((uint)level <= tree->Level.size());
        assert((uint)index <= tree->Level[level-1]->boxes.size());

        Box *bb=tree->Level[level-1]->boxes[index];
        return bb;
    }
    /*---------------------------------------------------------------------------------------*/
    Box::Box(){
        I  = V = nullptr;
        F  = nullptr;
        R  = nullptr;
        Ft = nullptr;
        name.assign("Box");
        type = DTTypes::Box_type;
    }
    /*---------------------------------------------------------------------------------------*/
    GeneralArray::GeneralArray(Box &b){
        level = b.level;
        index = b.index;
    }
    /*---------------------------------------------------------------------------------------*/
    void GeneralArray::set_element(int i, int j , ElementType d){//deprecated
    }
    /*---------------------------------------------------------------------------------------*/
    ElementType GeneralArray::get_element(int i, int j){//deprecated
        #ifdef COMPLEX
            ElementType e(0,0);
            return e;
        #else
            return 0.0;
        #endif // COMPLEX
    }
    /*---------------------------------------------------------------------------------------*/
    int GeneralArray::rows_count(){
        return M;
    }
    /*---------------------------------------------------------------------------------------*/
    int GeneralArray::cols_count(){
        return N;
    }
    /*---------------------------------------------------------------------------------------*/
    GeneralArray *GeneralArray::get(){
        Box &b=*tree->Level[level-1]->boxes[index-1];
        if (type ==DTTypes::I){
            return b.I;
        }
        else{
            return b.V;
        }
        return nullptr;
    }
    /*---------------------------------------------------------------------------------------*/
    void GeneralArray::export_it(fstream &f){
        f << name << " "
          << "Box(" << index << "," << level << ")";
        DTBase::export_it(f);
    }
    /*---------------------------------------------------------------------------------------*/
    I_vect::I_vect(Box &b):GeneralArray(b){
        name.assign("I_vect");
        type = DTTypes::I;
        host = b.host;
    }
    /*---------------------------------------------------------------------------------------*/
    V_vect::V_vect(Box &b):GeneralArray(b){
        name.assign("V_vect");
        type = DTTypes::V;
        host = b.host;
    }
    /*---------------------------------------------------------------------------------------*/
    Z_near *Z_near::get(){
        Z_near *z=nullptr;
        Box &iBox = *tree->Level[l1-1]->boxes[i1-1];
        int n=iBox.Z.indx.size();
        for(int i=0;i<n;i++){
            if (iBox.Z.indx[i] == i2){
                return iBox.Z.data[i];
            }
        }
        std::cout << "Z(" << i1 << "," << i2 <<") not found." << std::endl;
        return z;
    }
    /*---------------------------------------------------------------------------------------*/
    Z_near::Z_near(Box b1, Box b2){
        name.assign("Z_near");
        l1 = b1.level;
        i1 = b1.index;
        l2 = b2.level;
        i2 = b2.index;
        type = DTTypes::Z;
        host = -1;
    }
    /*---------------------------------------------------------------------------------------*/
    Z_near::Z_near(int m,int n){}
    Z_near::Z_near(int m,int n,bool){
        M = m;
        N = n;
        complex_data= new ComplexElementType[m*n];
    }
    /*---------------------------------------------------------------------------------------*/
    void Z_near::set_element(int,int,ElementType){//deprecated
    }
    /*---------------------------------------------------------------------------------------*/
    void Z_near::set_size(int m,int n){
        M = m;
        N = n;
    }
    /*---------------------------------------------------------------------------------------*/
    void Z_near::export_it(fstream &f){
        f << name << " "
          << "Box(" << i1 << "," << l1 << ")->"
          << "Box(" << i2 << "," << l2 << ")" ;
          DTBase::export_it(f);
    }
    /*---------------------------------------------------------------------------------------*/
     Kappa_hat::Kappa_hat(int j_, int level_):j(j_),level(level_){
         name.assign("Kappa");
         type = DTTypes::Kappa_type;
         host = -1;
     }
    /*---------------------------------------------------------------------------------------*/
     void Kappa_hat::export_it(fstream &f){
         f << name << "_{"
            << j << "," << level << "}";
     }
    /*---------------------------------------------------------------------------------------*/
    F_far::F_far(Box &b_,int level_,Kappa_hat &k_):b(b_),level(level_),k(k_){
        name.assign("F");
        type = DTTypes::F;
        host = b.host;
    }
    /*---------------------------------------------------------------------------------------*/
    F_far::F_far(int m,int n){}
    F_far::F_far(int m,int n,bool){
        M = m;
        N = n;
        data = new ElementType[m*n];
    }
    /*---------------------------------------------------------------------------------------*/
    void F_far::set_size(int m, int n){
        M=m;
        N=n;
    }
    /*---------------------------------------------------------------------------------------*/
    void F_far::set_element(int , int , ElementType){//deprecated
    }
    /*---------------------------------------------------------------------------------------*/
    F_far *F_far::get(){
        Box &bb=*get_box(b.index,b.level);
        if(!bb.F)
            std::cout <<"F for B("<<b.index<<","<<b.level<<") not found." << std::endl;
        return bb.F;
    }
    /*---------------------------------------------------------------------------------------*/
     void F_far::export_it(fstream &f){

         f << name << "_{"
            << b.index << "," << level <<"}(" ;
            k.export_it(f);
            f << ")";
            DTBase::export_it(f);
     }
    /*---------------------------------------------------------------------------------------*/
    F_far_tilde::F_far_tilde(int m, int n,bool){
        M = m;
        N = n;
        data = new ElementType[m*n] ;
    }
    /*---------------------------------------------------------------------------------------*/
    F_far_tilde::F_far_tilde(int m_, int level_,Kappa_hat &k_):m(m_),level(level_),k(k_){
        name.assign("F~");
        type = DTTypes::F_tilde;
        Box b = *get_box(m,level);
        host = b.host;
    }
    /*---------------------------------------------------------------------------------------*/
    F_far_tilde *F_far_tilde::get(){
        Box &bb=*get_box(m,level);
        if(!bb.Ft)
            std::cout <<"F~ for B("<<m<<","<<level<<") not found." << std::endl;
        return bb.Ft;
    }
    /*---------------------------------------------------------------------------------------*/
    void F_far_tilde::export_it(fstream &f){
         f << name << "_{"
            << m << "," << level <<"}(" ;
            k.export_it(f);
            f << ")";
            DTBase::export_it(f);
     }
    /*---------------------------------------------------------------------------------------*/
    ElementType &F_far_tilde::get_element(int i, int j){
        return data[j*M+i];
    }
    /*---------------------------------------------------------------------------------------*/
    void F_far_tilde::set_element(int i, int j, ElementType v){
        data[j*M+i] = v;
    }
    /*---------------------------------------------------------------------------------------*/
    void F_far_tilde::expand(){
        int m=Parameters.m;
        //assumption: F~ has Mx(m/2+N+m/2) dims
        /*  m/2  ----N----  m/2
            l k  a b c d e  o n
            g f  f g h i j  j i
            b a  k l m n o  e d
        */
        for (int i=0;i<M;i++){
            for (int j=0;j<N;j++){
            // F(i,j) = F(M-i-1,m-j)        ; i<M/2 j<m/2
            if (i <  M/2 and  j<m/2)
                set_element(i,j,get_element(M-i-1,m-j));
            // F(i,j) = F(M-i-1,2*N+m-1-j)  ; i<M/2 j>N+m/2
            if ( i<M/2 and j >(N+m/2))
                set_element(i,j,get_element(M-i-1,2*N+m-1-j));
            // F(i,j) = F(M-i-1,m-j)        ; i>M/2 j<m/2
            if ( i>M/2 and j <(m/2))
                set_element(i,j,get_element(M-i-1,m-j));
            // F(i,j) = F(M-i-1,2*N+m-1-j)  ; i>M/2 j>N+m/2
            if ( i>M/2 and j >(N+m/2))
                set_element(i,j,get_element(M-i-1,2*N+m-1-j));
            // F(i,j) = F(i    ,m-j)        ; i=M/2 j<m/2
            if ( i==M/2 and j<m/2)
                set_element(i,j,get_element(i,m-j));
            // F(i,j) = F(i    ,2*N+m-1-j)  ; i=M/2 j>N+m/2
            if ( i==M/2 and j>(2*N+m/2))
                set_element(i,j,get_element(i,2*N+m-1-j));
            }
        }
    }
    /*---------------------------------------------------------------------------------------*/
    Interpolation::Interpolation(int from_, int to_, Kappa_hat &k_):from(from_),to(to_),k(k_){
        name.assign("Interp");
        type = DTTypes::Interpolation_type;
        host = -1;
    }
    /*---------------------------------------------------------------------------------------*/
    Interpolation * Interpolation::get(){
        LevelBase &L=*tree->Level[from-1];
        if ( from < to){
            if(!L.C2P)
                std::cout << "Interp("<<from <<"," <<to<<") not found."<< std::endl;
            return L.C2P;
        }
        else{
            if(!L.P2C)
                std::cout << "Interp("<<from <<"," <<to<<") not found."<< std::endl;
            return L.P2C;
        }
        return nullptr;
    }
    /*---------------------------------------------------------------------------------------*/
    void Interpolation::export_it(fstream &f){
        f << name << "_"
          << from << "^" << to << "(";
        k.export_it(f);
        f << ")";
        DTBase::export_it(f);
     }
    /*---------------------------------------------------------------------------------------*/
    Exponential::Exponential( int m , int n, bool){
        M = m;
        N=n;
        data = new ElementType[m*n];
    }
    Exponential::Exponential( int j , int level, int box_idx1, int level1, int box_idx2, int level2):
        i1(j) , l1(level), i2(box_idx1), l2(level1), i3(box_idx2), l3(level2){
            name.assign("Exponent");
            type = DTTypes::E;
            host = -1;
        }
    /*---------------------------------------------------------------------------------------*/
    Exponential* Exponential::get(){
        Box &from=*get_box(i2,l2);
        Box &to  =*get_box(i3,l3);
        Exponential *e=nullptr;
        if (l3 >l2) // to > from => P2C
            e=to.E;
        else
            e=from.E;
        if(!e)
            std::cout << "Exponential for B("<<i2<<","<<l2<<") --> B("<<i3<<","<<l3<<") not found.\n" ;
        return e;
    }
    /*---------------------------------------------------------------------------------------*/
    void Exponential::export_it(fstream &f){
         f << name << "_{"
            << i1 << "," << l1 << "}"
            << "Box(" << i2 <<"," << l2 << ")=>"
            << "Box(" << i3 <<"," << l3 << ")";
            DTBase::export_it(f);
     }
    /*---------------------------------------------------------------------------------------*/
    Translator::Translator(int32_t d_[],int M_, int N_, ComplexElementType* mat){
        d[0]=d_[0];d[1]=d_[1];d[2]=d_[2];
        M=M_;
        N=N_;
        complex_data=mat;
        name.assign("Translator");
        type = DTTypes::T;
        host = -1;
    }
    /*---------------------------------------------------------------------------------------*/
    double distance(Point p1, Point p2){
        return
        std::sqrt(
                  std::pow(p1.x - p2.x,2)+
                  std::pow(p1.y - p2.y,2)+
                  std::pow(p1.z - p2.z,2)
                  );
    }
    /*---------------------------------------------------------------------------------------*/
    Translator *Translator::get(){
        Box &b1=*get_box(i1,l1);
        Box &b2=*get_box(i2,l1);
        double cl = std::sqrt(std::pow(b1.diagonal ,2)/3.0);
        double di=distance(b1.center,b2.center);
        int d = di/cl;
        int n = tree->Level[l1-1]->T.size();
        Translator *t=nullptr;
        if ( d<=n and d>=0)
            t=tree->Level[l1-1]->T[d%n];
        if(!t)
            std::cout << "T for B(" <<i1<<"," << l1<<")--B("<<i2<<","<<l1<<") with distance: "<<d<<" not found." << std::endl;
        return t;

    }
    /*---------------------------------------------------------------------------------------*/
    Translator::Translator(int j, int box_idx1, int box_idx2, int level):
          i1(box_idx1), i2(box_idx2),l1(level){
            name.assign("Translator");
            type = DTTypes::T;
            host = -1;
        }
    /*---------------------------------------------------------------------------------------*/
    void Translator::export_it(fstream &f){
         f << name << "_{"
            << i1 << "," << l1 << "}"
            << "Box(" << i2 <<"," << l1 << ")";
            DTBase::export_it(f);
     }
    /*---------------------------------------------------------------------------------------*/
    Green *Green::get(){
        Box &box = *tree->Level[level-1]->boxes[m-1];
        Green *g = box.G;
        if (!g)
            std::cout <<"G("<< m<< ","<< level<<") not found."<< std::endl;
        return g;
    }
    /*---------------------------------------------------------------------------------------*/
    Green::Green(int m, int n,bool){
        M = m;
        N = n;
        mem = new ElementType[m*n];
    }
    /*---------------------------------------------------------------------------------------*/
    Green::Green(int m_, int level_, Kappa_hat &k_):m(m_),level(level_),k(k_){
        name.assign("G");
        type = DTTypes::G;
        Box b = *get_box(m,level);
        host = b.host;
    }
    /*---------------------------------------------------------------------------------------*/
    void Green::export_it(fstream &f){
         f << name << "_{"
            << m << "," << level <<"}(" ;
            k.export_it(f);
            f << ")";
            DTBase::export_it(f);
     }
    /*---------------------------------------------------------------------------------------*/
    Receiving::Receiving(Box b_,Kappa_hat &k_):b(b_),k(k_){
        name.assign("R");
        type = DTTypes::R;
        host = b.host;
    }
    /*---------------------------------------------------------------------------------------*/
    Receiving::Receiving(int m,int n){
    }
    /*---------------------------------------------------------------------------------------*/
    Receiving *Receiving::get(){
        Receiving *r=tree->Level[b.level-1]->boxes[b.index-1]->R;
        if(!r)
            std::cout << "R(" <<b.index<<"," <<b.level << ") not found." <<std::endl;
        return r;
    }
    /*---------------------------------------------------------------------------------------*/
    Receiving::Receiving(int m,int n,bool){
        M = m;
        N = n;
        data = new ElementType[m*n];
    }
    /*---------------------------------------------------------------------------------------*/
    void Receiving::set_size(int m, int n){
        M=m;
        N=n;
    }
    /*---------------------------------------------------------------------------------------*/
    void Receiving::set_element(int , int , ElementType){//deprecated
        //#warning "This method should not be used any more"
    }
    /*---------------------------------------------------------------------------------------*/
    void Receiving::export_it(fstream &f){
    }
}
