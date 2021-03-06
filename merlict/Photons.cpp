// Copyright 2014 Sebastian A. Mueller
#include "merlict/Photons.h"
#include <sstream>
#include "merlict/PhotonAndFrame.h"


namespace merlict {

void propagate_photons_in_frame_with_config(
    std::vector<Photon> *photons,
    const Frame* world,
    const PropagationConfig* settings,
    random::Generator* prng
) {
    PropagationEnvironment env;
    env.root_frame = world;
    env.config = settings;
    env.prng = prng;

    for (unsigned int i = 0; i < photons->size(); i++)
        Propagator(&photons->at(i), env);
}

std::vector<Photon> raw_matrix2photons(
    std::vector<std::vector<double>> raw_matrix
) {
    std::vector<Photon> photons;
    for (std::vector<double> raw_row : raw_matrix)
        photons.push_back(raw_row2photon(raw_row));
    return photons;
}

std::vector<std::vector<double>> photons2raw_matrix(
    std::vector<Photon> *photons
) {
    std::vector<std::vector<double>> raw_matrix;
    for (const Photon &ph : *photons)
        raw_matrix.push_back(photon2raw_row(ph));
    return raw_matrix;
}

std::vector<double> photon2raw_row(const Photon& ph) {
    std::vector<double> raw_row;
    raw_row.reserve(8);

    raw_row.push_back(static_cast<double>(ph.simulation_truth_id));

    raw_row.push_back(ph.support().x);
    raw_row.push_back(ph.support().y);
    raw_row.push_back(ph.support().z);

    raw_row.push_back(ph.direction().x);
    raw_row.push_back(ph.direction().y);
    raw_row.push_back(ph.direction().z);

    raw_row.push_back(ph.wavelength);

    return raw_row;
}

Photon raw_row2photon(const std::vector<double> &raw_row) {
    assert_raw_row_size_matches_photon(raw_row);
    const double id = raw_row[0];
    const Vec3 support(raw_row[1], raw_row[2], raw_row[3]);
    const Vec3 direction(raw_row[4], raw_row[5], raw_row[6]);
    const double wavelength = raw_row[7];

    Photon ph = Photon(support, direction, wavelength);
    ph.simulation_truth_id = id;
    return ph;
}

void assert_raw_row_size_matches_photon(const std::vector<double> &raw_row) {
    if (raw_row.size() != 8) {
        std::stringstream out;
        out << "PhotonBunch, raw row of doubles to photon.\n";
        out << "Expected row to have exactly 8 columns, but actual it has ";
        out << raw_row.size() << " columns.\n";
        throw std::invalid_argument(out.str());
    }
}

namespace photon_source {

std::vector<Photon> point_like_towards_z_opening_angle_num_photons(
    const double opening_angle,
    const unsigned int num_photons,
    random::Generator* prng
) {
    std::vector<Photon> photons;
    photons.reserve(num_photons);
    const Vec3 support = VEC3_ORIGIN;

    random::ZenithDistancePicker zenith_picker(0.0, opening_angle);
    random::UniformPicker azimuth_picker(0.0, 2*M_PI);
    for (unsigned int i = 0; i < num_photons; i++) {
        Vec3 direction = random::draw_point_on_sphere(
            prng,
            zenith_picker,
            azimuth_picker);
        Photon ph = Photon(support, direction, 433e-9);
        ph.simulation_truth_id = i;
        photons.push_back(ph);
    }
    return photons;
}

std::vector<Photon> parallel_towards_z_from_xy_disc(
    const double disc_radius,
    const unsigned int num_photons,
    random::Generator* prng
) {
    std::vector<Photon> photons;
    photons.reserve(num_photons);
    const Vec3 direction = VEC3_UNIT_Z;

    for (unsigned int i = 0; i < num_photons; i++) {
        Vec3 support = prng->get_point_on_xy_disc_within_radius(
            disc_radius);
        Photon ph(support, direction, 433e-9);
        ph.simulation_truth_id = i;
        photons.push_back(ph);
    }
    return photons;
}

}  // namespace photon_source
}  // namespace merlict
