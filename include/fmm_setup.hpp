#ifndef FMM_SETUP_HPP
#define FMM_SETUP_HPP
#include <vector>
#include <cstdio>
#include <string>
#include "fmm_database.hpp"
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
#endif // FMM_SETUP_HPP
