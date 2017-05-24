#ifndef UTIL_3D_HPP
#define UTIL_3D_HPP
#include <fstream>
#include <sstream>
#include <cstdlib>
#include "types_3d.hpp"
using namespace std;
namespace FMM_3D{
    class Importer{
        private:
            ifstream f;
            string file_name,line;
            int line_no;;
            string::size_type pos;

        public:
            Importer(string fn);
            ~Importer();
            void import();
            void import_levels_count     ();
            int  import_boxes_count      ();
            void import_box              ();
            void import_near_boxes       ();
            void import_far_boxes        ();
            void import_child_boxes      ();
            void import_kappas_count     ();
            void import_kappa_x          ();
            void import_kappa_y          ();
            void import_kappa_z          ();
            void import_translators_count();
            void import_translator       ();
            void import_I_vect           ();
            void import_Z_near           ();
            void import_receiving        ();
            void import_far_field        ();
            void import_level_box(stringstream &,int &,int &);

    };
    /*-----------------------------------------------------------*/
    class Exporter{
         ofstream f;
    public:
        Exporter(string fn);
        ~Exporter();
        void export_levels      ();
        void export_boxes       (int l);
        void export_box_children(int l, int b);
        void export_box_nears   (int l, int b);
        void export_box_fars    (int l, int b);
        void export_box_I_vect  (int l, int b);
        void export_kappas_x    (int l);
        void export_kappas_y    (int l);
        void export_kappas_z    (int l);
        void export_all         ();

    };
    /*-----------------------------------------------------------*/
    void import_setup();
    void export_setup();
}
#endif // UTIL_3D_HPP
