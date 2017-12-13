// Copyright 2014 Sebastian A. Mueller
#include "Core/Photons.h"
#include <omp.h>
#include <sstream>
#include <iostream>
#include "Core/PhotonAndFrame.h"
using std::string;
using std::stringstream;
using std::vector;

namespace Photons {

string get_print(const vector<Photon> *photons) {
    stringstream out;
    out << "Photon bundle "<< photons->size() << "\n";
    for (const Photon &photon : *photons)
        out << photon.str() << "\n";
    return out.str();
}

// propagation

void propagate_photons_in_scenery_with_settings(
    vector<Photon> *photons,
    const Frame* world,
    const PropagationConfig* settings,
    Random::Generator* prng
) {
    if (settings->use_multithread_when_possible)
        propagate_photons_using_multi_thread(photons, world, settings);
    else
        propagate_photons_using_single_thread(photons, world, settings, prng);
}

void propagate_photons_using_single_thread(
    vector<Photon> *photons,
    const Frame* world,
    const PropagationConfig* settings,
    Random::Generator* prng
) {
    PropagationEnvironment env;
    env.world_geometry = world;
    env.propagation_options = settings;
    env.random_engine = prng;

    for (unsigned int i = 0; i < photons->size(); i++)
        PhotonAndFrame::Propagator(&photons->at(i), env);
}

void propagate_photons_using_multi_thread(
    vector<Photon> *photons,
    const Frame* world,
    const PropagationConfig* settings
) {
    unsigned int i;
    unsigned int number_of_threads;
    unsigned int thread_id, ray_counter;
    stringstream out;
    int HadCatch = 0;

    #pragma omp parallel shared(settings, world, HadCatch) private(number_of_threads, thread_id, out, ray_counter)
    {
        Random::Mt19937 dice_for_this_thread_only;
        ray_counter = 0;
        thread_id = omp_get_thread_num();
        number_of_threads = omp_get_num_threads();

        PropagationEnvironment env_for_this_thread_only;
        env_for_this_thread_only.world_geometry = world;
        env_for_this_thread_only.propagation_options = settings;
        env_for_this_thread_only.random_engine = &dice_for_this_thread_only;

        #pragma omp for schedule(dynamic) private(i)
        for (i = 0; i < photons->size(); i++) {
            try {
                ray_counter++;
                PhotonAndFrame::Propagator(
                    &photons->at(i),
                    env_for_this_thread_only);
            } catch (std::exception &error) {
                HadCatch++;
                std::cerr << error.what();
            } catch (...) {
                HadCatch++;
            }
        }

        out << "Thread " << thread_id+1 << "/" << number_of_threads;
        out << " is doing " << ray_counter << "/";
        out << photons->size() << " photons. ";
        out << "Seed: " << dice_for_this_thread_only.get_seed() << "\n";
        //  cout << out.str();
    }

    if (HadCatch) {
        stringstream info;
        info << "PhotonBunch::" << __func__ << "() in " << __FILE__ << ", ";
        info << __LINE__ <<"\n";
        info << "Cought exception during multithread propagation.\n";
        throw std::runtime_error(info.str());
    }
}
/*
A throw executed inside a parallel region must cause execution to resume
within the same parallel region, and it must be caught by the same thread
that threw the exception
*/

// In Out to raw matrix/table -> AsciiIO can read/write this to text files

vector<Photon> raw_matrix2photons(vector<vector<double>> raw_matrix) {
    vector<Photon> photons;
    for (vector<double> raw_row : raw_matrix)
        photons.push_back(raw_row2photon(raw_row));
    return photons;
}

vector<vector<double>> photons2raw_matrix(vector<Photon> *photons) {
    vector<vector<double>> raw_matrix;
    for (Photon &ph : *photons)
        raw_matrix.push_back(photon2raw_row(&ph));
    return raw_matrix;
}

vector<double> photon2raw_row(Photon* ph) {
    vector<double> raw_row;
    raw_row.reserve(8);

    raw_row.push_back(static_cast<double>(ph->get_simulation_truth_id()));

    raw_row.push_back(ph->get_support().x);
    raw_row.push_back(ph->get_support().y);
    raw_row.push_back(ph->get_support().z);

    raw_row.push_back(ph->get_direction().x);
    raw_row.push_back(ph->get_direction().y);
    raw_row.push_back(ph->get_direction().z);

    raw_row.push_back(ph->get_wavelength());

    return raw_row;
}

Photon raw_row2photon(vector<double> &raw_row) {
    assert_raw_row_size_matches_photon(raw_row);
    const double id = raw_row[0];
    const Vec3 support(raw_row[1], raw_row[2], raw_row[3]);
    const Vec3 direction(raw_row[4], raw_row[5], raw_row[6]);
    const double wavelength = raw_row[7];

    Photon ph = Photon(support, direction, wavelength);
    ph.set_simulation_truth_id(id);
    return ph;
}

void assert_raw_row_size_matches_photon(vector<double> &raw_row) {
    if (raw_row.size() != 8) {
        stringstream out;
        out << "PhotonBunch, raw row of doubles to photon.\n";
        out << "Expected row to have exactly 8 columns, but actual it has ";
        out << raw_row.size() << " columns.\n";
        throw std::invalid_argument(out.str());
    }
}

namespace Source {

vector<Photon> point_like_towards_z_opening_angle_num_photons(
    const double opening_angle,
    const unsigned int number_of_photons
) {
    vector<Photon> photons;
    photons.reserve(number_of_photons);
    const Vec3 support = Vec3::ORIGIN;

    Random::Mt19937 prng(0);

    for (unsigned int i = 0; i < number_of_photons; i++) {
        Vec3 direction =
            prng.get_point_on_unitsphere_within_polar_distance(opening_angle);
        Photon ph = Photon(support, direction, 433e-9);
        ph.set_simulation_truth_id(i);
        photons.push_back(ph);
    }
    return photons;
}

vector<Photon> parallel_towards_z_from_xy_disc(
    const double disc_radius,
    const unsigned int number_of_photons
) {
    vector<Photon> photons;
    photons.reserve(number_of_photons);
    const Vec3 direction = Vec3::UNIT_Z;

    Random::Mt19937 prng(0);

    for (unsigned int i = 0; i < number_of_photons; i++) {
        Vec3 support = prng.get_point_on_xy_disc_within_radius(
            disc_radius);
        Photon ph(support, direction, 433e-9);
        ph.set_simulation_truth_id(i);
        photons.push_back(ph);
    }
    return photons;
}

}  // namespace Source

// transformations, move and rotate photons

void transform_all_photons(const HomTra3 Trafo, vector<Photon> *photons) {
    for (size_t i = 0; i < photons->size(); i++)
        photons->at(i).transform(&Trafo);
}

void transform_all_photons_multi_thread(
    const HomTra3 Trafo,
    vector<Photon> *photons
) {
    unsigned int i;
    int HadCatch = 0;
    #pragma omp parallel shared(photons, HadCatch)
    {
        #pragma omp for schedule(dynamic) private(i)
        for (i = 0; i < photons->size(); i++) {
            try {
                photons->at(i).transform(&Trafo);
            } catch (std::exception &error) {
                HadCatch++;
                std::cerr << error.what();
            } catch (...) {
                HadCatch++;
            }
        }
    }

    if (HadCatch) {
        stringstream info;
        info << "PhotonBunch::" << __func__ << "() in " << __FILE__ << ", ";
        info << __LINE__ << "\n";
        info << "Cought exception during multithread transformation.\n";
        throw std::runtime_error(info.str());
    }
}

}  // namespace Photons
