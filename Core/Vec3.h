// Copyright 2014 Sebastian A. Mueller
#ifndef MCTRACER_CORE_VEC3_H_
#define MCTRACER_CORE_VEC3_H_

#include <string>

struct Vec3 {
    double x;
    double y;
    double z;

    Vec3();
    Vec3(const double x, const double y, const double z);
    void set(const double x, const double y, const double z);
    unsigned int get_octant()const;
    Vec3 get_projection_on_XZ_plane()const;
    Vec3 get_projection_on_YZ_plane()const;
    Vec3 get_projection_on_XY_plane()const;
    double norm()const;
    void normalize();
    Vec3 cross(const Vec3 v)const;
    void mirror(Vec3* ray)const;
    double angle_in_between(const Vec3& vec)const;
    double operator*(const Vec3 v)const;
    Vec3 operator*(const double scalar)const;
    Vec3 operator-(const Vec3 v)const;
    Vec3 operator+(const Vec3 v)const;
    Vec3 operator/(const double scalar)const;
    void operator=(const Vec3 eq);
    bool operator == (const Vec3& eqVec)const;
    bool operator != (const Vec3& eqVec)const;
    double distance_to(const Vec3 &v)const;
    double squared_distance_to(const Vec3 &v)const;
    bool is_paralell_to_z_axis()const;
    bool is_parallel_to_x_y_plane()const;
    bool norm_is_less_equal_than(const double length_to_compare)const;
    std::string str()const;

    static const double MAX_DEVIATION_EQUAL_VEC3_SQUARE;
    static const Vec3 ORIGIN;
    static const Vec3 UNIT_X;
    static const Vec3 UNIT_Y;
    static const Vec3 UNIT_Z;
};
#endif  // MCTRACER_CORE_VEC3_H_
