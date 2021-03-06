// Copyright 2014 Sebastian A. Mueller
#include "merlict/scenery/geometry/HexagonalPrismZ.h"
#include <math.h>
#include <sstream>

namespace merlict {

void HexagonalPrismZ::set_outer_radius(const double outer_radius) {
    assert_outer_radius_positive(outer_radius);
    inner_radius = outer_radius * cos(M_PI/6.0);
    this->outer_radius = outer_radius;
}

bool HexagonalPrismZ::is_inside(const Vec3* vec)const {
    const double projection_onto_UNIT_U = VEC3_UNIT_U * *vec;
    const double projection_onto_UNIT_V = VEC3_UNIT_V * *vec;
    const double projection_onto_UNIT_W = VEC3_UNIT_W * *vec;
    return (
        projection_onto_UNIT_U < inner_radius
        &&
        projection_onto_UNIT_U > -inner_radius
        &&
        projection_onto_UNIT_V < inner_radius
        &&
        projection_onto_UNIT_V > -inner_radius
        &&
        projection_onto_UNIT_W < inner_radius
        &&
        projection_onto_UNIT_W > -inner_radius);
}

void HexagonalPrismZ::assert_outer_radius_positive(
    const double outer_radius
)const {
    if (outer_radius <= 0.0) {
        std::stringstream info;
        info << "HexagonalPrismZ::" << __func__ << "()\n";
        info << "The outer radius must not be smaller or equal zero! ";
        info << "Expected: outer_radius > 0m, but actual: outer radius = ";
        info << outer_radius  << "m\n";
        throw std::invalid_argument(info.str());
    }
}

double HexagonalPrismZ::get_area()const {
    return outer_radius*outer_radius * 3.0/2.0 * sqrt(3.0);
}

double HexagonalPrismZ::get_outer_radius()const {
    return outer_radius;
}

}  // namespace merlict
