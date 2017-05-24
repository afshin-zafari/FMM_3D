#include "types_dist.hpp"
#include <cassert>
namespace FMM_3D{
    /*----------------------------------------------------*/
    void MVPTask::run(){
        assert(A);
        assert(x);
        assert(y);
    }
    /*----------------------------------------------------*/
    void GreenInterpTask::run(){
        assert(P);
        assert(G1);
        assert(G2);
        assert(E);
    }
    /*----------------------------------------------------*/
    void GreenTransTask::run(){
        //assert(T);
        assert(F);
        assert(G);
    }
    /*----------------------------------------------------*/
    void ReceivingTask::run(){
        assert(R);
        assert(G);
        assert(V);
    }
    /*----------------------------------------------------*/
    void InterpTask::run(){
        assert(E);
        assert(I);
        assert(f1);
        assert(f2);
    }

    /*----------------------------------------------------*/
    void FarFieldTask::run(){}
    /*----------------------------------------------------*/
    void MVPTask::export_it(fstream &f){
        f << name << "\t\t";
        if(A){A->export_it(f); f << "\t\t";}
        if(x){x->export_it(f); f << "\t\t";}
        if(y){y->export_it(f); f << "\t\t";}
        f << endl;
    }
    /*----------------------------------------------------*/
    void GreenInterpTask::export_it(fstream &f){
        string tabs("\t\t");
        f << name << tabs;

         if(P){P->export_it(f); f << tabs;}
         if(E){E->export_it(f); f << tabs;}
        if(G1){G1->export_it(f); f << tabs;}
        if(G2){G2->export_it(f); f << endl;}
    }
    /*----------------------------------------------------*/
    void GreenTransTask::export_it(fstream &f){
        string tabs("\t\t");
        f << name << tabs;
        if(T){T->export_it(f); f << tabs;}
        if(F){F->export_it(f); f << tabs;}
        if(G){G->export_it(f); f << endl;}
    }
    /*----------------------------------------------------*/
    void ReceivingTask::export_it(fstream &f){
        string tabs("\t\t");
        f << name << tabs;
        if(R){R->export_it(f); f << tabs;}
        if(G){G->export_it(f); f << tabs;}
        if(V){V->export_it(f); f << endl;}
    }
    /*----------------------------------------------------*/
    void InterpTask::export_it(fstream &f){
        string tabs("\t\t");
        f << name << tabs;
         if(E){E->export_it(f); f << tabs;}
         if(I){I->export_it(f); f << tabs;}
        if(f1){f1->export_it(f); f << tabs;}
        if(f2){f2->export_it(f);}
        f<< endl;
    }
    /*----------------------------------------------------*/
    void FarFieldTask::export_it(fstream &f){
        string tabs("\t\t");
        f << name << tabs;
          if(F){F->export_it(f); f << tabs;}
          if(I){I->export_it(f); f << tabs;}
         if(Ft){Ft->export_it(f); f << endl;}
    }
    /*----------------------------------------------------*/
    void FMMContext::export_tasks(string fn){
        list<DTTask *>::iterator it;
        fstream f;
        std::cout << "No. of tasks: " << tasks.size() << std::endl;
        std::cout << "No. of data: " << DTBase::data_count  << std::endl;
        std::cout << "No. of handles: " << DTBase::last_handle<< std::endl;
        std::cout << "No. of MVP: " << fmm_engine->counts[MVP]<< std::endl;
        std::cout << "No. of Interp: " << fmm_engine->counts[Interpolation_Key]<< std::endl;
        std::cout << "No. of GTrans: " << fmm_engine->counts[Green_Translate]<< std::endl;
        std::cout << "No. of GInter: " << fmm_engine->counts[Green_Interpolate]<< std::endl;
        std::cout << "No. of REceiving: " << fmm_engine->counts[Receiving_Key]<< std::endl;
        std::cout << "No. of FarField: " << fmm_engine->counts[FarField_key]<< std::endl;
        f.open(fn,fstream::out);

        for ( it= tasks.begin();it != tasks.end();it++){
            DTTask *t = *it;
            //t->export_it(f);
            t->run();
        }
        f.close();
    }
    /*----------------------------------------------------*/
}
