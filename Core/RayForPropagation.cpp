// Copyright 2014 Sebastian A. Mueller
#include "Core/RayForPropagation.h"
#include <sstream>
#include "Core/SimulationTruth.h"

using std::stringstream;
using std::vector;
using std::string;


RayForPropagation::RayForPropagation(
    const Vec3 support,
    const Vec3 direction
):
    simulation_truth_id(SimulationTruth::MCTRACER_DEFAULT) {
    set_support_and_direction(support, direction);
    push_back_production_of_ray();
}

void RayForPropagation::push_back_production_of_ray() {
    Intersection production_intersection(
        &SurfaceEntity::PHOTON_SOURCE,
        support,
        direction,  // set normal of production obj to direction of ray
        0.0,
        direction);

    push_back_intersection_and_type_to_propagation_history(
        production_intersection,
        production);
}

void RayForPropagation::set_simulation_truth_id(const int id) {
    simulation_truth_id = id;
}

int RayForPropagation::get_simulation_truth_id()const {
    return simulation_truth_id;
}

string RayForPropagation::str()const {
    stringstream out;
    out << Ray::str() << ", ";
    out << "ID: " << simulation_truth_id << ", Interactions: ";
    out << get_number_of_interactions_so_far() << "\n";
    out << get_history_print();
    return out.str();
}

string RayForPropagation::get_history_print()const {
    stringstream out;
    int index = 0;
    for (Interaction type : interaction_history) {
        out << ++index << ") " << get_type_print(type) << " in ";
        out << intersection_history.at(index-1).get_object()->get_name();
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

void RayForPropagation::push_back_intersection_and_type_to_propagation_history(
    const Intersection &interact,
    const Interaction type
) {
    intersection_history.push_back(interact);
    interaction_history.push_back(type);
}

double RayForPropagation::get_accumulative_distance()const {
    double accumulative_distance = 0.0;

    for (const Intersection &intersection : intersection_history)
        accumulative_distance += intersection.distance_to_ray_support();

    return accumulative_distance;
}

unsigned int RayForPropagation::get_number_of_interactions_so_far()const {
    return (unsigned int)(intersection_history.size());
}

const Intersection& RayForPropagation::get_intersection_at(
    const unsigned int index
)const {
    return intersection_history.at(index);
}

string RayForPropagation::get_type_print(const Interaction type)const {
    switch (type) {
        case production: return "production"; break;
        case absorption_in_void: return "absorption_in_void"; break;

        case absorption_in_medium: return "absorption_in_medium"; break;
        case absorption_on_surface: return "absorption_on_surface"; break;

        case reflection_on_surface: return "reflection_on_surface"; break;
        case fresnel_reflection_on_surface:
            return "fresnel_reflection_on_surface"; break;

        case refraction_to_outside: return "refraction_to_outside"; break;
        case refraction_to_inside: return "refraction_to_inside"; break;

        case scattering: return "scattering"; break;
        default: return "unknown_interaction"; break;
    }
}

const Intersection& RayForPropagation::get_final_intersection()const {
    return intersection_history.back();
}

Vec3 RayForPropagation::
get_final_intersection_incident_vector_in_object_frame()const {
    if (intersection_history.size() == 1) {
        // only production
        return Vec3::ORIGIN;
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

        return intersection_history.back().object2world()->
            get_transformed_orientation_inverse(incident_direction_in_world);
    }
}

double RayForPropagation::get_time_of_flight()const {
    return 0.0;
}

Interaction RayForPropagation::get_final_interaction_type()const {
    return interaction_history.back();
}
