// Copyright 2014 Sebastian A. Mueller
#ifndef MCTRACER_CORE_INTERSECTION_H_
#define MCTRACER_CORE_INTERSECTION_H_

class Ray;

#include <string>
#include "Core/Vec3.h"
#include "Core/SurfaceEntity.h"

class Intersection {
 protected:
    Vec3 position;
    Vec3 surface_normal;
    const SurfaceEntity* object;
    double distance_of_ray;
    bool _from_outside_to_inside;
    bool ray_is_running_from_outside_to_inside(
        const Vec3& incident_in_obj_sys)const;

 public:
    Intersection();
    Intersection(
        const SurfaceEntity* intersectiong_object,
        const Vec3 position,
        const Vec3 surface_normal,
        const double distance_of_ray_support_to_intersection,
        const Vec3 incident_in_obj_sys);
    bool does_intersect()const;
    const SurfaceEntity * get_object()const;
    Vec3 position_in_object_frame()const;
    Vec3 position_in_root_frame()const;
    Vec3 surface_normal_in_object_frame()const;
    Vec3 surface_normal_in_root_frame()const;
    double distance_to_ray_support()const;
    std::string str()const;
    Vec3 reflection_direction_in_root_frame(Vec3 incomming_dir_in_world)const;
    double facing_reflection_propability(const double wavelength)const;
    double refractive_index_going_to(const double wavelength)const;
    double refractive_index_coming_from(const double wavelength)const;
    double get_half_way_depth_coming_from(const double wavelength)const;
    double get_half_way_depth_going_to(const double wavelength)const;
    bool boundary_layer_is_transparent()const;
    bool from_outside_to_inside()const;
    const Color facing_color()const;
    const HomTra3* object2world()const;
    Vec3 get_normal_in_faceing_surface_system()const;
    bool going_to_default_refractive_index()const;
    bool operator<(const Intersection& other) const;
    static bool compare(const Intersection &one, const Intersection &two);
};
#endif  // MCTRACER_CORE_INTERSECTION_H_
