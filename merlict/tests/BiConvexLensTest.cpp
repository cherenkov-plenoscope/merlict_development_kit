// Copyright 2014 Sebastian A. Mueller
#include <vector>
#include "catch.hpp"
#include "merlict/merlict.h"
#include <iostream>
namespace ml = merlict;


struct BiConvexLensTest {
    ml::Scenery scenery;
    ml::PropagationConfig settings;
    ml::random::Mt19937 prng;
    ml::PropagationEnvironment env;
    ml::sensor::Sensors sensor_list;

    BiConvexLensTest() {
        set_up_settings();
        set_up_test_bench();

        prng.set_seed(0u);

        env.root_frame = &scenery.root;
        env.config = &settings;
        env.prng = &prng;
    }

    void set_up_settings() {
        settings.max_num_interactions_per_photon = 5;
    }

    void set_up_test_bench() {
        ml::Frame* test_bench = scenery.root.add<ml::Frame>();
        test_bench->set_name_pos_rot(
            "BiConvexLens_test_world",
            ml::VEC3_ORIGIN,
            ml::ROT3_UNITY);

        ml::lens_maker::Config cfg;
        cfg.focal_length = 1.0;
        cfg.aperture_radius = 0.125;
        cfg.refractive_index = 1.49;

        ml::BiConvexLens* lens = test_bench->add<ml::BiConvexLens>();
        lens->set_name_pos_rot(
            "little_lens",
            ml::VEC3_ORIGIN,
            ml::ROT3_UNITY);

        scenery.colors.add("lens_color", ml::Color(255, 128, 128));
        scenery.functions.add(
            "refraction_vs_wavelength",
            ml::function::Func1(
                {
                    {200e-9, 1.49},
                    {1200e-9, 1.49}
                }));

        lens->outer_color = scenery.colors.get("lens_color");
        lens->inner_color = scenery.colors.get("lens_color");
        lens->inner_refraction = scenery.functions.get("refraction_vs_wavelength");
        lens->set_curvature_radius_and_aperture_radius(
            ml::lens_maker::get_curvature_radius(cfg),
            cfg.aperture_radius);
        //-------------------------
        ml::Disc* image_sensor = test_bench->add<ml::Disc>();
        image_sensor->set_name_pos_rot(
            "sensor_disc",
            ml::Vec3(0.0, 0.0, -1.0),
            ml::ROT3_UNITY);

        scenery.colors.add("sensor_color", ml::Color(128, 255, 128));
        image_sensor->outer_color = scenery.colors.get("sensor_color");
        image_sensor->inner_color = scenery.colors.get("sensor_color");
        image_sensor->set_radius(cfg.aperture_radius);

        scenery.sensors.add(0, image_sensor);
        sensor_list.init(scenery.sensors.sensors);
        //-------------------------

        scenery.root.init_tree_based_on_mother_child_relations();
    }
};

TEST_CASE("BiConvexLensTest: send_photon_frontal_into_lens", "[merlict]") {
    BiConvexLensTest lt;
    unsigned int total_propagations = 1e4;
    unsigned int num_photons_reaching_sensor_disc = 0;
    for (unsigned int i = 0; i < total_propagations; i++) {
        ml::Photon blue_photon(
            ml::Vec3(0.0, 0.0, 1.0),
            ml::Vec3(0.0, 0.0, -1.0),
            433e-9);
        ml::Propagator(&blue_photon, lt.env);
        // blue_photon.propagate_in(env);

        if (2.0 == blue_photon.accumulated_distance())
            num_photons_reaching_sensor_disc++;
    }

    CHECK(0.97 == Approx(static_cast<double>(num_photons_reaching_sensor_disc)/static_cast<double>(total_propagations)).margin(5.0e-2));
}

TEST_CASE("BiConvexLensTest: send_photons_frontal_into_lens_with_offset", "[merlict]") {
    BiConvexLensTest lt;

    // light source
    unsigned int num_photons_emitted = 1e4;
    ml::random::Mt19937 prng(0);
    std::vector<ml::Photon> photons =
        ml::photon_source::parallel_towards_z_from_xy_disc(
            0.125,
            num_photons_emitted,
            &prng);

    ml::HomTra3 Trafo;
    Trafo.set_transformation(
        ml::Rot3(0.0, -ml::deg2rad(180.0), 0.0),
        ml::Vec3(0.0, 0.0, 1.0));

    for (size_t i = 0; i < photons.size(); i++)
        photons.at(i).transform(&Trafo);

    // photon propagation
    ml::propagate_photons_in_frame_with_config(
        &photons,
        lt.env.root_frame,
        lt.env.config,
        &prng);

    // detect photons in sensors
    lt.sensor_list.clear_history();
    lt.sensor_list.assign_photons(&photons);

    REQUIRE(lt.sensor_list.by_frame.size() == 1);

    ml::sensor::Sensor* sen = lt.sensor_list.at(0);

    CHECK(1.5e-3 == Approx(ml::sensor::point_spread_std_dev(sen->photon_arrival_history)).margin(1e-3));

    CHECK(1.0 == Approx(static_cast<double>(sen->photon_arrival_history.size())/static_cast<double>(num_photons_emitted)).margin(10e-2));
}
