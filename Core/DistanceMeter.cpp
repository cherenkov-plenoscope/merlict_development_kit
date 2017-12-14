// Copyright 2014 Sebastian A. Mueller
#include <limits>
#include <algorithm>
#include "Core/DistanceMeter.h"
#include "Core/RayAndFrame.h"


DistanceMeter::DistanceMeter(const Ray* ray, const Frame* world) {
    faces_an_object = false;
    distance_to_closest_object = std::numeric_limits<double>::max();

    const Intersection intersec = RayAndFrame::first_intersection(ray, world);

    if (intersec.does_intersect()) {
        faces_an_object = true;
        distance_to_closest_object = intersec.distance_to_ray_support();
    }
}
