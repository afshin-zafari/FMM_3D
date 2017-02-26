#include <cstring>
#include <assert.h>
#include "util_3d.hpp"
#define EXPECTED(x) assert(x)
namespace FMM_3D{
    Importer::Importer(string fn){
        file_name = fn;
        f.open(fn.c_str());
    }
    /*------------------------------------------------------*/
    Importer::~Importer(){
        f.close();
    }
    /*------------------------------------------------------*/
    string trim(const string& str)
    {
        size_t first = str.find_first_not_of(' ');
        if (string::npos == first)
        {
            return str;
        }
        size_t last = str.find_last_not_of(' ');
        return str.substr(first, (last - first + 1));
    }

    /*------------------------------------------------------*/
    void Importer::import_levels_count(){
        stringstream ss;
        ss.str(line);
        string token;
        ss >> token >> L_max ;

        EXPECTED(trim(token)=="Levels");

        set_length(FMM_3D::tree->Level,L_max);
    }
    /*------------------------------------------------------*/
    int Importer::import_boxes_count(){
        int l,n;
        string sLevel,comma,sBoxes;
        stringstream ss;
        ss.str(line);
        ss >> sLevel >> l >> comma >> sBoxes >> n ;
        EXPECTED(trim(sLevel)=="Level");
        EXPECTED(trim(sBoxes)=="Boxes");
        EXPECTED(trim(comma)==",");
        EXPECTED(l<=L_max);
        EXPECTED(l>=1);

        set_length(tree->Level[l-1]->boxes,n);
        return n;
    }
    /*------------------------------------------------------*/
    void Importer::import_box(){
        double x,y,z,d;
        int l,b;
        stringstream ss;
        string Level,comma,Box;
        ss.str(line);
        //Level 2, Box 1, 1.0,1.0,1.0,1.14
        ss >> Level >> l >> comma >> Box >> b >> comma >> x >> comma >> y >> comma >> z >> comma >> d ;
        EXPECTED(trim(Level)=="Level");
        EXPECTED(trim(Box)=="Box");
        EXPECTED(trim(comma)==",");
        EXPECTED(l<=L_max);
        EXPECTED(l>=1);
        EXPECTED((uint)b<=tree->Level[l-1]->boxes.size());
        FMM_3D::Box *box=tree->Level[l-1]->boxes[b-1];
        box->center.x = x;
        box->center.y = y;
        box->center.z = z;
        box->diagonal = d;
    }
    /*------------------------------------------------------*/
    void Importer::import_near_boxes(){}
    /*------------------------------------------------------*/
    void Importer::import_far_boxes(){}
    /*------------------------------------------------------*/
    void Importer::import_child_boxes(){}
    /*------------------------------------------------------*/
    void Importer::import_charges(){}
    /*------------------------------------------------------*/
    void Importer::import_kappas_count(){}
    /*------------------------------------------------------*/
    void Importer::import_kappa(){}
    /*------------------------------------------------------*/
    void Importer::import_translators_count(){}
    /*------------------------------------------------------*/
    void Importer::import_translator(){}
    /*------------------------------------------------------*/
    void Importer::import(){
        while ( !f.eof()){
            getline(f,line);
            if (line.find("Levels") != string::npos){
                import_levels_count();
                continue;
            }
            if (line.find("Boxes") != string::npos){
                import_boxes_count();
                continue;
            }
            if (line.find("Nears") != string::npos){
                import_near_boxes();
                continue;
            }
            if (line.find("Fars") != string::npos){
                import_far_boxes();
                continue;
            }
            if (line.find("Children") != string::npos){
                import_child_boxes();
                continue;
            }
            if (line.find("Kappas") != string::npos){
                import_kappas_count();
                continue;
            }
            if (line.find("Kappa") != string::npos){
                import_kappa();
                continue;
            }
            if (line.find("Translators") != string::npos){
                import_translators_count();
                continue;
            }
            if (line.find("Translator") != string::npos){
                import_translator();
                continue;
            }
            if (line.find("Box") != string::npos){ // this test should be the last. Since all other lines have a "Box".
                import_box();
                continue;
            }
        }
    }
    /*------------------------------------------------------*/
    void import_setup(){

        Importer setup("problem_setup.txt");
        setup.import();
    }
}
