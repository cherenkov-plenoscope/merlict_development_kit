// Copyright 2014 Sebastian A. Mueller
#include "merlict_signal_processing/PipelinePhoton.h"
#include <algorithm>
#include "merlict_signal_processing/simulation_truth.h"


namespace signal_processing {

PipelinePhoton::PipelinePhoton():
    simulation_truth_id(DEFAULT_SIMULATION_TRUTH) {
    arrival_time = 0.0;
    wavelength = 0.0;
}

PipelinePhoton::PipelinePhoton(
    const double _arrival_time,
    const double _wavelength,
    const int32_t _simulation_truth_id
):
    arrival_time(_arrival_time),
    wavelength(_wavelength),
    simulation_truth_id(_simulation_truth_id)
{}

std::vector<std::vector<PipelinePhoton>> get_photon_pipelines(
    const merlict::sensor::Sensors* sensors
) {
    std::vector<std::vector<PipelinePhoton>> photon_pipelines;
    const unsigned int num_sensors = sensors->size();
    photon_pipelines.reserve(num_sensors);

    // for each sensor
    for (unsigned int i = 0; i < num_sensors; i++) {
        std::vector<PipelinePhoton> photon_pipeline;
        const unsigned int num_photons =
            sensors->by_occurence[i]->photon_arrival_history.size();
        photon_pipeline.reserve(num_photons);

        // for each photon
        for (unsigned int j = 0; j < num_photons; j++) {
            PipelinePhoton ph(
                sensors->by_occurence[i]->photon_arrival_history[j].
                    arrival_time,
                sensors->by_occurence[i]->photon_arrival_history[j].
                    wavelength,
                sensors->by_occurence[i]->photon_arrival_history[j].
                    simulation_truth_id);

            photon_pipeline.push_back(ph);
        }

        sort_photon_pipelines_arrival_time(&photon_pipeline);
        photon_pipelines.push_back(photon_pipeline);
    }

    return photon_pipelines;
}

void sort_photon_pipelines_arrival_time(std::vector<PipelinePhoton>* pipeline) {
    std::sort(
        pipeline->begin(),
        pipeline->end(),
        [&](const PipelinePhoton& a, const PipelinePhoton& b) {
            return (a.arrival_time < b.arrival_time);
        });
}

}  // namespace signal_processing
