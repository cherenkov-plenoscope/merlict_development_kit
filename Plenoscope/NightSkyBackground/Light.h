// Copyright 2016 Sebastian A. Mueller
#ifndef PLENOSCOPE_NIGHTSKYBACKGROUND_LIGHT_H_
#define PLENOSCOPE_NIGHTSKYBACKGROUND_LIGHT_H_

#include <string>
#include "Plenoscope/LightFieldSensor/Geometry.h"
#include "Core/random/random.h"
#include "Core/Function/Function.h"

namespace Plenoscope {
namespace NightSkyBackground {

struct Light {
    const relleums::Function::Func1* flux_vs_wavelength;
    relleums::random::SamplesFromDistribution wavelength_probability;
    double rate;
    double fov_radius;
    double fov_solid_angle;
    double aperture_radius;
    double aperture_area;
    const LightFieldSensor::Geometry *sensor_geometry;
    Light(
        const LightFieldSensor::Geometry *_sensor_geometry,
        const relleums::Function::Func1* flux_vs_wavelength);
    std::string str()const;
};

}  // namespace NightSkyBackground
}  // namespace Plenoscope

#endif  // PLENOSCOPE_NIGHTSKYBACKGROUND_LIGHT_H_
