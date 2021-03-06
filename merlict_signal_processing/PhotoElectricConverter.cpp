// Copyright 2014 Sebastian A. Mueller
#include "merlict_signal_processing/simulation_truth.h"
#include "merlict_signal_processing/PhotoElectricConverter.h"
#include <sstream>
#include "merlict/merlict.h"
namespace ml = merlict;


namespace signal_processing {
namespace PhotoElectricConverter {

Config::Config() {
    dark_rate = 0.0;
    probability_for_second_puls = 0.0;
    quantum_efficiency_vs_wavelength = &zero_efficiency;
}

Converter::Converter(const Config* config) {
    this->config = config;

    if (config->probability_for_second_puls >= 1.0) {
        std::stringstream info;
        info << __FILE__ << ", " << __LINE__ << "\n";
        info << "Expected config->probability_for_second_puls < 1.0, ";
        info << "but actual it is: " << config->probability_for_second_puls;
        info << "\n";
        throw std::invalid_argument(info.str());
    }
}

std::vector<ElectricPulse> Converter::get_pulse_pipeline_for_photon_pipeline(
    const std::vector<PipelinePhoton> &photon_pipeline,
    const double exposure_time,
    ml::random::Generator* prng
) {
    std::vector<ElectricPulse> electric_pipeline;
    for (const PipelinePhoton &ph : photon_pipeline) {
        if (
            config->quantum_efficiency_vs_wavelength->evaluate(ph.wavelength) >=
            prng->uniform()
        ) {
            const ElectricPulse converted_photon(
                ph.arrival_time,
                ph.simulation_truth_id);
            add_pulse(converted_photon, &electric_pipeline, prng);
        }
    }
    add_accidental_pulse(&electric_pipeline, exposure_time, prng);
    return electric_pipeline;
}

void Converter::add_pulse(
    const ElectricPulse &pulse,
    std::vector<ElectricPulse> *electric_pipeline,
    ml::random::Generator* prng
)const {
    electric_pipeline->push_back(pulse);

    if (config->probability_for_second_puls >= prng->uniform()) {
        const ElectricPulse crosstalk_pulse(
            pulse.arrival_time,
            signal_processing::PHOTO_ELECTRIC_CONVERTER_CROSSTALK);
        add_pulse(crosstalk_pulse, electric_pipeline, prng);
    }
}

void Converter::add_accidental_pulse(
    std::vector<ElectricPulse> *electric_pipeline,
    const double exposure_time,
    ml::random::Generator* prng
)const {
    double relative_arrival_times_sum = prng->expovariate(config->dark_rate);
    while (relative_arrival_times_sum < exposure_time) {
        double time_until_next_pulse = prng->expovariate(config->dark_rate);

        const ElectricPulse accidental_pulse(
            relative_arrival_times_sum,
            signal_processing::PHOTO_ELECTRIC_CONVERTER_ACCIDENTAL);

        add_pulse(
            accidental_pulse,
            electric_pipeline,
            prng);

        relative_arrival_times_sum += time_until_next_pulse;
    }
}

}  // namespace PhotoElectricConverter
}  // namespace signal_processing
