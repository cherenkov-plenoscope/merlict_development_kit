// Copyright 2014 Sebastian A. Mueller
#ifndef SCENERY_PRIMITIVE_SPHERECAPWITHCYLINDERBOUND_H_
#define SCENERY_PRIMITIVE_SPHERECAPWITHCYLINDERBOUND_H_

#include <vector>
#include <string>
#include "merlict/scenery/geometry/SphericalCapRayIntersectionEquation.h"
#include "merlict/scenery/geometry/CylinderPrismZ.h"
#include "merlict/scenery/geometry/SurfaceWithOuterPrismBound.h"

namespace merlict {

class SphereCapWithCylinderBound :public SurfaceWithOuterPrismBound {
 protected:
    CylinderPrismZ CylBounds;
    double curvature_radius;
    double cap_radius;

 public:
    void set_curvature_radius_and_outer_radius(
        const double curvature_radius,
        const double cap_radius);
    std::string str()const;
    void calculate_intersection_with(
        const Ray* ray,
        std::vector<Intersection> *intersections)const;
    double get_focal_length()const;

 private:
    void restrict_cap_radius_to_curvature_radius();
    void post_initialize_radius_of_enclosing_sphere();
};

}  // namespace merlict

#endif  // SCENERY_PRIMITIVE_SPHERECAPWITHCYLINDERBOUND_H_
