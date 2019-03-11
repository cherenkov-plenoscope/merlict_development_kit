// Copyright 2014 Sebastian A. Mueller
#include "merlict/SurfaceEntity.h"
#include <limits>
#include <sstream>

namespace merlict {

const SurfaceEntity SurfaceEntity::VOID_SURFACE_ENTITY;
const SurfaceEntity SurfaceEntity::PHOTON_SOURCE;

const Color* SurfaceEntity::DEFAULT_COLOR = &COLOR_DARK_GRAY;

const function::Func1* SurfaceEntity::DEFAULT_REFLECTION = new function::Func1(
    {
        {200e-9, 0},
        {1200e-9, 0}
    });

const function::Func1* SurfaceEntity::DEFAULT_REFRACTION = new function::Func1(
    {
        {200e-9, 1},
        {1200e-9, 1}
    });

const function::Func1* SurfaceEntity::DEFAULT_ABSORPTION = new function::Func1(
    {
        {200e-9, std::numeric_limits<double>::infinity()},
        {1200e-9, std::numeric_limits<double>::infinity()}
    });

SurfaceEntity::SurfaceEntity() {
    init_surface_defaults();
}

SurfaceEntity::SurfaceEntity(
    const std::string new_name,
    const Vec3 new_pos,
    const Rot3 new_rot
): Frame() {
    set_name_pos_rot(new_name, new_pos, new_rot);
    init_surface_defaults();
}

void SurfaceEntity::init_surface_defaults() {
    outer_color = DEFAULT_COLOR;
    inner_color = DEFAULT_COLOR;

    outer_reflection_vs_wavelength = DEFAULT_REFLECTION;
    inner_reflection_vs_wavelength = DEFAULT_REFLECTION;

    outer_refraction_vs_wavelength = DEFAULT_REFRACTION;
    inner_refraction_vs_wavelength = DEFAULT_REFRACTION;

    outer_absorption_vs_wavelength = DEFAULT_ABSORPTION;
    inner_absorption_vs_wavelength = DEFAULT_ABSORPTION;
}

const function::Func1* SurfaceEntity::outer_reflection()const {
    return outer_reflection_vs_wavelength;
}

const function::Func1* SurfaceEntity::inner_reflection()const {
    return inner_reflection_vs_wavelength;
}

const function::Func1* SurfaceEntity::outer_refraction()const {
    return outer_refraction_vs_wavelength;
}

const function::Func1* SurfaceEntity::inner_refraction()const {
    return inner_refraction_vs_wavelength;
}

const function::Func1* SurfaceEntity::outer_absorption()const {
    return outer_absorption_vs_wavelength;
}

const function::Func1* SurfaceEntity::inner_absorption()const {
    return inner_absorption_vs_wavelength;
}

void SurfaceEntity::set_outer_reflection(const function::Func1* reflec) {
    outer_reflection_vs_wavelength = reflec;
}

void SurfaceEntity::set_inner_reflection(const function::Func1* reflec) {
    inner_reflection_vs_wavelength = reflec;
}

void SurfaceEntity::set_outer_refraction(const function::Func1* refrac) {
    outer_refraction_vs_wavelength = refrac;
}

void SurfaceEntity::set_inner_refraction(const function::Func1* refrac) {
    inner_refraction_vs_wavelength = refrac;
}

void SurfaceEntity::set_outer_absorption(const function::Func1* absorp) {
    outer_absorption_vs_wavelength = absorp;
}

void SurfaceEntity::set_inner_absorption(const function::Func1* absorp) {
    inner_absorption_vs_wavelength = absorp;
}

void SurfaceEntity::set_allowed_frames_to_propagate_to(const Frame* frame) {
   _allowed_frame_to_propagate_to = frame;
}

bool SurfaceEntity::has_restrictions_on_frames_to_propagate_to()const {
    return _allowed_frame_to_propagate_to != &VOID_FRAME;
}

const Frame* SurfaceEntity::allowed_frame_to_propagate_to()const {
    return _allowed_frame_to_propagate_to;
}

bool SurfaceEntity::boundary_layer_is_transparent()const {
    if (
        outer_refraction_vs_wavelength != DEFAULT_REFRACTION ||
        inner_refraction_vs_wavelength != DEFAULT_REFRACTION)
        return true;
    else
        return false;
}

void SurfaceEntity::take_boundary_layer_properties_from(
    const SurfaceEntity* proto
) {
    outer_color = proto->outer_color;
    inner_color = proto->inner_color;
    set_outer_reflection(proto->outer_reflection());
    set_inner_reflection(proto->inner_reflection());
    set_outer_refraction(proto->outer_refraction());
    set_inner_refraction(proto->inner_refraction());
    set_outer_absorption(proto->outer_absorption());
    set_inner_absorption(proto->inner_absorption());
}

void SurfaceEntity::take_boundary_layer_properties_but_inside_out_from(
    const SurfaceEntity* proto
) {
    outer_color = proto->inner_color;
    inner_color = proto->outer_color;
    set_outer_reflection(proto->inner_reflection());
    set_inner_reflection(proto->outer_reflection());
    set_outer_refraction(proto->inner_refraction());
    set_inner_refraction(proto->outer_refraction());
    set_outer_absorption(proto->inner_absorption());
    set_inner_absorption(proto->outer_absorption());
}

std::string SurfaceEntity::str()const {
    std::stringstream out;
    out << Frame::str();

    out << " inner surface:\n";
    out << "| color : " << inner_color->str() << "\n";
    out << "| reflec: " << inner_reflection_vs_wavelength->str() << "\n";
    out << "| refrac: " << inner_refraction_vs_wavelength->str() << "\n";
    out << "| absorp: " << inner_absorption_vs_wavelength->str() << "\n";

    out << " outer surface:\n";
    out << "| color : " << outer_color->str() << "\n";
    out << "| reflec: " << outer_reflection_vs_wavelength->str() << "\n";
    out << "| refrac: " << outer_refraction_vs_wavelength->str() << "\n";
    out << "| absorp: " << inner_absorption_vs_wavelength->str() << "\n";

    out << " boundary layer: ";
    if (boundary_layer_is_transparent())
        out << "transparent" << "\n";
    else
        out << "opaque" << "\n";

    if (has_restrictions_on_frames_to_propagate_to())
        out << " inner frame: " << _allowed_frame_to_propagate_to->
            path_in_tree() << "\n";

    return out.str();
}

}  // namespace merlict
