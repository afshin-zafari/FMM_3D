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
        A.export_it(f);
        //x->export_it(f);
        //y->export_it(f);
        f << endl;
    }
    /*----------------------------------------------------*/
    void GreenInterpTask::export_it(fstream &f){
        string tabs("\t\t");
        f << name << tabs
            << args[0] << tabs
            << args[1] << tabs
            << args[2] << tabs
            << args[3] << endl;
    }
    /*----------------------------------------------------*/
    void GreenTransTask::export_it(fstream &f){
        string tabs("\t\t");
        f << name << tabs
            << args[0] << tabs
            << args[1] << tabs
            << args[2] << endl;
    }
    /*----------------------------------------------------*/
    void ReceivingTask::export_it(fstream &f){
        string tabs("\t\t");
        f << name << tabs
            << args[0]->name << " " << tabs
            << args[1] << tabs
            << args[2] << endl;
    }
    /*----------------------------------------------------*/
    void InterpTask::export_it(fstream &f){
        string tabs("\t\t");
        f << name << tabs
            << args[0] << tabs
            << args[1] << tabs
            << args[2] << tabs
            << args[3] << endl;
    }
    /*----------------------------------------------------*/
    void FarFieldTask::export_it(fstream &f){
        string tabs("\t\t");
        f << name << tabs
            << args[0] << tabs
            << args[1] << tabs
            << args[2] << tabs
            << args[3] << endl;
    }
    /*----------------------------------------------------*/
    void FMMContext::export_tasks(string fn){
        list<DTTask *>::iterator it;
        fstream f;
        f.open(fn,fstream::out);
        for ( it= tasks.begin();it != tasks.end();it++){
            DTTask *t = *it;
            switch(t->key){
            case MVP:
                MVPTask *mt=static_cast<MVPTask*>(t);
                mt->export_it(f);
                break;
            }


        }
        f.close();
    }
    /*----------------------------------------------------*/
}
