// Copyright 2014 Sebastian A. Mueller
#ifndef CORE_SCENERY_SEGMENTED_IMAGING_REFLECTOR_CONFIG_H_
#define CORE_SCENERY_SEGMENTED_IMAGING_REFLECTOR_CONFIG_H_

#include "Core/Color.h"
#include "Core/function/function.h"

namespace relleums {
namespace segmented_imaging_reflector {

struct Config {
    double focal_length;
    double DaviesCotton_over_parabolic_mixing_factor;
    double max_outer_aperture_radius;
    double min_inner_aperture_radius;
    double facet_inner_hex_radius;
    double gap_between_facets;
    const Color *mirror_color;
    const Color *inner_mirror_color;
    const function::Func1* reflectivity;
    Config();
};

static const function::Func1 typical_reflectivity(
    {
        {200e-9, .8},
        {1200e-9, .8}
    });

static const function::Func1 perfect_reflectivity(
    {
        {200e-9, 1},
        {1200e-9, 1}
    });

}  // namespace segmented_imaging_reflector
}  // namespace relleums

#endif  // CORE_SCENERY_SEGMENTED_IMAGING_REFLECTOR_CONFIG_H_
