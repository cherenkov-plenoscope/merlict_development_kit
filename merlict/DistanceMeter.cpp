// Copyright 2014 Sebastian A. Mueller
#include <limits>
#include "merlict/DistanceMeter.h"
#include "merlict/RayAndFrame.h"

namespace merlict {

DistanceMeter::DistanceMeter(const Ray* ray, const Frame* world) {
    faces_an_object = false;
    distance_to_closest_object = std::numeric_limits<double>::max();
    const Intersection intersec = rays_first_intersection_with_frame(
        ray,
        world);
    if (intersec.does_intersect()) {
        faces_an_object = true;
        distance_to_closest_object = intersec.distance_to_ray_support();
    }
}

}  // namespace merlict
