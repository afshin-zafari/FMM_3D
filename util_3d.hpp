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
            void import_charges          ();
            void import_kappas_count     ();
            void import_kappa            ();
            void import_translators_count();
            void import_translator       ();
            void import_I_vect           ();

    };
    void import_setup();
}
#endif // UTIL_3D_HPP
