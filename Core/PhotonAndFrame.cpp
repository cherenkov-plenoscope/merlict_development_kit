// Copyright 2014 Sebastian A. Mueller
#include "Core/PhotonAndFrame.h"

namespace PhotonAndFrame {

Propagator::Propagator(
    Photon* photon,
    PropagationEnvironment environment
): env(environment), ph(photon) {
    propagate();
}

void Propagator::propagate() {
    if (limit_of_interactions_is_not_reached_yet())
        work_on_first_causal_intersection();
}

bool Propagator::limit_of_interactions_is_not_reached_yet()const {
    return ph->get_number_of_interactions_so_far() <=
        env.propagation_options->max_number_of_interactions_per_photon;
}

void Propagator::work_on_first_causal_intersection() {
    isec = RayAndFrame::first_intersection(ph, env.world_geometry);

    if (isec.does_intersect() && !absorbed_in_medium_before_reaching_surface())
        interact_with_object();
    else
        get_absorbed_in_void_space();
}

bool Propagator::absorbed_in_medium_before_reaching_surface()const {
    const double one_over_e_way =
        isec.get_half_way_depth_coming_from(ph->wavelength);
    const double survival_prob =
        exp(-isec.distance_to_ray_support()/one_over_e_way);
    return env.random_engine->uniform() > survival_prob;
}

void Propagator::interact_with_object() {
    if (
        isec.facing_reflection_propability(ph->wavelength) >=
        env.random_engine->uniform()
    )
        reflect_on_surface_and_propagate_on(reflection_on_surface);
    else
        reach_boundary_layer();
}

void Propagator::get_absorbed_in_void_space() {
    ph->push_back_intersection_and_type_to_propagation_history(
        isec,
        absorption_in_void);
}

void Propagator::reflect_on_surface_and_propagate_on(const Interaction type) {
    ph->set_support_and_direction(
        isec.position_in_root_frame(),
        isec.reflection_direction_in_root_frame(ph->get_direction()));

    ph->push_back_intersection_and_type_to_propagation_history(
        isec,
        type);

    propagate();
}

void Propagator::reach_boundary_layer() {
    if (isec.boundary_layer_is_transparent())
        fresnel_refraction_and_reflection();
    else
        get_absorbed_on_surface();
}

void Propagator::fresnel_refraction_and_reflection() {
    FresnelRefractionAndReflection fresnel(
        isec.object2world()->
            get_transformed_orientation_inverse(ph->get_direction()),
        isec.get_normal_in_faceing_surface_system(),
        isec.refractive_index_coming_from(ph->wavelength),
        isec.refractive_index_going_to(ph->wavelength));

    if (fresnel.reflection_propability() > env.random_engine->uniform())
        reflect_on_surface_and_propagate_on(fresnel_reflection_on_surface);
    else
        pass_the_boundary_layer(fresnel);
}

void Propagator::pass_the_boundary_layer(
    const FresnelRefractionAndReflection &fresnel
) {
    if (isec.from_outside_to_inside())
        ph->push_back_intersection_and_type_to_propagation_history(
            isec,
            refraction_to_inside);
    else
        ph->push_back_intersection_and_type_to_propagation_history(
            isec,
            refraction_to_outside);

    propagate_on_after_boundary_layer(fresnel);
}

void Propagator::propagate_on_after_boundary_layer(
    const FresnelRefractionAndReflection &fresnel
) {
    if (isec.get_object()->has_restrictions_on_frames_to_propagate_to() &&
        !isec.going_to_default_refractive_index()
    )
        env.world_geometry =
            isec.get_object()->get_allowed_frame_to_propagate_to();
    else
        env.world_geometry =
            isec.get_object()->get_root();

    ph->set_support_and_direction(
        isec.position_in_root_frame(),
        isec.object2world()->get_transformed_orientation(
            fresnel.get_refrac_dir_in_object_system()));

    propagate();
}

void Propagator::get_absorbed_on_surface() {
    ph->push_back_intersection_and_type_to_propagation_history(
        isec,
        absorption_on_surface);
}

}  // namespace PhotonAndFrame
