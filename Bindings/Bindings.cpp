// bindings.cpp
//
// Author: Max Ahnen 2017
//
//---------------------------------
// included dependencies
#include "pybind11/pybind11.h"
#include "Core/Vec3.h"
#include "Core/Rot3.h"
#include "Core/Ray.h"
#include "Core/RayForPropagation.h"
#include "Core/Photon.h"
#include "Core/Frame.h"
#include "Core/Color.h"
#include "Core/Function/LimitsFunction.h"
#include "Core/Function/Func1DFunction.h"
#include "Scenery/Primitive/Triangle.h"
#include "Scenery/Primitive/Annulus.h"
#include "Scenery/Primitive/BiConvexLens.h"
#include "Scenery/Primitive/BiConvexLensHexBound.h"
#include "Scenery/Primitive/Cylinder.h"
#include "Scenery/Primitive/Disc.h"
#include "Scenery/Primitive/EllipticalCapWithHexagonalBound.h"
#include "Scenery/Primitive/HexPlane.h"
#include "Scenery/Primitive/Plane.h"
#include "Scenery/Primitive/PlaneDualSphericalBound.h"
#include "Scenery/Primitive/RectangularBox.h"
#include "Scenery/Primitive/Sphere.h"
#include "Scenery/Primitive/SphereCapWithCylinderBound.h"
#include "Scenery/Primitive/SphereCapWithHexagonalBound.h"
#include "Scenery/Primitive/SphereCapWithRectangularBound.h"
//---------------------------------

using Function::Func1D;
using Function::Limits;

