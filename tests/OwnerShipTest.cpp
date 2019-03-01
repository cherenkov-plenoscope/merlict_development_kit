// Copyright 2014 Sebastian A. Mueller
#include <vector>
#include <string>
#include "catch.hpp"
#include "merlict/merlict.h"
namespace ml = merlict;


TEST_CASE("OwnerShipTest: own", "[merlict]") {
    ml::Frame tree;
    tree.set_name_pos_rot("tree", ml::VEC3_ORIGIN, ml::ROT3_UNITY);

    ml::Sphere* sphere = tree.append<ml::Sphere>();
    sphere->set_name_pos_rot("leaf_ball", ml::Vec3(0, 0, 1.8), ml::ROT3_UNITY);
    sphere->set_radius(1);
    sphere->set_outer_color(&ml::COLOR_GREEN);

    ml::Frame* pole = tree.append<ml::Frame>();
    pole->set_name_pos_rot("pole", ml::Vec3(0, 0, 0.5), ml::ROT3_UNITY);

    ml::Cylinder* pole1 = pole->append<ml::Cylinder>();
    pole1->set_name_pos_rot("pole1", ml::Vec3(0, 0, 0.0), ml::ROT3_UNITY);
    pole1->set_radius_and_length(0.25, 1.0);
    pole1->set_outer_color(&ml::COLOR_RED);

    ml::Cylinder* fork = pole->append<ml::Cylinder>();
    fork->set_name_pos_rot("fork", ml::Vec3(0, 0, 0), ml::ROT3_UNITY);
    fork->set_cylinder(0.1, ml::VEC3_ORIGIN, ml::Vec3(0, 0.5, 0.5));
    fork->set_outer_color(&ml::COLOR_RED);

    ml::Disc* ground = tree.append<ml::Disc>();
    ground->set_name_pos_rot("ground", ml::Vec3(0, 0, 0), ml::ROT3_UNITY);
    ground->set_radius(3.0);
    ground->set_outer_color(&ml::COLOR_GRASS_GREEN);

    tree.init_tree_based_on_mother_child_relations();
}
