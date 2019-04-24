// Copyright 2014 Sebastian A. Mueller
#include <math.h>
#include <sstream>
#include <vector>
#include "merlict/Ray.h"


namespace merlict {

Ray::Ray():
    support_(0., 0., 0.),
    direction_(0., 0., 1.)
{}

Ray::Ray(const Vec3 support, const Vec3 direction) {
    set_support_and_direction(support, direction);
}

void Ray::set_support_and_direction(const Vec3 support, const Vec3 direction) {
    support_ = support;
    direction_ = direction;
    direction_.normalize();
}

std::string Ray::str()const {
    std::stringstream out;
    out << "support: " << support_.str() << ", direction: " << direction_.str();
    return out.str();
}

Vec3 Ray::position_at(const double scalar)const {
    return support_ + direction_*scalar;
}

Vec3 Ray::support()const {
    return support_;
}

Vec3 Ray::direction()const {
    return direction_;
}

void Ray::transform(const HomTra3 *T) {
    support_ = T->position(support_);
    direction_ = T->orientation(direction_);
}

void Ray::transform_inverse(const HomTra3 *T) {
    support_ = T->position_inverse(support_);
    direction_ = T->orientation_inverse(direction_);
}

double Ray::parameter_for_closest_distance_to_point(const Vec3 &point)const {
    // We create a plane orthogonal to this ray and containing the point
    // plane equation:
    //  d = x*a + y*b + z*c
    //
    // We set the normal vector n of the plane to the ray's direction vector:
    //  a=direction.x b=direction.y c=direction.z
    //
    // Now we insert the support vector of the frame into the plane eqaution:
    //  d = point.x*dirx + point.y*diry + point.z*dirz
    const double d = direction_*point;

    // Insert the ray into plane equation and solve for the ray parameter.
    // The ray's direction is normalized, therefore: (direction * direction)=1
    return d - support_*direction_;
}

double Ray::closest_distance_to_point(const Vec3 &point)const {
    const double a = parameter_for_closest_distance_to_point(point);
    const Vec3 q = position_at(a);
    const Vec3 shortest_connection = point - q;
    return shortest_connection.norm();
}

}  // namespace merlict
