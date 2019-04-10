// Copyright 2014 Sebastian A. Mueller
#include <numeric>
#include "merlict/tests/catch.hpp"
#include "merlict_corsika/PhotonFactory.h"
#include "merlict_corsika/eventio.h"
#include "merlict/merlict.h"
namespace ml = merlict;


TEST_CASE("EventIoPhotonFactoryTest: intersection_point_on_ground", "[merlict]") {
    // compare the input ground intersection point with the actual intersection
    // point of the merlict photons when they are absorbed on the ground.
    ml::random::Mt19937 prng(0);

    for (float x = -1e4; x < 1e4; x = x+1495.0) {
        for (float y = -1e4; y < 1e4; y = y+1495.0) {
            for (float cx = -0.5; cx < 0.5; cx = cx+0.11) {
                for (float cy = -0.5; cy < 0.5; cy = cy+0.11) {
                    const std::array<float, 8> corsika_photon =
                    {    x,   y,   cx,  cy,  0.0,  1e5, 1.0,   433};
                    //   x    y    xcos ycos time  zem  weight lambda
                    //   cm   cm   1    1    ns    cm   1      nm
                    const unsigned int id = 1337;
                    ml::random::FakeConstant fake_prng(0.0);

                    ml::EventIoPhotonFactory cpf(
                        corsika_photon,
                        id,
                        &fake_prng);

                    std::vector<ml::Photon> photons;
                    photons.push_back(cpf.make_photon());

                    // propagate merlict photons down to ground
                    ml::Scenery scenery;
                    ml::Disc* ground = scenery.root.add<ml::Disc>();
                    ground->set_name_pos_rot(
                        "ground",
                        ml::VEC3_ORIGIN,
                        ml::ROT3_UNITY);
                    scenery.colors.add("ground_color", ml::COLOR_GRAY);
                    const unsigned int ground_sensor_id = 0;
                    ground->outer_color = scenery.colors.get("ground_color");
                    ground->inner_color = scenery.colors.get("ground_color");
                    ground->set_radius(1e3);

                    ml::sensor::Sensor sensor(ground_sensor_id, ground);
                    std::vector<ml::sensor::Sensor*> sensor_vec = {&sensor};
                    ml::sensor::Sensors sensor_list(sensor_vec);

                    scenery.root.init_tree_based_on_mother_child_relations();

                    // propagation settings
                    ml::PropagationConfig settings;

                    // photon propagation down to the ground
                    ml::propagate_photons_in_frame_with_config(
                        &photons, &scenery.root, &settings, &prng);

                    // detect photons in ground sensor
                    sensor_list.clear_history();
                    sensor_list.assign_photons(&photons);

                    REQUIRE(sensor.photon_arrival_history.size() == 1u);

                    CHECK(x*1e-2 == Approx(sensor.photon_arrival_history[0].x_intersect).margin(1e-6));
                    CHECK(y*1e-2 == Approx(sensor.photon_arrival_history[0].y_intersect).margin(1e-6));
                }
            }
        }
    }
}

TEST_CASE("EventIoPhotonFactoryTest: wavelength_sign", "[merlict]") {
    ml::random::FakeConstant prng(0.0);
    for (int l = 1; l < 1337; l++) {
        const float sign = (l%2 == 0) ? +1.0: -1.0;
        const float lambda = static_cast<float>(l)*sign;
        const std::array<float, 8> corsika_photon = {
             1.2, 3.4, 0.0, 0.0, 1e-9, 1e5, 0.999, lambda};
        //   x    y    xcos ycos time  zem  weight lambda
        //   cm   cm   1    1    ns    cm   1      nm

        const int id = 1337;
        ml::EventIoPhotonFactory cpf(corsika_photon, id, &prng);
        REQUIRE(fabs(lambda) == Approx(cpf.wavelength()*1e9).margin(1e-2));
        REQUIRE(cpf.wavelength() > 0.0);
    }
}

TEST_CASE("EventIoPhotonFactoryTest: convert_photons", "[merlict]") {
    const std::array<float, 8> corsika_photon =
        {1.2, 3.4, 0.0, 0.0, 1e-9, 1e5, 0.999, 433};
    //   x    y    xcos ycos time  zem  weight lambda
    //   cm   cm   1    1    ns    cm   1    nm

    const int id = 1337;
    ml::random::FakeConstant prng(0.0);

    ml::EventIoPhotonFactory cpf(corsika_photon, id, &prng);

    REQUIRE(cpf.passed_atmosphere());
    ml::Photon ph = cpf.make_photon();

    CHECK(ph.simulation_truth_id == id);
    CHECK(433e-9 == Approx(ph.wavelength).margin(1e-9));

    // since the x,y angles are zero, the support vector can be tested
    CHECK(0.012 == Approx(ph.support().x).margin(1e-9));
    CHECK(0.034 == Approx(ph.support().y).margin(1e-9));
    CHECK(1e3 == Approx(ph.support().z).margin(1e-9));
}

