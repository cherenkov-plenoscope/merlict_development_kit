// Copyright 2014 Sebastian A. Mueller
#ifndef MCTRACER_CORE_VEC2_H_
#define MCTRACER_CORE_VEC2_H_

#include <string>

struct Vec2 {
    double x;
    double y;

    Vec2();
    Vec2(const double x, const double y);
    void set(const double x, const double y);
    unsigned int get_quadrant()const;
    double norm()const;
    void normalize();
    double angle_in_between(const Vec2& vec)const;
    double operator*(const Vec2 v)const;
    Vec2 operator*(const double scalar)const;
    Vec2 operator-(const Vec2 v)const;
    Vec2 operator+(const Vec2 v)const;
    Vec2 operator/(const double scalar)const;
    void operator=(const Vec2 eq);
    bool operator == (const Vec2& eq_v)const;
    bool operator != (const Vec2& eq_v)const;
    double distance_to(const Vec2 &v)const;
    bool norm_is_less_equal_than(const double length_to_compare)const;
    std::string str()const;

    static const double MAX_DEVIATION_EQUAL_VEC2;
    static const Vec2 ORIGIN;
    static const Vec2 UNIT_X;
    static const Vec2 UNIT_Y;
};
#endif  // MCTRACER_CORE_VEC2_H_
