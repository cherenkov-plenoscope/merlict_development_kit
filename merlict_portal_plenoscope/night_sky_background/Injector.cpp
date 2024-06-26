// Copyright 2016 Sebastian A. Mueller
#include "merlict_portal_plenoscope/night_sky_background/Injector.h"
#include <limits>
#include "merlict/merlict.h"
namespace ml = merlict;


namespace plenoscope {
namespace night_sky_background {

void inject_nsb_into_photon_pipeline(
    std::vector<std::vector<signal_processing::PipelinePhoton>>
        *photon_pipelines,
    const double nsb_exposure_time,
    const std::vector<float> *lixel_efficiencies,
    const Light *nsb,
    double *nsb_exposure_start_time,
    ml::random::Generator* prng
) {
    if (photon_pipelines->size() == 0)
        return;

    // FIND MIN MAX ARRIVAL TIMES OF CHERENKOV PHOTONS

    unsigned int num_cherenkov_photons = 0;
    std::vector<double> arrival_times;
    double min_crk_arrival_time = std::numeric_limits<double>::max();
    double max_crk_arrival_time = std::numeric_limits<double>::min();

    for (unsigned int i = 0; i < photon_pipelines->size(); i++) {
        if (photon_pipelines->at(i).size() > 0) {
            if (photon_pipelines->at(i).front().arrival_time <
                min_crk_arrival_time
            )
                min_crk_arrival_time =
                    photon_pipelines->at(i).front().arrival_time;
            if (photon_pipelines->at(i).back().arrival_time >
                max_crk_arrival_time
            )
                max_crk_arrival_time =
                    photon_pipelines->at(i).back().arrival_time;
            num_cherenkov_photons += photon_pipelines->at(i).size();
        }
        for (signal_processing::PipelinePhoton ph : photon_pipelines->at(i))
            arrival_times.push_back(ph.arrival_time);
    }

    // FIND MODE OF CHERENKOV PHOTON ARRIVAL TIMES

    double mode_of_cherenkov_arrival_times;

    if (num_cherenkov_photons == 0) {
        mode_of_cherenkov_arrival_times = 0.0;
    } else {
        const unsigned int bin_edge_count = 2u + sqrt(num_cherenkov_photons);
        std::vector<double> arrival_time_bin_edges = ml::numeric::linspace(
            min_crk_arrival_time,
            max_crk_arrival_time,
            bin_edge_count);
        ml::Histogram1 arrival_time_histo(
            arrival_times, arrival_time_bin_edges);
        mode_of_cherenkov_arrival_times = arrival_time_histo.mode();
    }

    // INIT START TIME OF NSB EXPOSURE

    const double NSB_EXPOSURE_START_TIME =
        mode_of_cherenkov_arrival_times - 0.5*nsb_exposure_time;


    for (unsigned int i = 0; i < photon_pipelines->size(); i++) {
        const double lixel_nsb_rate =
            nsb->rate*
            lixel_efficiencies->at(i)/nsb->sensor_geometry->num_lixel();

        std::vector<double> nsb_arrival_times;
        double relative_arrival_times_sum = prng->expovariate(lixel_nsb_rate);
        while (relative_arrival_times_sum < nsb_exposure_time) {
            const double time_until_next_photon =
                prng->expovariate(lixel_nsb_rate);
            nsb_arrival_times.push_back(
                NSB_EXPOSURE_START_TIME + relative_arrival_times_sum);
            relative_arrival_times_sum += time_until_next_photon;
        }

        for (double nsb_arrival_time : nsb_arrival_times) {
            signal_processing::PipelinePhoton nsb_ph(
                nsb_arrival_time,
                nsb->wavelength_probability.draw(prng->uniform()),
                signal_processing::NIGHT_SKY_BACKGROUND);
            photon_pipelines->at(i).push_back(nsb_ph);
        }

        sort_photon_pipelines_arrival_time(&photon_pipelines->at(i));
    }

    // SUBTRACT NSB_EXPOSURE_START_TIME

    for (unsigned int i = 0; i < photon_pipelines->size(); i++) {
        for (unsigned int p = 0; p < photon_pipelines->at(i).size(); p++) {
            photon_pipelines->at(i).at(p).arrival_time -=
                NSB_EXPOSURE_START_TIME;
        }
    }

    (*nsb_exposure_start_time) = NSB_EXPOSURE_START_TIME;
}

}  // namespace night_sky_background
}  // namespace plenoscope
