#include "types_dist.hpp"
namespace FMM_3D{
    /*----------------------------------------------------*/
    void MVPTask::run(){}
    /*----------------------------------------------------*/
    void GreenInterpTask::run(){}
    /*----------------------------------------------------*/
    void GreenTransTask::run(){}
    /*----------------------------------------------------*/
    void ReceivingTask::run(){}
    /*----------------------------------------------------*/
    void InterpTask::run(){}
    /*----------------------------------------------------*/
    void FarFieldTask::run(){}
    /*----------------------------------------------------*/
    void MVPTask::export_it(fstream &f){
        f << name << "\t\t";
        A->export_it(f); f << "\t\t";
        x->export_it(f); f << "\t\t";
        y->export_it(f); f << "\t\t";
        f << endl;
    }
    /*----------------------------------------------------*/
    void GreenInterpTask::export_it(fstream &f){
        string tabs("\t\t");
        f << name << tabs;
         P.export_it(f); f << tabs;
         E.export_it(f); f << tabs;
        G1.export_it(f); f << tabs;
        G2.export_it(f); f << endl;
    }
    /*----------------------------------------------------*/
    void GreenTransTask::export_it(fstream &f){
        string tabs("\t\t");
        f << name << tabs;
        T.export_it(f); f << tabs;
        F.export_it(f); f << tabs;
        G.export_it(f); f << endl;
    }
    /*----------------------------------------------------*/
    void ReceivingTask::export_it(fstream &f){
        string tabs("\t\t");
        f << name << tabs;
        R.export_it(f); f << tabs;
        G.export_it(f); f << tabs;
        V.export_it(f); f << endl;
    }
    /*----------------------------------------------------*/
    void InterpTask::export_it(fstream &f){
        string tabs("\t\t");
        f << name << tabs;
         E->export_it(f); f << tabs;
         I->export_it(f); f << tabs;
        f1->export_it(f); f << tabs;
        f2->export_it(f);
        f<< endl;
    }
    /*----------------------------------------------------*/
    void FarFieldTask::export_it(fstream &f){
        string tabs("\t\t");
        f << name << tabs;
          F->export_it(f); f << tabs;
          I->export_it(f); f << tabs;
         Ft->export_it(f); f << endl;
    }
    /*----------------------------------------------------*/
    void FMMContext::export_tasks(string fn){
        list<DTTask *>::iterator it;
        fstream f;
        f.open(fn,fstream::out);
        for ( it= tasks.begin();it != tasks.end();it++){
            DTTask *t = *it;
            t->export_it(f);
        }
        f.close();
    }
    /*----------------------------------------------------*/
}
