// Copyright 2015 Sebastian A. Mueller
#include <sstream>
#include <string>
#include "merlict/tests/catch.hpp"
#include "merlict/merlict.h"
#include "merlict_visual/visual.h"
#include "merlict_signal_processing/signal_processing.h"
namespace ml = merlict;


template<typename T>
std::string print_size(const std::string &name, T s) {
    std::stringstream out;
    out << ml::txt::fill_up_text_with_whitespaces_until_column(name, 36);
    out << sizeof(s) << '\n';
    return out.str();
}


TEST_CASE("SizeOfTest: sizes_of", "[merlict]") {
    std::stringstream out;
    out << "struct/name                size/bytes\n";
    out << "--------------------------------------" << "\n";

    ml::Color col(123, 65, 12);
    out << print_size("Color", col);

    ml::Vec2 v2;
    out << print_size("Vec2", v2);

    ml::Vec3 v3;
    out << print_size("Vec3", v3);

    ml::Rot3 r;
    out << print_size("Rot3", r);

    ml::HomTra3 h;
    out << print_size("HomTra3", h);
    out << "\n";

    ml::Ray ray(ml::VEC3_ORIGIN, ml::VEC3_UNIT_Z);
    out << print_size("Ray", ray);

    ml::visual::CameraRay camray(ml::VEC3_ORIGIN, ml::VEC3_UNIT_Z);
    out << print_size("CameraRay", camray);

    ml::RayForPropagation rayfp(ml::VEC3_ORIGIN, ml::VEC3_UNIT_Z);
    out << print_size("RayForPropagation", rayfp);

    ml::Photon ph(ml::VEC3_ORIGIN, ml::VEC3_UNIT_Z, 433e-9);
    out << print_size("Photon", ph);
    out << "\n";

    ml::Intersection is;
    out << print_size("Intersection", is);

    ml::PropagationEnvironment environment;
    out << print_size("PropagationEnvironment", environment);
    out << "\n";

    ml::Frame tree;
    tree.set_name_pos_rot("0123456789", ml::VEC3_ORIGIN, ml::ROT3_UNITY);
    out << print_size("Frame", tree);

    ml::SurfaceEntity surf("0123456789", ml::VEC3_ORIGIN, ml::ROT3_UNITY);
    out << print_size("SurfaceEntity", surf);

    ml::Annulus annulus;
    out << print_size("Annulus", annulus);

    ml::BiConvexLens biConvexLens;
    out << print_size("BiConvexLens", biConvexLens);

    ml::BiConvexLensHexBound biConvexLensHexBound;
    out << print_size("BiConvexLensHexBound", biConvexLensHexBound);

    ml::Cylinder cylinder;
    out << print_size("Cylinder", cylinder);

    ml::Disc disc;
    out << print_size("Disc", disc);

    ml::EllipticalCapWithHexagonalBound ellipticalCapWithHexagonalBound;
    out << print_size(
        "EllipticalCapWithHexagonalBound", ellipticalCapWithHexagonalBound);

    ml::HexPlane hexPlane;
    out << print_size("HexPlane", hexPlane);

    ml::Plane plane;
    out << print_size("Plane", plane);

    ml::PlaneDualSphericalBound planeDualSphericalBound;
    out << print_size("PlaneDualSphericalBound", planeDualSphericalBound);

    ml::Sphere sphere;
    out << print_size("Sphere", sphere);

    ml::SphereCapWithCylinderBound sphereCapWithCylinderBound;
    out << print_size(
        "SphereCapWithCylinderBound",
        sphereCapWithCylinderBound);

    ml::SphereCapWithHexagonalBound sphereCapWithHexagonalBound;
    out << print_size(
        "SphereCapWithHexagonalBound",
        sphereCapWithHexagonalBound);

    ml::Triangle triangle;
    out << print_size("Triangle", triangle);
    out << "\n";

    ml::sensor::PhotonArrival arr_info;
    out << print_size("PhotonArrival", arr_info);

    ml::sensor::Sensor sensor(0u, &tree);
    out << print_size("Sensor", sensor);

    std::vector<ml::sensor::Sensor*> raw_sensors;
    ml::sensor::Sensors sensors(raw_sensors);
    out << print_size("Sensors", sensors);

    ml::sensor::FindSensorByFrame finder(&tree, &sensors.by_frame);
    out << print_size("FindSensorByFrame", finder);
    out << "\n";

    signal_processing::PipelinePhoton pipe_ph;
    out << print_size("PipelinePhoton", pipe_ph);

    ml::write_text_to_file(out.str(), "merlict_tests/sizeof.txt");
}
