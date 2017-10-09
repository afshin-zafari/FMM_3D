#include <cstring>
#include <assert.h>
#include "fmm_util.hpp"
#include "ductteip.hpp"
#include "fmm_setup.hpp"
#define EXPECTED(x) if ( !(x)) { cout << "Assertion failed:"<< #x << endl << "The input line " << line_no << ":"<< endl << line << endl;}
#define EXPECTED_MSG(x,m) if ( !(x)) { cout << "Assertion failed:\n"<< (m).rdbuf() << endl << "The input line " << line_no << ":"<< endl << line << endl;}
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
        K = new int [L_max];
        M = new int [L_max];

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
        FMM_3D::M[l-1]=n;
        return n;
    }
    /*------------------------------------------------------*/
    void Importer::import_level_box(stringstream &s, int &level,int & box){

        string Level,comma,Box;
        s.str(line);
        //Level 2, Box 1, ...
        s >> Level >> level >> comma >> Box >> box >> comma ;
        EXPECTED(trim(Level)=="Level");
        EXPECTED(trim(Box)=="Box");
        EXPECTED(trim(comma)==",");
        EXPECTED(level<=L_max);
        EXPECTED(level>=1);
    }
    /*------------------------------------------------------*/
    void Importer::import_box(){
        double x,y,z,d;
        int l,b;
        stringstream ss;
        string comma;
        import_level_box(ss,l,b);
        ss >> x >> comma >> y >> comma >> z >> comma >> d ;
        EXPECTED(d>1e-9);
        EXPECTED((uint)b<=tree->Level[l-1]->boxes.size());
        FMM_3D::Box *box=tree->Level[l-1]->boxes[b-1];
        box->center.x = x;
        box->center.y = y;
        box->center.z = z;
        box->diagonal = d;
        box->index    = b;
        box->level    = l;
    }
    /*------------------------------------------------------*/
    void Importer::import_child_boxes(){
        int l,b,c;
        stringstream ss;
        string Level,comma,Box,Children;
        //Level 1, Box 1, Children 2, 1, 20
        import_level_box(ss,l,b);
        ss >> Children >> c ;
        EXPECTED(c>=1);
        EXPECTED((uint)b<=tree->Level[l-1]->boxes.size());

        FMM_3D::Box *box=tree->Level[l-1]->boxes[b-1];
        int child;
        for(int i=0;i<c;i++){
            stringstream err;
            err.str("");
            ss>> comma >> child ;
            EXPECTED_MSG((uint)child<=tree->Level[l]->boxes.size(), err << endl << " Child index " << child << " exceeds the limit." << endl);
            FMM_3D::Box *ChildBox=tree->Level[l]->boxes[child-1];
            box->children.push_back(ChildBox);
        }
    }
    /*------------------------------------------------------*/
    void Importer::import_far_boxes(){
        int l,b,nf;
        stringstream ss;
        import_level_box(ss,l,b);
        string Fars,comma;
        ss >> Fars >> nf ;

        FMM_3D::Box *box=tree->Level[l-1]->boxes[b-1];
        int far;
        for(int i=0;i<nf;i++){
            stringstream err;
            err.str("");
            ss>> comma >> far ;
            EXPECTED_MSG((uint)far<=tree->Level[l-1]->boxes.size(), err << endl << " Far box index " << far << " exceeds the limit." << endl);
            FMM_3D::Box *FarBox=tree->Level[l-1]->boxes[far-1];
            box->ff_int_list.push_back(FarBox);
        }


    }
    /*------------------------------------------------------*/
    void Importer::import_near_boxes(){
        int l,b,nn;
        stringstream ss;
        import_level_box(ss,l,b);
        string Nears,comma;
        ss >> Nears >> nn >> comma ;

        FMM_3D::Box *box=tree->Level[l-1]->boxes[b-1];
        int near;
        for(int i=0;i<nn;i++){
            stringstream err;
            err.str("");
            ss>> near >> comma;
            EXPECTED_MSG((uint)near<=tree->Level[l-1]->boxes.size(), err << endl << " Near box index " << near << " exceeds the limit." << endl);
            FMM_3D::Box *NearBox=tree->Level[l-1]->boxes[near-1];
            box->nf_int_list.push_back(NearBox);
        }
    }
    /*------------------------------------------------------*/
    void Importer::import_kappas_count(){
        int l,nx,ny;
        stringstream ss;
        ss.str(line);
        string Level, Kappas, comma;
        ss >> Level >> l >> comma >> Kappas >> nx >> comma >> ny;
//        tree->Level[l-1]->K_x = new ElementType [ny*nx];
//        tree->Level[l-1]->K_y = new ElementType [ny*nx];
//        tree->Level[l-1]->K_z = new ElementType [ny*nx];
        tree->Level[l-1]->K_rows = ny;
        tree->Level[l-1]->K_cols = nx;
        K[l-1]=nx*ny;
    }
    /*------------------------------------------------------*/
    void Importer::import_kappa_x(){
        int l,c;
        stringstream ss,err;
        ss.str(line);
        string Level,Kappa,_x, comma;
        ss >> Level >> l >> comma >> Kappa >> _x >> c ;

        int ny = tree->Level[l-1]->K_rows;
        int nx = tree->Level[l-1]->K_cols;
        EXPECTED_MSG((c == (nx*ny)), err << "Number of Kappas in x grid are not correct." );
        for (int i=0; i < ny ; i++){
            for(int j=0; j< nx; j++){
                ElementType d;
                ss >> comma >> d ;
                EXPECTED(trim(comma) == ",");
//                tree->Level[l-1]->K_x[i+j*ny]=d;
            }
        }
    }
    /*------------------------------------------------------*/
    void Importer::import_kappa_y(){
        int l,c;
        stringstream ss,err;
        ss.str(line);
        string Level,Kappa,_y, comma;
        ss >> Level >> l >> comma >> Kappa >> _y >> c ;

        int ny = tree->Level[l-1]->K_rows;
        int nx = tree->Level[l-1]->K_cols;
        EXPECTED_MSG((c == (nx*ny)), err << "Number of Kappas in y grid are not correct." );
        for (int i=0; i < ny ; i++){
            for(int j=0; j< nx; j++){
                ElementType d;
                ss >> comma >> d ;
                EXPECTED(trim(comma) == ",");
//                tree->Level[l-1]->K_y[i+j*ny]=d;
            }
        }
    }
    /*------------------------------------------------------*/
    void Importer::import_kappa_z(){
        int l,c;
        stringstream ss,err;
        ss.str(line);
        string Level,Kappa,_z, comma;
        ss >> Level >> l >> comma >> Kappa >> _z >> c ;

        int ny = tree->Level[l-1]->K_rows;
        int nx = tree->Level[l-1]->K_cols;
        EXPECTED_MSG((c == (nx*ny)), err << "Number of Kappas in z grid are not correct." );
        for (int i=0; i < ny ; i++){
            for(int j=0; j< nx; j++){
                ElementType d;
                ss >> comma >> d ;
                EXPECTED(trim(comma) == ",");
//                tree->Level[l-1]->K_z[i+j*ny]=d;
            }
        }
    }
    /*------------------------------------------------------*/
    void Importer::import_translators_count(){}
    /*------------------------------------------------------*/
    void Importer::import_translator(){}
    /*------------------------------------------------------*/
    void Importer::import_Z_near(){
        stringstream ss;
        int b1,b2,ny,nx;
        string Box1,Box2, comma, Size;
        ss.str(line);
        ss >> Box1 >> b1 >> comma >> Box2 >> b2 >> comma >> Size >> ny >> nx ;
        Z_near z(b1,b2);
        z.set_size(ny,nx);
        for (int i=0; i < ny ; i++){
            for(int j=0; j< nx; j++){
                ElementType d;
                ss >> comma >> d ;
                EXPECTED(trim(comma) == ",");
                z.set_element(i,j,d);
            }
        }
    }
    /*------------------------------------------------------*/
    void Importer::import_receiving(){
        stringstream ss;
        int box, bf, ny, nx;
        string Box, Basis, FarField, comma;
        ss >> Box >> box >> comma >> Basis >> bf >> comma >> FarField >> ny >> nx ;
        Receiving r(box,bf);
        r.set_size(ny,nx);
        for (int i=0; i < ny ; i++){
            for(int j=0; j< nx; j++){
                ElementType d;
                ss >> comma >> d ;
                EXPECTED(trim(comma) == ",");
                r.set_element(i,j,d);
            }
        }
    }
    /*------------------------------------------------------*/
    void Importer::import_far_field(){
        stringstream ss;
        int box, bf, ny, nx;
        string Box, Basis, FarField, comma;
        ss >> Box >> box >> comma >> Basis >> bf >> comma >> FarField >> ny >> nx ;
        F_far ff(box,bf);
        ff.set_size(ny,nx);
        for (int i=0; i < ny ; i++){
            for(int j=0; j< nx; j++){
                ElementType d;
                ss >> comma >> d ;
                EXPECTED(trim(comma) == ",");
                ff.set_element(i,j,d);
            }
        }
    }
    /*------------------------------------------------------*/
    void Importer::import_I_vect(){
        int l,b,n;
        stringstream ss;
        import_level_box(ss,l,b);
        string I_vect,comma;
        ss >> I_vect >> n ;
        FMM_3D::Box * box = tree->Level[l-1]->boxes[b-1];
        box->I = new GeneralArray(n,1);
        for(int i=0;i<n;i++){
            ElementType d;
            ss >> comma >> d ;
            box->I->set_element(i, 0, d);
        }
    }
    /*------------------------------------------------------*/
    void Importer::import(){
        line_no=0;
        while ( !f.eof()){
            getline(f,line);
            line_no++;
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
            if (line.find("Kappa x") != string::npos){
                import_kappa_x();
                continue;
            }
            if (line.find("Kappa y") != string::npos){
                import_kappa_y();
                continue;
            }
            if (line.find("Kappa z") != string::npos){
                import_kappa_z();
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
             if (line.find("I_vect") != string::npos){
                import_I_vect();
                continue;
            }
            if (line.find("Z_near") != string::npos){
                import_Z_near();
                continue;
            }
            if (line.find("Receiving") != string::npos){
                import_receiving();
                continue;
            }
            if (line.find("FarField") != string::npos){
                import_far_field();
                continue;
            }
           if (line.find("Box") != string::npos){ // this test should be the last. Since all other lines have a "Box".
                import_box();
                continue;
            }
        }
    }
    /*=====================================================================*/
    void Exporter::export_boxes(int l){
        int nb = tree->Level[l-1]->boxes.size();
        f << "Level " << l << ", Boxes " << nb << endl;
        for(int b=1;b<=nb; b++){                        // Box loop
            Box *box = tree->Level[l-1]->boxes[b-1];
            f << "Level " << l << ", Box " << b <<
                ", " << box->center.x <<
                ", " << box->center.y <<
                ", " << box->center.z <<
                ", " << box->diagonal
                << endl;
            export_box_children(l,b);
            export_box_fars    (l,b);
            export_box_nears   (l,b);
            export_box_I_vect  (l,b);

        }
    }
    /*------------------------------------------------------*/
    void Exporter::export_levels(){
        f << "Levels " << L_max << endl;
        for(int l =1;l<=L_max;l++){                         // Level loop
            export_boxes(l);
            export_kappas_x(l);
            export_kappas_y(l);
            export_kappas_z(l);
        }
    }
    /*------------------------------------------------------*/
    void Exporter::export_box_children(int l,int b){
        Box *box = tree->Level[l-1]->boxes[b-1];
        int nc = box->children.size();
        f << "Level " << l << ", Box " << b << ", Children " << nc;
        for (int c=0;c<nc;c++){
            f << ", " << box->children[c]->index ;
        }
        f << endl;
    }
    /*------------------------------------------------------*/
    void Exporter::export_box_fars(int l,int b){
        Box *box = tree->Level[l-1]->boxes[b-1];
        int nf = box->ff_int_list.size();
        f << "Level " << l << ", Box " << b << ", Fars " << nf;
        for(int fi=0;fi<nf;fi++){
            f << ", " << box->ff_int_list[fi]->index ;
        }
        f << endl;
    }
    /*------------------------------------------------------*/
    void Exporter::export_box_nears(int l,int b){
        Box *box = tree->Level[l-1]->boxes[b-1];
        int nn = box->nf_int_list.size();
        f << "Level " << l << ", Box " << b << ", Nears " << nn;
        for(int ni=0;ni<nn;ni++){
            f << ", " << box->nf_int_list[ni]->index ;
        }
        f << endl;
    }
    /*------------------------------------------------------*/
    void Exporter::export_box_I_vect(int l,int b){
        Box *box = tree->Level[l-1]->boxes[b-1];
        if ( box->I == NULL )
            return;
        int ny=box->I->rows_count();
        int nx=box->I->cols_count();
        f << "Level " << l << ", Box " << b << ", I_vect " << ny*nx;
        for( int ii=0;ii<ny; ii++){
            for(int jj=0;jj<nx;jj++){
                f << ", " <<  box->I->get_element(ii,jj);
            }
        }
        f << endl;
    }
    /*------------------------------------------------------*/
    void Exporter::export_kappas_x(int l){
        int nky = tree->Level[l-1]->K_rows;
        int nkx = tree->Level[l-1]->K_cols;
        f << "Level " << l << ", Kappas " << nky*nkx << endl;
        f << "Level " << l << ", Kappa x " << nky*nkx ;
        for(int ki=0;ki<nky;ki++){
            for(int kj=0;kj<nkx;kj++){
//                f << ", " << tree->Level[l-1]->K_x[ki+kj*nky];
            }
        }
        f << endl;
    }
    /*------------------------------------------------------*/
    void Exporter::export_kappas_y(int l){
        int nky = tree->Level[l-1]->K_rows;
        int nkx = tree->Level[l-1]->K_cols;
        f << "Level " << l << ", Kappa y " << nky*nkx ;
        for(int ki=0;ki<nky;ki++){
            for(int kj=0;kj<nkx;kj++){
//                f << ", " << tree->Level[l-1]->K_y[ki+kj*nky];
            }
        }
        f << endl;
    }
    /*------------------------------------------------------*/
    void Exporter::export_kappas_z(int l){
        int nky = tree->Level[l-1]->K_rows;
        int nkx = tree->Level[l-1]->K_cols;
        f << "Level " << l << ", Kappa z " << nky*nkx ;
        for(int ki=0;ki<nky;ki++){
            for(int kj=0;kj<nkx;kj++){
//                f << ", " << tree->Level[l-1]->K_z[ki+kj*nky];
            }
        }
        f << endl;
    }
    /*------------------------------------------------------*/
    void Exporter::export_all(){
        export_levels();
    }
    /*------------------------------------------------------*/
    Exporter::Exporter(string fn){
        f.open(fn);
    }
    /*------------------------------------------------------*/
    Exporter::~Exporter(){
        f.close();
    }
    /*------------------------------------------------------*/
    void export_setup(){
        Exporter setup("export_setup.txt");
        setup.export_all();
    }
    /*------------------------------------------------------*/
}