PYBIND11_MODULE(mctPy, module) {
    module.doc() = "mctracer Python bindings";  // optional module docstring

    pybind11::class_<Vec3>(module, "Vec3")
        .def(pybind11::init<const double, const double, const double>(),
            pybind11::arg("x"), pybind11::arg("y"), pybind11::arg("z"))
        .def("set", &Vec3::set,
            pybind11::arg("x"), pybind11::arg("y"), pybind11::arg("z"))
        .def("__repr__", &Vec3::str);

    pybind11::class_<Ray>(module, "Ray")
        .def(pybind11::init<const Vec3, const Vec3>(),
            pybind11::arg("support"), pybind11::arg("direction"))
        .def("set_direction", &Ray::set_direction)
        .def("set_support", &Ray::set_support)
        .def("set_support_and_direction", &Ray::set_support_and_direction)
        .def("__repr__", &Ray::str);

    pybind11::class_<RayForPropagation, Ray>(module, "RayForPropagation")
        .def(pybind11::init<const Vec3, const Vec3>(),
            pybind11::arg("support"), pybind11::arg("direction"))
        .def("set_simulation_truth_id", &RayForPropagation::set_simulation_truth_id)
        .def("__repr__", &RayForPropagation::str);

    pybind11::class_<Photon, RayForPropagation>(module, "Photon")
        .def(pybind11::init<const Vec3, const Vec3, const double>(),
            pybind11::arg("support"), pybind11::arg("direction"),
            pybind11::arg("wavelength"))
        .def("__repr__", &Photon::str);

    pybind11::class_<Frame>(module, "Frame")
        .def(pybind11::init<>())
        .def("__repr__", &Frame::str);

    pybind11::class_<Rot3>(module, "Rot3")
        .def(pybind11::init<>())
        .def(pybind11::init<double, double, double>(),
            pybind11::arg("Phi"), pybind11::arg("The"), pybind11::arg("Psi"))
        .def(pybind11::init<const Vec3, const double>(),
            pybind11::arg("rot_axis"), pybind11::arg("rot_angle"))
        .def("__repr__", &Rot3::str);

    pybind11::class_<Color>(module, "Color")
        .def(pybind11::init<const int, const int, const int>(),
            pybind11::arg("_r"), pybind11::arg("_g"), pybind11::arg("_b"))
        .def("__repr__", &Color::str);

    pybind11::class_<SurfaceEntity, Frame>(module, "SurfaceEntity")
        .def(pybind11::init<const std::string, const Vec3, const Rot3>(),
            pybind11::arg("name"), pybind11::arg("pos"), pybind11::arg("rot"))
        .def("set_outer_color", &SurfaceEntity::set_outer_color,
            pybind11::arg("color"))
        .def("set_inner_color", &SurfaceEntity::set_inner_color,
            pybind11::arg("color"))/*
        .def("set_outer_reflection", &SurfaceEntity::set_outer_reflection,
            pybind11::arg("refl"))
        .def("set_inner_reflection", &SurfaceEntity::set_inner_reflection,
            pybind11::arg("refl"))
        .def("set_outer_refraction", &SurfaceEntity::set_outer_refraction,
            pybind11::arg("refrac"))
        .def("set_inner_refraction", &SurfaceEntity::set_inner_refraction,
            pybind11::arg("refrac"))
        .def("set_outer_absorption", &SurfaceEntity::set_outer_absorption,
            pybind11::arg("absorp"))
        .def("set_inner_absorption", &SurfaceEntity::set_inner_absorption,
            pybind11::arg("absorp"))*/
        .def("set_allowed_frames_to_propagate_to", &SurfaceEntity::set_allowed_frames_to_propagate_to,
            pybind11::arg("frame"))
        .def("take_boundary_layer_properties_from", &SurfaceEntity::take_boundary_layer_properties_from,
            pybind11::arg("proto"))
        .def("take_boundary_layer_properties_but_inside_out_from", 
            &SurfaceEntity::take_boundary_layer_properties_but_inside_out_from,
            pybind11::arg("proto"))
        .def("boundary_layer_is_transparent", 
            &SurfaceEntity::boundary_layer_is_transparent)
        .def("has_restrictions_on_frames_to_propagate_to", 
            &SurfaceEntity::has_restrictions_on_frames_to_propagate_to)
        .def("__repr__", &SurfaceEntity::str);

// ---------------- the primitives ------------------------

    pybind11::class_<Triangle, SurfaceEntity>(module, "Triangle")
        .def(pybind11::init<>())
        .def("set_corners_in_xy_plane", &Triangle::set_corners_in_xy_plane,
            pybind11::arg("Ax"), pybind11::arg("Ay"), pybind11::arg("Bx"),
            pybind11::arg("By"), pybind11::arg("Cx"), pybind11::arg("Cy"))
        .def("set_normal_and_3_vertecies",
            &Triangle::set_normal_and_3_vertecies,
            pybind11::arg("nomal"), pybind11::arg("a"),
            pybind11::arg("b"), pybind11::arg("c"))
        .def("__repr__", &Triangle::str);

    pybind11::class_<Annulus, SurfaceEntity>(module, "Annulus")
        .def(pybind11::init<>())
        .def("set_outer_inner_radius", &Annulus::set_outer_inner_radius,
            pybind11::arg("outer_radius"), pybind11::arg("inner_radius"))
        .def("__repr__", &Annulus::str);

    pybind11::class_<BiConvexLens, SurfaceEntity>(module, "BiConvexLens")
        .def(pybind11::init<>())
        .def("set_curvature_radius_and_aperture_radius",
            &BiConvexLens::set_curvature_radius_and_aperture_radius,
            pybind11::arg("curvature_radius"), pybind11::arg("aperture_radius"))
        .def("__repr__", &BiConvexLens::str);

    pybind11::class_<BiConvexLensHexBound, SurfaceEntity>(module, "BiConvexLensHexBound")
        .def(pybind11::init<>())
        .def("set_curvature_radius_and_outer_hex_radius",
            &BiConvexLensHexBound::set_curvature_radius_and_outer_hex_radius,
            pybind11::arg("curvature_radius"), pybind11::arg("outer_aperture_radius"))
        .def("__repr__", &BiConvexLensHexBound::str);

    pybind11::class_<Cylinder, SurfaceEntity>(module, "Cylinder")
        .def(pybind11::init<const std::string, const Vec3, const Rot3>(),
            pybind11::arg("name"), pybind11::arg("pos"), pybind11::arg("rot"))
        .def("set_cylinder", &Cylinder::set_cylinder,
            pybind11::arg("radius"), pybind11::arg("start_pos"), pybind11::arg("end_pos"))
        .def("set_radius_and_length", &Cylinder::set_radius_and_length,
            pybind11::arg("radius"), pybind11::arg("length"))
        .def("__repr__", &Cylinder::str);

    pybind11::class_<Disc, SurfaceEntity>(module, "Disc")
        .def(pybind11::init<>())
        .def("set_radius", &Disc::set_radius, pybind11::arg("radius"))
        .def("__repr__", &Disc::str);

    pybind11::class_<EllipticalCapWithHexagonalBound, SurfaceEntity>(module, "EllipticalCapWithHexagonalBound")
        .def(pybind11::init<>())
        .def("set_focalLength_radiiRatio_hexAngel_hexRadius",
            &EllipticalCapWithHexagonalBound::set_focalLength_radiiRatio_hexAngel_hexRadius,
            pybind11::arg("focal_length"), pybind11::arg("ratio_x_to_y_curvature_radius"),
            pybind11::arg("hex_bound_rotation"), pybind11::arg("outer_hex_radius"))
        .def("__repr__", &EllipticalCapWithHexagonalBound::str);

    pybind11::class_<HexPlane, SurfaceEntity>(module, "HexPlane")
        .def(pybind11::init<>())
        .def("set_outer_hex_radius", &HexPlane::set_outer_hex_radius,
            pybind11::arg("outer_hex_radius"))
        .def("__repr__", &HexPlane::str);

    pybind11::class_<Plane, SurfaceEntity>(module, "Plane")
        .def(pybind11::init<const std::string, const Vec3, const Rot3>(),
            pybind11::arg("name"), pybind11::arg("pos"), pybind11::arg("rot"))
        .def("set_x_y_width", &Plane::set_x_y_width,
            pybind11::arg("x_width"), pybind11::arg("y_width"))
        .def("__repr__", &Plane::str);

    pybind11::class_<PlaneDualSphericalBound, SurfaceEntity>(module, "PlaneDualSphericalBound")
        .def(pybind11::init<>())
        .def("set_x_hight_and_y_width",
            &PlaneDualSphericalBound::set_x_hight_and_y_width,
            pybind11::arg("x_width"), pybind11::arg("y_width"))
        .def("__repr__", &PlaneDualSphericalBound::str);

    pybind11::class_<RectangularBox, SurfaceEntity>(module, "RectangularBox")
        .def(pybind11::init<>())
        .def("set_xyz_width",
            &RectangularBox::set_xyz_width,
            pybind11::arg("x"), pybind11::arg("y"), pybind11::arg("z"))
        .def("__repr__", &RectangularBox::str);

    pybind11::class_<Sphere, SurfaceEntity>(module, "Sphere")
        .def(pybind11::init<const std::string, const Vec3, const Rot3>(),
            pybind11::arg("name"), pybind11::arg("pos"), pybind11::arg("rot"))
        .def("set_radius", &Sphere::set_radius, pybind11::arg("radius"))
        .def("__repr__", &Sphere::str);

    pybind11::class_<SphereCapWithCylinderBound, SurfaceEntity>(module, "SphereCapWithCylinderBound")
        .def(pybind11::init<>())
        .def("set_curvature_radius_and_outer_radius", 
            &SphereCapWithCylinderBound::set_curvature_radius_and_outer_radius,
            pybind11::arg("curvature_radius"), pybind11::arg("cap_radius"))
        .def("__repr__", &SphereCapWithCylinderBound::str);

    pybind11::class_<SphereCapWithHexagonalBound, SurfaceEntity>(module, "SphereCapWithHexagonalBound")
        .def(pybind11::init<>())
        .def("set_curvature_radius_and_outer_hex_radius", 
            &SphereCapWithHexagonalBound::set_curvature_radius_and_outer_hex_radius,
            pybind11::arg("curvature_radius"), pybind11::arg("outer_hex_radius"))
        .def("__repr__", &SphereCapWithHexagonalBound::str);

    pybind11::class_<SphereCapWithRectangularBound, SurfaceEntity>(module, "SphereCapWithRectangularBound")
        .def(pybind11::init<>())
        .def("set_curvature_radius_and_x_y_width", 
            &SphereCapWithRectangularBound::set_curvature_radius_and_x_y_width,
            pybind11::arg("_curvature_radius"), pybind11::arg("x_width"), pybind11::arg("y_width"))
        .def("__repr__", &SphereCapWithRectangularBound::str);

// --------------------- primitives end ------------------------

}

