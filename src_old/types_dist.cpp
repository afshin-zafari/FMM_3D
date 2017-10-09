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
        int f1 = tasks.size() ;
        cout << "---- FIRST LEVEL DT ------\n" ;
        std::cout << "# of tasks: " << f1 << std::endl;
        std::cout << "No. of data: " << DTBase::data_count  << std::endl;
        std::cout << "No. of handles: " << DTBase::last_handle<< std::endl;

        f.open(fn,fstream::out);

        for ( it= tasks.begin();it != tasks.end();it++){
            DTTask *t = *it;
            //t->export_it(f);
            t->run();
        }
        cout << "---- SECOND LEVEL DT ------\n" ;
        cout << "# of tasks: " << tasks.size() - f1 << endl;
        cout << "----  THIRD LEVEL SG -------\n";
        cout << "#of tasks: " << mu_tasks.size() << endl;
        for ( int i=DT_FFL; i < NUM_TASK_KEYS+1;i++){
            if ( i == DT_FFL )
                 cout << "--------1st DT Tasks\n" ;
            if ( i == DT_ffl )
                 cout << "--------2nd DT Tasks\n" ;
            if ( i == SG_ffl )
                 cout << "--------SG Tasks \n" ;
            cout << "#Tasks[" << i << "] = " << counts[i] << endl;
        }
        for ( SGTask *t: mu_tasks){
            t->run();
        }
        f.close();
    }
    /*----------------------------------------------------*/
}
