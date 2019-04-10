// Copyright 2014 Sebastian A. Mueller
#ifndef MERLICT_CORSIKA_CORSIKA_H_
#define MERLICT_CORSIKA_CORSIKA_H_

#include <iostream>
#include <vector>
#include <string>
#include <array>

namespace corsika {

void write_273_f4_to_path(
    std::array<float, 273> block,
    const std::string &path);

void write_273_f4_to_path(
    std::vector<std::array<float, 273>> blocks,
    const std::string &path);

std::vector<std::array<float, 273>> read_273_f4_from_path(
    const std::string &path);

namespace header {
namespace event {

float event_number(const std::array<float, 273> &eh);
float particle_id(const std::array<float, 273> &eh);
float total_energy_in_GeV(const std::array<float, 273> &eh);
float zenith(const std::array<float, 273> &eh);
float azimuth(const std::array<float, 273> &eh);
float first_interaction_height_in_cm(const std::array<float, 273> &eh);

}  // namespace event

namespace run {

float run_number(const std::array<float, 273> &rh);
float slope_of_energy_spektrum(const std::array<float, 273> &rh);
float energy_range_start(const std::array<float, 273> &rh);
float energy_range_end(const std::array<float, 273> &rh);
float num_observation_levels(const std::array<float, 273> &rh);
float observation_level_at(
    const std::array<float, 273> &rh,
    const unsigned int i);

}  // namespace run
}  // namespace header

float str2float(const std::string word);
std::string float2str(const float word_in_float);

}  // namespace corsika

#endif  // MERLICT_CORSIKA_CORSIKA_H_
