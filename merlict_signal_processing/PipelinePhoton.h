// Copyright 2014 Sebastian A. Mueller
#ifndef SIGNALPROCESSING_PIPELINEPHOTON_H_
#define SIGNALPROCESSING_PIPELINEPHOTON_H_

#include <stdint.h>
#include <vector>
#include "merlict/merlict.h"

namespace signal_processing {

struct PipelinePhoton {
    double arrival_time;
    double wavelength;
    int32_t simulation_truth_id;
    PipelinePhoton();
    PipelinePhoton(
        const double arrival_time,
        const double wavelength,
        const int32_t simulation_truth_id);
};

std::vector<std::vector<PipelinePhoton>> get_photon_pipelines(
    const merlict::sensor::Sensors* sensors);

void sort_photon_pipelines_arrival_time(
    std::vector<PipelinePhoton>* pipeline);

}  // namespace signal_processing

#endif  // SIGNALPROCESSING_PIPELINEPHOTON_H_
