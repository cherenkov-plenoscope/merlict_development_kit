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
#include "Scenery/Primitive/Triangle.h"

//---------------------------------

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
            pybind11::arg("new_rot_axis"), pybind11::arg("new_rot_angle"))
        .def("__repr__", &Rot3::str);

    pybind11::class_<SurfaceEntity, Frame>(module, "SurfaceEntity")
        .def(pybind11::init<const std::string, const Vec3, const Rot3>(),
            pybind11::arg("new_name"), pybind11::arg("new_pos"),
            pybind11::arg("new_rot"))
        .def("__repr__", &SurfaceEntity::str);

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
}