TEST_CASE("EventIoPhotonFactoryTest: execute_atmospheric_absorption", "[merlict]") {
    // In Corsika photons can have a 'weight' to represent a bunch of photons.
    // This was introduced in Corsika to save computations and disk space.
    // Corsika does not recomend to have weights above 5.
    // Merlict has no concet of weighted photon bunches.
    // We force Corsika to produce single photons, i.e. the maximum weight
    // in the Corsika output is 1.0.
    // A weight below 1.0 means, that the photon passed Corsika's atmosphere
    // simulation with a probability of this weigth.
    // In merlict we collapse this photons probability when the photon
    // is created in merlict. This is why the EventIoPhotonFactory is
    // given a PseudoRandomNumberGenerator.

    double absorbed = 0;
    double passed = 0;
    double total = 1e5;

    ml::random::Mt19937 prng(0u);

    for (double i = 0; i < total-1; i++) {
        float weight = i/total;  // from 0.0 to 1.0
        const std::array<float, 8> corsika_photon =
            {1.2, 3.4, 0.0, 0.0, 1e-9, 1e3, weight, 433};
        //   x    y    xcos ycos time  zem  weight lambda
        //   cm   cm   1    1    ns    cm   1    nm

        ml::EventIoPhotonFactory cpf(corsika_photon, 1337, &prng);

        if (cpf.passed_atmosphere())
            passed++;
        else
            absorbed++;
    }

    double passed_ratio = passed/total;
    CHECK(0.5 == Approx(passed_ratio).margin(1e-2));
}

TEST_CASE("EventIoPhotonFactoryTest: merlict_rejects_photon_weight_below_0", "[merlict]") {
    const unsigned int id = 1337;
    ml::random::Mt19937 prng(0u);
    const std::array<float, 8> corsika_photon =
        {1.2, 3.4, 0.0, 0.0, 1e-9, 1e5, -0.1, 433};

    CHECK_THROWS_AS(ml::EventIoPhotonFactory(corsika_photon, id, &prng), ml::EventIoPhotonFactory::BadPhotonWeight);
}

TEST_CASE("EventIoPhotonFactoryTest: merlict_accepts_photon_weight_equal_1", "[merlict]") {
    const unsigned int id = 1337;
    ml::random::Mt19937 prng(0u);
    const std::array<float, 8> corsika_photon =
        {1.2, 3.4, 0.0, 0.0, 1e-9, 1e5, 1.0, 433};

    CHECK_NOTHROW(ml::EventIoPhotonFactory(corsika_photon, id, &prng));
}

TEST_CASE("EventIoPhotonFactoryTest: merlict_rejects_photon_weight_above_1", "[merlict]") {
    const unsigned int id = 1337;
    ml::random::Mt19937 prng(0u);
    const std::array<float, 8> corsika_photon =
        {1.2, 3.4, 0.0, 0.0, 1e-9, 1e5, 1.1, 433};

    CHECK_THROWS_AS(ml::EventIoPhotonFactory(corsika_photon, id, &prng), ml::EventIoPhotonFactory::BadPhotonWeight);
}

TEST_CASE("EventIoPhotonFactoryTest: merlict_accepts_photon_weight_equal_0", "[merlict]") {
    const unsigned int id = 1337;
    ml::random::Mt19937 prng(0u);
    const std::array<float, 8> corsika_photon =
        {1.2, 3.4, 0.0, 0.0, 1e-9, 1e5, 0.0, 433};
    CHECK_NOTHROW(ml::EventIoPhotonFactory(corsika_photon, id, &prng));
}

TEST_CASE("EventIoPhotonFactoryTest: merlict_accepts_photon_weight_btw_0_and_1", "[merlict]") {
    const unsigned int id = 1337;
    ml::random::Mt19937 prng(0u);
    const std::array<float, 8>  corsika_photon =
        {1.2, 3.4, 0.0, 0.0, 1e-9, 1e5, 0.4455, 433};
    CHECK_NOTHROW(ml::EventIoPhotonFactory(corsika_photon, id, &prng));
}

