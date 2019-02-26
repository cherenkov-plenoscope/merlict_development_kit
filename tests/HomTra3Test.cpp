// Copyright 2014 Sebastian A. Mueller
#include <math.h>
#include "catch.hpp"
#include "merlict/HomTra3.h"

using namespace merlict;



TEST_CASE("HomTra3Test: default_is_unit_trafo", "[merlict]") {
    HomTra3 trafo;
    HomTra3 unit_frafo;
    unit_frafo.set_transformation(
        Vec3(1, 0, 0),
        Vec3(0, 1, 0),
        Vec3(0, 0, 1), Vec3(0, 0, 0));
    CHECK(trafo == unit_frafo);
}

TEST_CASE("HomTra3Test: unit_trafo_has_no_translation", "[merlict]") {
    HomTra3 trafo;
    // translation should be zero
    CHECK(trafo.get_translation() == VEC3_ORIGIN);
}

TEST_CASE("HomTra3Test: unit_trafo_has_no_rotation", "[merlict]") {
    HomTra3 trafo;
    // and rotation should be unity as well
    Vec3 unit_x(1.0, 0.0, 0.0);
    CHECK(trafo.get_transformed_orientation(unit_x) == unit_x);
    Vec3 unit_y(0.0, 1.0, 0.0);
    CHECK(trafo.get_transformed_orientation(unit_y) == unit_y);
    Vec3 unit_z(0.0, 0.0, 1.0);
    CHECK(trafo.get_transformed_orientation(unit_z) == unit_z);
}

TEST_CASE("HomTra3Test: unit_trafo_is_its_own_inverse", "[merlict]") {
    HomTra3 trafo;
    HomTra3 inverse_trafo = trafo;
    inverse_trafo.inverse();
    CHECK(trafo == inverse_trafo);
}

TEST_CASE("HomTra3Test: translation_without_rotation", "[merlict]") {
    Vec3 translation(0.0, 0.0, 133.7);
    Rot3 rotation(0.0, 0.0, 0.0);
    HomTra3 frame1_to_frame2;
    frame1_to_frame2.set_transformation(rotation, translation);
    Vec3 my_vector_in_frame1(3.141, 4.2, 0.0);
    Vec3 my_vector_in_frame2(3.141, 4.2, 133.7);
    CHECK(frame1_to_frame2.get_transformed_position(my_vector_in_frame1) == my_vector_in_frame2);
    // and also the other/inverse way round
    HomTra3 frame2_to_frame1 = frame1_to_frame2.inverse();
    CHECK(frame2_to_frame1.get_transformed_position(my_vector_in_frame2) == my_vector_in_frame1);
}

TEST_CASE("HomTra3Test: trans_pos_forth_and_back_only_translation_component_set", "[merlict]") {
    Vec3 t(0.0, 0.0, 133.7);
    Rot3 r(0.0, 0.0, 0.0);
    HomTra3 homtra;
    homtra.set_transformation(r, t);
    Vec3 ux_original = VEC3_UNIT_X;
    Vec3 ux_forth = homtra.get_transformed_position(ux_original);
    CHECK(1.0 == ux_forth.x);
    CHECK(0.0 == ux_forth.y);
    CHECK(133.7 == ux_forth.z);
    Vec3 ux_back = homtra.get_transformed_position_inverse(ux_forth);
    CHECK(ux_original.x == ux_back.x);
    CHECK(ux_original.y == ux_back.y);
    CHECK(ux_original.z == ux_back.z);
}

TEST_CASE("HomTra3Test: transform_position_forth_and_back_full_set", "[merlict]") {
    Vec3 t(0.0, 0.0, 133.7);
    Rot3 r(0.0, M_PI, 0.0);
    HomTra3 homtra;
    homtra.set_transformation(r, t);
    Vec3 ux_original = VEC3_UNIT_X;
    Vec3 ux_forth = homtra.get_transformed_position(ux_original);
    CHECK(-1.0 == Approx(ux_forth.x).margin(1e-12));
    CHECK(0.0 == Approx(ux_forth.y).margin(1e-12));
    CHECK(133.7 == Approx(ux_forth.z).margin(1e-12));
    Vec3 ux_back = homtra.get_transformed_position_inverse(ux_forth);
    CHECK(ux_original.x == ux_back.x);
    CHECK(ux_original.y == ux_back.y);
    CHECK(ux_original.z == ux_back.z);
}

TEST_CASE("HomTra3Test: trans_orientation_forth_and_back_only_rot_component_set", "[merlict]") {
    Vec3 t(0.0, 0.0, 0.0);
    Rot3 r(0.0, M_PI, 0.0);
    HomTra3 homtra;
    homtra.set_transformation(r, t);
    Vec3 ux_original = VEC3_UNIT_X;
    Vec3 ux_forth = homtra.get_transformed_orientation(ux_original);
    CHECK(-1.0 == Approx(ux_forth.x).margin(1e-12));
    CHECK(0.0 == Approx(ux_forth.y).margin(1e-12));
    CHECK(0.0 == Approx(ux_forth.z).margin(1e-12));
    Vec3 ux_back = homtra.get_transformed_orientation_inverse(ux_forth);
    CHECK(ux_original.x == ux_back.x);
    CHECK(ux_original.y == ux_back.y);
    CHECK(ux_original.z == ux_back.z);
}