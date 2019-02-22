// Copyright 2014 Sebastian A. Mueller
#include <vector>
#include "gtest/gtest.h"
// #include "Cameras/FlyingCamera.h"
#include "Core/Function/Function.h"
#include "Core/Photons.h"
#include "Core/PhotonAndFrame.h"
#include "Scenery/Primitive/BiConvexLens.h"
#include "Scenery/Primitive/Disc.h"
#include "Scenery/Primitive/BiConvexLensHexBound.h"
#include "Scenery/Geometry/LensMaker/LensMaker.h"
#include "PhotonSensor/PhotonSensor.h"
#include "Tools/AsciiIo.h"

using namespace relleums;
using std::vector;

class BiConvexLensTest : public ::testing::Test {
 protected:
    Frame test_bench;
    PropagationConfig settings;
    Random::Mt19937 prng;
    PropagationEnvironment lens_test_bench_environment;
    PhotonSensor::Sensors sensor_list;
    PhotonSensor::Sensor *sensor;
    LensMaker::Config cfg;

    BiConvexLensTest() {
        set_up_settings();
        set_up_test_bench();

        prng.set_seed(Random::ZERO_SEED);

        lens_test_bench_environment.root_frame = &test_bench;
        lens_test_bench_environment.config = &settings;
        lens_test_bench_environment.prng = &prng;
    }
    void set_up_settings() {
        settings.max_number_of_interactions_per_photon = 5;
    }
    void set_up_test_bench() {
        test_bench.set_name_pos_rot(
            "BiConvexLens_test_world",
            VEC3_ORIGIN,
            ROT3_UNITY);

        cfg.focal_length = 1.0;
        cfg.aperture_radius = 0.125;
        cfg.refractive_index = 1.49;

        BiConvexLens* lens = test_bench.append<BiConvexLens>();
        lens->set_name_pos_rot(
            "little_lens",
            VEC3_ORIGIN,
            ROT3_UNITY);

        Color* lens_color = new Color(255, 128, 128);

        Function::Func1D* refraction_vs_wavelength = new Function::Func1D(
            {
                {200e-9, 1.49},
                {1200e-9, 1.49}
            });

        lens->set_outer_color(lens_color);
        lens->set_inner_color(lens_color);
        lens->set_inner_refraction(refraction_vs_wavelength);
        lens->set_curvature_radius_and_aperture_radius(
            LensMaker::Approximation::get_curvature_radius(cfg),
            cfg.aperture_radius);
        //-------------------------
        Disc* image_sensor = test_bench.append<Disc>();
        image_sensor->set_name_pos_rot(
            "sensor_disc",
            Vec3(0.0, 0.0, -1.0),
            ROT3_UNITY);

        Color* sensor_color;
        sensor_color = new Color(128, 255, 128);

        image_sensor->set_outer_color(sensor_color);
        image_sensor->set_inner_color(sensor_color);
        image_sensor->set_radius(cfg.aperture_radius);

        sensor = new PhotonSensor::Sensor(0, image_sensor);
        std::vector<PhotonSensor::Sensor*> sensors_vector = {sensor};
        sensor_list.init(sensors_vector);
        //-------------------------

        test_bench.init_tree_based_on_mother_child_relations();
    }
    virtual ~BiConvexLensTest() {
    }
};
//----------------------------------------------------------------------
TEST_F(BiConvexLensTest, send_photon_frontal_into_lens) {
    unsigned int total_propagations = 1e4;
    unsigned int number_of_photons_reaching_sensor_disc = 0;
    for (unsigned int i = 0; i < total_propagations; i++) {
        Photon blue_photon(Vec3(0.0, 0.0, 1.0), Vec3(0.0, 0.0, -1.0), 433e-9);
        PhotonAndFrame::Propagator(&blue_photon, lens_test_bench_environment);
        // blue_photon.propagate_in(lens_test_bench_environment);

        if (2.0 == blue_photon.get_accumulative_distance())
            number_of_photons_reaching_sensor_disc++;
    }

    EXPECT_NEAR(
        0.97,
        static_cast<double>(number_of_photons_reaching_sensor_disc)/
            static_cast<double>(total_propagations),
        5.0e-2);
}
//----------------------------------------------------------------------
TEST_F(BiConvexLensTest, send_photons_frontal_into_lens_with_offset) {
    // light source
    unsigned int number_of_photons_emitted = 1e4;
    vector<Photon> photons = Photons::Source::parallel_towards_z_from_xy_disc(
        0.125,
        number_of_photons_emitted);

    HomTra3 Trafo;
    Trafo.set_transformation(
        Rot3(0.0, -deg2rad(180.0), 0.0),
        Vec3(0.0, 0.0, 1.0));

    Photons::transform_all_photons(Trafo, &photons);

    // photon propagation
    Photons::propagate_photons_in_scenery_with_settings(
        &photons,
        lens_test_bench_environment.root_frame,
        lens_test_bench_environment.config,
        &prng);

    // detect photons in sensors
    sensor_list.clear_history();
    sensor_list.assign_photons(&photons);

    EXPECT_NEAR(
        1.5e-3,
        PhotonSensor::point_spread_std_dev(sensor->photon_arrival_history),
        1e-3);

    EXPECT_NEAR(
        1.0,
        static_cast<double>(sensor->photon_arrival_history.size())/
            static_cast<double>(number_of_photons_emitted),
        10e-2);

    /*FlyingCamera free(
        lens_test_bench_environment.root_frame,
        lens_test_bench_environment.config
    );*/
}
