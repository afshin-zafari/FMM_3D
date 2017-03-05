#include <assert.h>
#include "fmm_3d.hpp"
namespace FMM_3D{
    Tree *tree;
    FMMContext *fmm_engine;
    /*---------------------------------------------------------------------------------------*/
    Box &get_box(int index, int level){
        assert(level >= 1);
        assert(index >= 0);
        assert((uint)level <= tree->Level.size());
        assert((uint)index <= tree->Level[level-1]->boxes.size());
        return *tree->Level[level-1]->boxes[index];
    }
    /*---------------------------------------------------------------------------------------*/
    Box::Box(){I = V= NULL;name.assign("Box"); type = DTTypes::Box_type;}
    /*---------------------------------------------------------------------------------------*/
    GeneralArray::GeneralArray(Box &b){
        level = b.level;
        index = b.index;
    }
    /*---------------------------------------------------------------------------------------*/
    void GeneralArray::set_element(int i, int j , ElementType d){//todo
    }
    /*---------------------------------------------------------------------------------------*/
    ElementType GeneralArray::get_element(int i, int j){//todo
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
     void F_far::export_it(fstream &f){
         f << name << "_{"
            << b.index << "," << level <<"}(" ;
            k.export_it(f);
            f << ")";
            DTBase::export_it(f);
     }
    /*---------------------------------------------------------------------------------------*/
    F_far_tilde::F_far_tilde(int m_, int level_,Kappa_hat &k_):m(m_),level(level_),k(k_){
        name.assign("F~");
        type = DTTypes::F_tilde;
        Box b = get_box(m,level);
        host = b.host;
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
    Interpolation::Interpolation(int from_, int to_, Kappa_hat &k_):from(from_),to(to_),k(k_){
        name.assign("Interp");
        type = DTTypes::Interpolation_type;
        host = -1;
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
    Exponential::Exponential( int j , int level, int box_idx1, int level1, int box_idx2, int level2):
        i1(j) , l1(level), i2(box_idx1), l2(level1), i3(box_idx2), l3(level2){
            name.assign("Exponent");
            type = DTTypes::E;
            host = -1;
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
    Translator::Translator(int j, int box_idx1, int box_idx2, int level):
        i1(j) , l1(level), i2(box_idx1), i3(box_idx2){
            name.assign("Translator");
            type = DTTypes::T;
            host = -1;
        }
    /*---------------------------------------------------------------------------------------*/
    void Translator::export_it(fstream &f){
         f << name << "_{"
            << i1 << "," << l1 << "}"
            << "Box(" << i2 <<"," << l2 << ")=>"
            << "Box(" << i3 <<"," << l3 << ")";
            DTBase::export_it(f);
     }
    /*---------------------------------------------------------------------------------------*/
    Green::Green(int m_, int level_, Kappa_hat &k_):m(m_),level(level_),k(k_){
        name.assign("G");
        type = DTTypes::G;
        Box b = get_box(m,level);
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
    void Receiving::export_it(fstream &f){
    }
}
