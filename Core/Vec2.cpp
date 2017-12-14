// Copyright 2014 Sebastian A. Mueller
#include "Core/Vec2.h"
#include <math.h>
#include <sstream>


const Vec2 Vec2::ORIGIN   = Vec2(0.0, 0.0);
const Vec2 Vec2::UNIT_X = Vec2(1.0, 0.0);
const Vec2 Vec2::UNIT_Y = Vec2(0.0, 1.0);
const double Vec2::MAX_DEVIATION_EQUAL_VEC2 = 1e-7;

Vec2::Vec2() {}

Vec2::Vec2(const double _x, const double _y) {
    set(_x, _y);
}

void Vec2::set(const double _x, const double _y) {
    x = _x;
    y = _y;
}

void Vec2::normalize() {
    *this = *this/this->norm();
}

double Vec2::norm()const {
    return sqrt(x*x + y*y);
}

double Vec2::operator*(const Vec2 v)const {
    return v.x*x + v.y*y;
}

Vec2 Vec2::operator*(const double scalar)const {
    return Vec2(x*scalar, y*scalar);
}

Vec2 Vec2::operator-(const Vec2 v)const {
    return Vec2(x-v.x, y-v.y);
}

Vec2 Vec2::operator+(const Vec2 v)const {
    return Vec2(x+v.x, y+v.y);
}

Vec2 Vec2::operator/(const double scalar)const {
    return Vec2(x/scalar, y/scalar);
}

void Vec2::operator=(const Vec2 eq) {
    x = eq.x;
    y = eq.y;
}

std::string Vec2::str()const {
    std::stringstream out;
    out << "(" << x << " " << y << ")m";
    return out.str();
}

double Vec2::angle_in_between(const Vec2& that)const {
    Vec2 this_normalized = *this/this->norm();
    Vec2 that_normalized = that/that.norm();
    return acos(this_normalized*that_normalized);
}

bool Vec2::operator == (const Vec2& eq_v) const {
    return distance_to(eq_v) <= MAX_DEVIATION_EQUAL_VEC2;
}

bool Vec2::operator != (const Vec2& eq_v) const {
    return distance_to(eq_v) > MAX_DEVIATION_EQUAL_VEC2;
}

double Vec2::distance_to(const Vec2 &v)const {
    return (*this-v).norm();
}

bool Vec2::norm_is_less_equal_than(const double length_to_compare)const {
    // avoid the sqrt for speed up
    return (*this)*(*this) <= length_to_compare*length_to_compare;
}

unsigned int Vec2::get_quadrant()const {
    // encodes the octant sectors where the vector is pointing to
    // x y sector
    // - -   0
    // - +   1
    // + -   2
    // + +   3
    const bool sx = x > 0.0;
    const bool sy = y > 0.0;
    return 2*sx + 1*sy;
}
