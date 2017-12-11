// bindings.cpp
//
// Author: Max Ahnen 2017
//
//---------------------------------
// included dependencies
#include "pybind11/pybind11.h"
#include "Core/Vec3.h"

//---------------------------------
// Test
int add(int i, int j) {
    return i + j;
}

int subtract(int i, int j) {
    return i - j;
}

//---------------------------------

PYBIND11_MODULE(mctPy, module) {
    module.doc() = "pybind11 example plugin";  // optional module docstring

    module.def(
        "add", &add, "A function which adds two numbers",
        pybind11::arg("i"), pybind11::arg("j"));

    module.def(
        "subtract", &subtract, "A function which subtracts two numbers",
        pybind11::arg("i"), pybind11::arg("j"));

    pybind11::class_<Vec3>(module, "Vec3")
        .def(pybind11::init<const double, const double, const double>())
        .def("set", &Vec3::set)
        .def("str", &Vec3::str);
}
