// Copyright 2016 Sebastian A. Mueller
#ifndef PLENOSCOPE_NIGHTSKYBACKGROUND_INJECTOR_H_
#define PLENOSCOPE_NIGHTSKYBACKGROUND_INJECTOR_H_

#include <vector>
#include "Plenoscope/NightSkyBackground/Light.h"
#include "SignalProcessing/PipelinePhoton.h"
#include "Plenoscope/Calibration/LixelStatistics.h"

namespace Plenoscope {
namespace NightSkyBackground {

void inject_nsb_into_photon_pipeline(
    std::vector<std::vector<SignalProcessing::PipelinePhoton>> *
        photon_pipelines,
    const double exposure_time,
    const std::vector<Plenoscope::Calibration::LixelStatistic> *
        lixel_statistics,
    const Light *nsb,
    Random::Generator* prng
);

}  // namespace NightSkyBackground
}  // namespace Plenoscope

#endif  // PLENOSCOPE_NIGHTSKYBACKGROUND_INJECTOR_H_
