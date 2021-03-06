// Copyright 2014 Sebastian A. Mueller
#include "merlict/scenery/primitive/PlaneDualSphericalBound.h"
#include <sstream>


namespace merlict {

void PlaneDualSphericalBound::set_x_hight_and_y_width(
    const double x_width,
    const double y_width
) {
    dual_sphere_bounds.set_x_hight_and_y_width(x_width, y_width);
    post_initialize_radius_of_enclosing_sphere();
}

void PlaneDualSphericalBound::post_initialize_radius_of_enclosing_sphere() {
    bounding_sphere_radius =
        dual_sphere_bounds.get_enclosing_radius();
}

std::string PlaneDualSphericalBound::str()const {
    std::stringstream out;
    out << SurfaceEntity::str();
    out << "plane dual circle bound:\n";
    out << "| hight: " << dual_sphere_bounds.get_hight() << "m\n";
    out << "| width: " << dual_sphere_bounds.get_width() << "m\n";
    out << "| circle radius: ";
    out << dual_sphere_bounds.get_circle_radius() << "m\n";
    out << "| area: " << dual_sphere_bounds.get_area() << "m^2\n";
    return out.str();
}

#include "merlict/Ray.h"
#include "merlict/Intersection.h"

void PlaneDualSphericalBound::calculate_intersection_with(
    const Ray* ray,
    std::vector<Intersection> *intersections
)const {
    XyPlaneRayIntersectionEquation xyPlaneRayEquation(ray);
    if (xyPlaneRayEquation.has_causal_solution()) {
        double v = xyPlaneRayEquation.get_ray_parameter_for_intersection();
        Vec3 intersection_vector = ray->position_at(v);
        if (dual_sphere_bounds.is_inside(&intersection_vector)) {
            if (ray->support() != intersection_vector) {
                intersections->emplace_back(
                    this,
                    intersection_vector,
                    xyPlaneRayEquation.get_plane_normal_vector(),
                    v,
                    ray->direction());
            }
        }
    }
}

}  // namespace merlict
