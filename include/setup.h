#ifndef SETUP_H
#define SETUP_H
#include <vector>
#include <cstdio>
#include <string>
#include "types_3d.hpp"
namespace FMM_3D{
    /*---------------------------------------------------------------*/
    void import_boxes_binary    (const string fn);
    void import_translators     (const string fn);
    void import_interpolators   (const string fn);
    void import_kappa           (const string fn);
    void import_Z               (const string fn);
    void import_I               (const string fn);
    void import_F               (const string fn);
    void import_R               (const string fn);
    void create_V               ();
    void create_Far_fields      ();
    void create_Exponential     ();
    void create_I               ();
    void extend_F               (F_far_tilde *F);
}
#endif // SETUP_H

