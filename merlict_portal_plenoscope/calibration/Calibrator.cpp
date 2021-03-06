// Copyright 2014 Sebastian A. Mueller
#include "merlict_portal_plenoscope/calibration/Calibrator.h"
#include <future>
#include <thread>
#include <iomanip>
#include <sstream>
#include <random>
#include <iostream>
#include "merlict/merlict.h"
#include "merlict_portal_plenoscope/calibration/Writer.h"
#include "merlict_portal_plenoscope/night_sky_background/NightSkyBackground.h"
#include "merlict_multi_thread/vitaliy_vitsentiy_thread_pool.h"
namespace ml = merlict;


namespace plenoscope {
namespace calibration {

Calibrator::Calibrator(
    const Config _config,
    const PlenoscopeInScenery *_plenoscope,
    const ml::Frame* _world):
        config(_config),
        plenoscope(_plenoscope),
        world(_world),
        num_photons(config.photons_per_block*config.num_blocks),
        MAX_APERTURE_PLANE_RADIUS(
            night_sky_background::APERTURE_RADIUS_OVERHEAD*
            plenoscope->light_field_sensor_geometry.
                expected_imaging_system_max_aperture_radius()),
        MAX_INCIDENT_ANGLE(
            night_sky_background::FOV_RADIUS_OVERHEAD*
            plenoscope->light_field_sensor_geometry.max_FoV_radius()
        ) {}


ml::Photon create_photon(
    ml::Vec3 support_on_aperture,
    ml::Vec3 incident_direction
) {
    const ml::Ray back_running_ray(support_on_aperture, incident_direction);

    const ml::Vec3 creation_position = back_running_ray.position_at(
        DISTANCE_TO_APERTURE_PLANE);

    return ml::Photon(creation_position, incident_direction*(-1.0), WAVELENGTH);
}


CalibrationPhotonResult one_photon(
    int id,
    const uint64_t seed,
    const Calibrator &cal,
    const ml::random::ZenithDistancePicker &zenith_picker,
    const ml::random::UniformPicker &azimuth_picker)
{
    (void)id;
    ml::random::Mt19937 prng(seed);

    // create photon
    const ml::Vec3 support_on_aperture = prng.get_point_on_xy_disc_within_radius(
        cal.MAX_APERTURE_PLANE_RADIUS);

    const ml::Vec3 incident_direction = ml::random::draw_point_on_sphere(
        &prng,
        zenith_picker,
        azimuth_picker);

    ml::Photon ph = create_photon(support_on_aperture, incident_direction);

    // propagate photon
    ml::PropagationEnvironment env;
    env.root_frame = cal.world;
    env.prng = &prng;
    ml::Propagator(&ph, env);

    ml::sensor::FindSensorByFrame sensor_finder(
        ph.final_intersection().object(),
        &cal.plenoscope->light_field_channels->by_frame);

    if (sensor_finder.is_absorbed_by_known_sensor) {
        // remember photon
        CalibrationPhotonResult result;
        result.reached_sensor = true;
        result.lixel_id = sensor_finder.final_sensor->id;
        result.x_pos_on_principal_aperture = support_on_aperture.x;
        result.y_pos_on_principal_aperture = support_on_aperture.y;
        result.x_tilt_vs_optical_axis = incident_direction.x;
        result.y_tilt_vs_optical_axis = incident_direction.y;
        result.relative_time_of_flight = ph.time_of_flight();
        return result;
    } else {
        CalibrationPhotonResult result;
        result.reached_sensor = false;
        return result;
    }
}


void fill_another_block(
    const Calibrator &cal,
    LixelStatisticsFiller *lixel_statistics_filler,
    ml::random::Generator *prng)
{
    std::vector<CalibrationPhotonResult> photon_results(
        cal.config.photons_per_block);

    const ml::random::ZenithDistancePicker zenith_picker(
        0.0,
        cal.MAX_INCIDENT_ANGLE);
    const ml::random::UniformPicker azimuth_picker(
        0.0,
        2*M_PI);

    std::vector<uint64_t> prng_seeds(photon_results.size());
    for (uint64_t i = 0; i < photon_results.size(); i ++) {
        prng_seeds[i] = prng->create_seed();
    }

    uint64_t num_threads = std::thread::hardware_concurrency();
    merlict::ctpl::thread_pool pool(num_threads);
    std::vector<std::future<CalibrationPhotonResult>> futs(
        photon_results.size());
    for (uint64_t i = 0; i < futs.size(); ++i) {
        futs[i] = pool.push(
            one_photon,
            prng_seeds[i],
            cal,
            zenith_picker,
            azimuth_picker);
    }

    for (uint64_t i = 0; i < futs.size(); i ++) {
        photon_results[i] = futs[i].get();
    }

    lixel_statistics_filler->fill_in_block(photon_results);
}

void run_calibration(
    const Calibrator &cal,
    const std::string &path,
    ml::random::Generator *prng
) {
    LixelStatisticsFiller lixel_statistics_filler(
        &cal.plenoscope->light_field_sensor_geometry,
        &cal.config);

    std::cout << "Plenoscope Calibrator: propagating ";
    std::cout << double(cal.num_photons)/1.0e6 << "e6 photons\n";

    for (uint64_t block = 0; block < cal.config.num_blocks; block++) {
        std::cout << block + 1 << " of " << cal.config.num_blocks << "\n";
        fill_another_block(
            cal,
            &lixel_statistics_filler,
            prng);
    }
    std::vector<LixelStatistic> lixel_statistics =
        lixel_statistics_filler.get_lixel_statistics();
    write(lixel_statistics, path);
}


/*

void Calibrator::fill_calibration_block_to_table() {
    random::ZenithDistancePicker zenith_picker(
        0.0,
        max_tilt_vs_optical_axis_to_throw_photons_in);
    random::UniformPicker azimuth_picker(0.0, 2*M_PI);

    unsigned int i;
    int HadCatch = 0;
    std::random_device rd;
    const uint32_t master_seed = rd();


    for (i = 0; i < config.photons_per_block; i++) {

        // create photon
        Vec3 pos_on_principal_aperture =
            thread_local_prng.get_point_on_xy_disc_within_radius(
                max_principal_aperture_radius_to_trow_photons_on);

        Vec3 direction_on_principal_aperture = random::draw_point_on_sphere(
            &thread_local_prng,
            zenith_picker,
            azimuth_picker);

        Photon ph = get_photon_given_pos_and_angle_on_principal_aperture(
            pos_on_principal_aperture,
            direction_on_principal_aperture);

        // propagate photon
        PropagationEnvironment my_env;
        my_env.root_frame = scenery;
        my_env.prng = &thread_local_prng;
        Propagator(&ph, my_env);

        sensor::FindSensorByFrame sensor_finder(
            ph.final_intersection().object(),
            &plenoscope->light_field_channels->by_frame);

        if (sensor_finder.is_absorbed_by_known_sensor) {
            // remember photon
            CalibrationPhotonResult result;
            result.reached_sensor = true;
            result.lixel_id = sensor_finder.final_sensor->id;
            result.x_pos_on_principal_aperture =
                pos_on_principal_aperture.x;
            result.y_pos_on_principal_aperture =
                pos_on_principal_aperture.y;
            result.x_tilt_vs_optical_axis =
                direction_on_principal_aperture.x;
            result.y_tilt_vs_optical_axis =
                direction_on_principal_aperture.y;
            result.relative_time_of_flight = ph.time_of_flight();
            photon_results[i] = result;
        } else {
            CalibrationPhotonResult result;
            result.reached_sensor = false;
            photon_results[i] = result;
        }
}


void Calibrator::run_calibration() {
    cout << "Plenoscope Calibrator: propagating ";
    cout << double(num_photons)/1.0e6 << "e6 photons\n";
    photon_results.resize(config.photons_per_block);
    for (unsigned int j = 0; j < config.num_blocks; j++) {
        cout << j+1 << " of " << config.num_blocks << "\n";
        fill_calibration_block_to_table();
        lixel_statistics_filler.fill_in_block(photon_results);
    }
}

void Calibrator::write_lixel_statistics(const std::string &path)const {
    vector<LixelStatistic> lixel_statistics =
        lixel_statistics_filler.get_lixel_statistics();
    write(lixel_statistics, path);
}

std::string Calibrator::str()const {
    std::stringstream out;
    out << "Light_Field_Calibration__\n";
    std::stringstream tab;
    tab << "number of photons........................ ";
    tab << num_photons << "\n";
    tab << "principal aperture illumunation radius... ";
    tab << max_principal_aperture_radius_to_trow_photons_on << "m\n";
    tab << "principal aperture illumunation angle.... ";
    tab << rad2deg(max_tilt_vs_optical_axis_to_throw_photons_in) << "deg\n";
    tab << "distance for photon to travel before\n";
    tab << "reaching principal aperture plane........ ";
    tab << distance_to_travel_before_intersecting_principal_aperture << "m\n";
    tab << "photon wavelength........................ ";
    tab << wavelenght << "m\n";
    out << txt::place_first_infront_of_each_new_line_of_second(
        "  ",
        tab.str());
    return out.str();
}

lixel_statistics_filler(
    &plenoscope->light_field_sensor_geometry,
    &_calib_config)
*/

}  // namespace calibration
}  // namespace plenoscope
