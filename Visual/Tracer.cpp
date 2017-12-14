// Copyright 2014 Sebastian A. Mueller
#include "Visual/Tracer.h"

namespace Visual {

const double Tracer::wavelength = 533e-9;

Tracer::Tracer(
    CameraRay* _cray,
    const Frame* _scenery,
    const Config* _config,
    Random::Mt19937* _prng
):
    prng(_prng),
    scenery(_scenery),
    config(_config),
    cray(_cray)
{
    trace_back();
}

void Tracer::trace_back() {
    isec = RayAndFrame::first_intersection(cray, scenery);

    if (isec.does_intersect())
        trace_back_to_object_interaction();
    else
        trace_back_to_sky_dome();
}

void Tracer::trace_back_to_object_interaction() {
    if (isec.facing_reflection_propability(wavelength) >= prng->uniform())
        trace_back_after_reflection();
    else
        trace_back_to_boundary_layer();
}

void Tracer::trace_back_after_reflection() {
    cray->set_support_and_direction(
        isec.position_in_root_frame(),
        isec.reflection_direction_in_root_frame(cray->get_direction()));
    trace_back();
}

void Tracer::trace_back_to_boundary_layer() {
    if (isec.boundary_layer_is_transparent())
        trace_back_to_fresnel_interaction();
    else
        trace_back_to_surface();
}

void Tracer::trace_back_to_fresnel_interaction() {
    FresnelRefractionAndReflection fresnel(
        isec.object2world()->
            get_transformed_orientation_inverse(cray->get_direction()),
        isec.get_normal_in_faceing_surface_system(),
        isec.refractive_index_coming_from(wavelength),
        isec.refractive_index_going_to(wavelength));

    if (fresnel.reflection_propability() > prng->uniform())
        trace_back_after_reflection();
    else
        trace_back_beyond_boundary_layer(fresnel);
}

void Tracer::trace_back_to_surface() {
    if (config->global_illumination.on)
        color = shadow_of_sky_light();
    else
        color = isec.facing_color();
}

void Tracer::trace_back_beyond_boundary_layer(
    const FresnelRefractionAndReflection &fresnel
) {
    if (isec.get_object()->has_restrictions_on_frames_to_propagate_to() &&
        !isec.going_to_default_refractive_index()
    )
        scenery = isec.get_object()->get_allowed_frame_to_propagate_to();
    else
        scenery = isec.get_object()->get_root();

    cray->set_support_and_direction(
        isec.position_in_root_frame(),
        isec.object2world()->get_transformed_orientation(
            fresnel.get_refrac_dir_in_object_system()));

    trace_back();
}

void Tracer::trace_back_to_sky_dome() {
    color = config->sky_dome.get_color_for_direction(cray->get_direction());
}

Color Tracer::shadow_of_sky_light()const {
    const double max_darkening = 0.750;

    Vec3 specular_dir = isec.reflection_direction_in_root_frame(
        config->global_illumination.incoming_direction);

    double darkening = specular_dir*cray->get_direction();

    if (darkening < 0.0) darkening = 0.0;
    if (darkening > max_darkening) darkening = max_darkening;

    Color color;
    color = isec.facing_color();

    if (!surface_iluminated_by_global_light_source())
        darkening = darkening*0.25;

    color.reflection_mix(Color::BLACK, max_darkening - darkening);

    return color;
}

bool Tracer::surface_iluminated_by_global_light_source()const {
    Ray ray_to_source(
        isec.position_in_root_frame(),
        config->global_illumination.incoming_direction);

    const Intersection intersec_light_source =
        RayAndFrame::first_intersection(&ray_to_source, scenery);

    double p = isec.surface_normal_in_root_frame()*
        config->global_illumination.incoming_direction;

    if (surface_normal_is_facing_observer(isec))
        p = p*-1.0;

    bool does_intersect = intersec_light_source.does_intersect();

    return !(does_intersect || p < 0.0);
}

bool Tracer::surface_normal_is_facing_observer(
    const Intersection &intersection
)const {
    return (
        intersection.surface_normal_in_root_frame()*
        cray->get_direction() >
        0.0);
}

}  // namespace Visual
