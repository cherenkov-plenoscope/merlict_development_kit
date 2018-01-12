// Copyright 2014 Sebastian A. Mueller
#ifndef SCENERY_PRIMITIVE_HEXPLANE_H_
#define SCENERY_PRIMITIVE_HEXPLANE_H_

#include <vector>
#include <string>
#include "Core/SurfaceEntity.h"
#include "Scenery/Geometry/XyPlaneRayIntersectionEquation.h"
#include "Scenery/Geometry/HexagonalPrismZ.h"

class HexPlane :public SurfaceEntity{
 protected:
    HexagonalPrismZ hex_bounds;

 public:
    void set_outer_hex_radius(const double outer_hex_radius);
    std::string str()const;
    void calculate_intersection_with(
        const Ray* ray,
        std::vector<Intersection> *intersections)const;
 private:
    void post_initialize_radius_of_enclosing_sphere();
};

#endif  // SCENERY_PRIMITIVE_HEXPLANE_H_
