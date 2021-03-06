// Copyright 2016 Sebastian A. Mueller
#include "merlict_signal_processing/PhotonStream.h"
#include <math.h>
#include <sstream>
#include <fstream>
#include "merlict_signal_processing/pulse_extraction.h"
#include "merlict/merlict.h"
namespace ml = merlict;


namespace signal_processing {
namespace PhotonStream {

Stream::Stream() {
    time_slice_duration = 0.0;
}

void write(
    const std::vector<std::vector<ExtractedPulse>> &channels,
    const float time_slice_duration,
    const std::string path
) {
    const uint32_t num_channels = channels.size();
    const uint32_t num_symbols = num_symbols_to_represent(channels);

    std::ofstream file;
    file.open(path, std::ios::binary);

    if (!file.is_open()) {
        std::stringstream info;
        info << "Can not write file '" << path << "'.\n";
        throw std::runtime_error(info.str());
    }

    // PhotonStream Header 16 byte
    // -------------------
    ml::append_float32(time_slice_duration, file);
    ml::append_uint32(num_channels, file);
    ml::append_uint32(NUMBER_TIME_SLICES, file);
    ml::append_uint32(num_symbols, file);

    // Pulses
    // ------
    for (uint64_t ch = 0; ch < num_channels; ch++) {
        for (uint64_t pu = 0; pu < channels.at(ch).size(); pu++) {
            if (
                channels.at(ch).at(pu).arrival_time_slice ==
                NEXT_CHANNEL_MARKER
            ) {
                std::stringstream info;
                info << "PhotonStream::write(" << path << ")\n";
                info << "Expected arrival slice of photon != ";
                info << "NEXT_CHANNEL_MARKER\n";
                throw std::runtime_error(info.str());
            }
            ml::append_uint8(channels.at(ch).at(pu).arrival_time_slice, file);
        }
        if (ch < num_channels-1)
            ml::append_uint8(NEXT_CHANNEL_MARKER, file);
    }

    file.close();
}

void write_simulation_truth(
    const std::vector<std::vector<ExtractedPulse>> &channels,
    const std::string path
) {
    std::ofstream file;
    file.open(path, std::ios::binary);

    if (!file.is_open()) {
        std::stringstream info;
        info << "Can not write file '" << path << "'.\n";
        throw std::runtime_error(info.str());
    }

    for (uint32_t ch = 0; ch < channels.size(); ch++) {
        for (uint32_t pu = 0; pu < channels.at(ch).size(); pu++) {
            ml::append_int32(
                channels.at(ch).at(pu).simulation_truth_id,
                file);
        }
    }

    file.close();
}

Stream read(const std::string path) {
    Stream stream;

    std::ifstream file;
    file.open(path, std::ios::binary);

    if (!file.is_open()) {
        std::stringstream info;
        info << __FILE__ << " " << __LINE__ << "\n";
        info << "PhotonStream: Unable to open file: '" << path << "'\n";
        throw std::runtime_error(info.str());}

    stream.time_slice_duration = ml::read_float32(file);
    ml::read_uint32(file);
    uint32_t num_time_slices = ml::read_uint32(file);
    if (num_time_slices != NUMBER_TIME_SLICES) {
        std::stringstream info;
        info << "PhotonStream::read(" << path << ")\n";
        info << "Expected num_time_slices == " << NUMBER_TIME_SLICES;
        info << ", but actual it is " << num_time_slices << "\n";
        throw std::runtime_error(info.str());
    }
    uint32_t num_symbols = ml::read_uint32(file);

    if (num_symbols > 0) {
        std::vector<ExtractedPulse> first_channel;
        stream.photon_stream.push_back(first_channel);
    }

    uint32_t channel = 0;
    for (uint32_t i = 0; i < num_symbols; i++) {
        uint8_t symbol = ml::read_uint8(file);
        if (symbol == NEXT_CHANNEL_MARKER) {
            channel++;
            if (i < num_symbols) {
                std::vector<ExtractedPulse> next_channel;
                stream.photon_stream.push_back(next_channel);
            }
        } else {
            ExtractedPulse pulse;
            pulse.arrival_time_slice = symbol;
            stream.photon_stream.at(channel).push_back(pulse);
        }
    }

    file.close();

    return stream;
}

Stream read_with_simulation_truth(
    const std::string path,
    const std::string truth_path
) {
    Stream stream = read(path);

    std::ifstream file;
    file.open(truth_path, std::ios::binary);
    if (!file.is_open()) {
        std::stringstream info;
        info << __FILE__ << " " << __LINE__ << "\n";
        info << "PhotonStream: Unable to open file: '" << truth_path << "'\n";
        throw std::runtime_error(info.str());}

    for (
        uint32_t channel = 0;
        channel < stream.photon_stream.size();
        channel++
    ) {
        for (
            uint32_t pulse = 0;
            pulse < stream.photon_stream.at(channel).size();
            pulse++
        ) {
            stream.photon_stream.at(channel).at(pulse).simulation_truth_id =
                ml::read_int32(file);
        }
    }

    file.close();
    return stream;
}

uint64_t num_pulses(
    const std::vector<std::vector<ExtractedPulse>> &raw
) {
    uint64_t num_pulses = 0;
    for (uint64_t channel = 0; channel < raw.size(); channel++)
        num_pulses += raw.at(channel).size();
    return num_pulses;
}

uint64_t num_symbols_to_represent(
    const std::vector<std::vector<ExtractedPulse>> &raw
) {
    uint64_t num_pulses_plus_num_channels =
        num_pulses(raw) + raw.size();

    uint64_t num_symbols;
    if (num_pulses_plus_num_channels > 0)
        num_symbols = num_pulses_plus_num_channels - 1;
    else
        num_symbols = 0;
    return num_symbols;
}

}  // namespace PhotonStream
}  // namespace signal_processing