TEST_CASE("EventIoPhotonFactoryTest: zero_weight_is_passed_on_zero_from_prng", "[merlict]") {
    const std::array<float, 8>  corsika_photon =
    {1.2, 3.4, 0.0, 0.0, 1e-9, 1e5, 0.0, 433};
    const unsigned int id = 1337;
    ml::random::FakeConstant prng(0.0);
    ml::EventIoPhotonFactory cpf(corsika_photon, id, &prng);
    CHECK(cpf.passed_atmosphere());
}

TEST_CASE("EventIoPhotonFactoryTest: relative_arrival_time_on_ground", "[merlict]") {
    float arrival_time_on_dround_in_ns = 1.0;
    const std::array<float, 8>  corsika_photon =
    {1.2, 3.4, 0.0, 0.0, arrival_time_on_dround_in_ns, 1e5, 0.4455, 433};
    const unsigned int id = 1337;
    ml::random::FakeConstant prng(0.0);
    ml::EventIoPhotonFactory cpf(corsika_photon, id, &prng);
    CHECK(cpf.relative_arrival_time_on_ground() == arrival_time_on_dround_in_ns*1e-9);
}

TEST_CASE("EventIoPhotonFactoryTest: correct_rel_time_when_intersecting_ground", "[merlict]") {
    eventio::Run corsika_file("merlict_corsika/tests/resources/telescope.dat");
    while (corsika_file.has_still_events_left()) {
        /*
        read in a corsika eventio event. we remember the relative arrival
        times in the eventio file 'relative_arrival_times_in_corsika_file' and
        compare these to the actual arrival times of the merlict 
        photons on ground.
        */
        eventio::Event event = corsika_file.next_event();

        std::vector<float> relative_arrival_times_in_corsika_file;

        std::vector<ml::Photon> photons;

        ml::random::Mt19937 prng(0u);

        for (unsigned int id = 0; id < event.photons.size(); id++) {
            ml::EventIoPhotonFactory factory(
                event.photons.at(id),
                id,
                &prng);
            relative_arrival_times_in_corsika_file.push_back(
                factory.relative_arrival_time_on_ground());
            photons.push_back(factory.make_photon());
        }

        // propagate merlict photons down to ground
        ml::Scenery scenery;
        ml::Disc* ground = scenery.root.add<ml::Disc>();
        ground->set_name_pos_rot("ground", ml::VEC3_ORIGIN, ml::ROT3_UNITY);
        scenery.colors.add("ground_color", ml::COLOR_GRAY);
        const unsigned int ground_sensor_id = 0;
        ground->outer_color = scenery.colors.get("ground_color");
        ground->inner_color = scenery.colors.get("ground_color");
        ground->set_radius(1e7);

        ml::sensor::Sensor sensor(ground_sensor_id, ground);
        std::vector<ml::sensor::Sensor*> sensor_vec = {&sensor};
        ml::sensor::Sensors sensor_list(sensor_vec);

        scenery.root.init_tree_based_on_mother_child_relations();

        // propagation settings
        ml::PropagationConfig settings;

        // photon propagation
        ml::propagate_photons_in_frame_with_config(
            &photons, &scenery.root, &settings, &prng);

        // detect photons in sensors
        sensor_list.clear_history();
        sensor_list.assign_photons(&photons);

        double mean_arrival_time = ml::sensor::arrival_time_mean(
            sensor.photon_arrival_history);

        for (
            unsigned int row = 0;
            row < sensor.photon_arrival_history.size();
            row++
        )
            sensor.photon_arrival_history[row].arrival_time -=
                mean_arrival_time;

        double mean_time_of_corsika_photons = std::accumulate(
            relative_arrival_times_in_corsika_file.begin(),
            relative_arrival_times_in_corsika_file.end(),
            0.0)/static_cast<double>(
                relative_arrival_times_in_corsika_file.size());

        for (unsigned int row = 0;
            row < sensor.photon_arrival_history.size();
            row++
        )
            sensor.photon_arrival_history[row].arrival_time +=
                mean_time_of_corsika_photons;

        // for each photon we compare the relative arrival
        // time written in the in the eventio file with the actual arrival time
        // of the merlict photon which ran down to the ground.
        for (
            unsigned int i = 0;
            i < sensor.photon_arrival_history.size();
            i++
        ) {
            unsigned int id =
                sensor.photon_arrival_history[i].simulation_truth_id;
            CHECK(relative_arrival_times_in_corsika_file.at(id) == Approx(sensor.photon_arrival_history[i].arrival_time).margin(1e-11));
        }

        // relative_arrival_times_in_corsika_file
    }
}
