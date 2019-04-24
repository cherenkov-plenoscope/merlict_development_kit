// Copyright 2014 Sebastian A. Mueller
#include "merlict/RayForPropagation.h"
#include <sstream>
#include "merlict/simulation_truth.h"


namespace merlict {

std::string interaction_str(Interaction type) {
    switch (type) {
        case PRODUCTION: return "production"; break;
        case ABSORPTION_IN_VOID: return "absorption_in_void"; break;

        case ABSORPTION_IN_MEDIUM: return "absorption_in_medium"; break;
        case ABSORPTION_ON_SURFACE: return "absorption_on_surface"; break;

        case REFLECTION_ON_SURFACE: return "reflection_on_surface"; break;
        case FRESNEL_REFLECTION_ON_SURFACE:
            return "fresnel_reflection_on_surface"; break;

        case REFRACTION_TO_OUTSIDE: return "refraction_to_outside"; break;
        case REFRACTION_TO_INSIDE: return "refraction_to_inside"; break;

        case SCATTERING: return "scattering"; break;
        default: return "unknown_interaction"; break;
    }
}

RayForPropagation::RayForPropagation():
    simulation_truth_id(0)
{ }

RayForPropagation::RayForPropagation(
    const Vec3 support,
    const Vec3 direction
):
    simulation_truth_id(DEFAULT_SIMULATION_TRUTH) {
    set_support_and_direction(support, direction);
    push_back_production();
}

void RayForPropagation::push_back_production() {
    Intersection production_intersection(
        &SURFACE_PHOTON_SOURCE,
        support_,
        direction_,  // set normal of production obj to direction of ray
        0.0,
        direction_);

    push_back_intersection_and_interaction(
        production_intersection,
        PRODUCTION);
}

std::string RayForPropagation::str()const {
    std::stringstream out;
    out << Ray::str() << ", ";
    out << "ID: " << simulation_truth_id << ", Interactions: ";
    out << num_interactions() << "\n";
    out << history_str();
    return out.str();
}

std::string RayForPropagation::history_str()const {
    std::stringstream out;
    int index = 0;
    for (Interaction type : interaction_history) {
        out << ++index << ") " << interaction_str(type) << " in ";
        out << intersection_history.at(index-1).object()->get_name();
        out << " " << intersection_history.at(index-1).
            position_in_root_frame().str();
        out << ", dist to prev.:";

        if (index > 1) {
            out << intersection_history.at(index-1).
                position_in_root_frame().distance_to(
                    intersection_history.at(index-2).
                        position_in_root_frame())*1e9 << "nm";
        }

        out << "\n";
    }
    return out.str();
}

void RayForPropagation::push_back_intersection_and_interaction(
    const Intersection &interact,
    const Interaction type
) {
    intersection_history.push_back(interact);
    interaction_history.push_back(type);
}

double RayForPropagation::accumulated_distance()const {
    double accumulative_distance = 0.0;

    for (const Intersection &intersection : intersection_history)
        accumulative_distance += intersection.distance_to_ray_support();

    return accumulative_distance;
}

unsigned int RayForPropagation::num_interactions()const {
    return (unsigned int)(intersection_history.size());
}

const Intersection& RayForPropagation::intersection_at(
    const unsigned int index
)const {
    return intersection_history.at(index);
}

const Intersection& RayForPropagation::final_intersection()const {
    return intersection_history.back();
}

Vec3 RayForPropagation::
final_intersection_incident_direction_wrt_frame()const {
    if (intersection_history.size() == 1) {
        // only production
        return VEC3_ORIGIN;
    } else {
        const unsigned int last_i = intersection_history.size() - 1;
        const unsigned int second_last_i = last_i - 1;

        Vec3 final =
            intersection_history.at(last_i).
                position_in_root_frame();

        Vec3 second_last = intersection_history.at(second_last_i).
                position_in_root_frame();

        Vec3 incident_direction_in_world = (final - second_last);
            incident_direction_in_world.normalize();

        return intersection_history.back().object2root()->
            orientation_inverse(incident_direction_in_world);
    }
}

double RayForPropagation::time_of_flight()const {
    return 0.0;
}

Interaction RayForPropagation::final_interaction()const {
    return interaction_history.back();
}

}  // namespace merlict
