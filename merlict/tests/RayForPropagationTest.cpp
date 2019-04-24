// Copyright 2014 Sebastian A. Mueller
#include "catch.hpp"
#include "merlict/merlict.h"
namespace ml = merlict;

TEST_CASE("RayForPropagationTest: default_ctor", "[merlict]") {
    ml::RayForPropagation prop_ray;
    CHECK(prop_ray.simulation_truth_id == 0);

    CHECK(prop_ray.support().x == 0.);
    CHECK(prop_ray.support().y == 0.);
    CHECK(prop_ray.support().z == 0.);

    CHECK(prop_ray.direction().x == 0.);
    CHECK(prop_ray.direction().y == 0.);
    CHECK(prop_ray.direction().z == 1.);
}


TEST_CASE("RayForPropagationTest: Constructor_using_vectors", "[merlict]") {
    ml::Vec3 support(0.0, 0.0, 1.0);
    ml::Vec3 direction(0.0, 0.0, 1.0);
    ml::RayForPropagation prop_ray(support, direction);
    prop_ray.simulation_truth_id = 1337;

    CHECK(prop_ray.support() == support);
    CHECK(prop_ray.direction() == direction);
    CHECK(prop_ray.num_interactions() == 1u);
    // creation itselt
    CHECK(prop_ray.simulation_truth_id == 1337);
}

TEST_CASE("RayForPropagationTest: direction_normalized", "[merlict]") {
    ml::Vec3 support(0.0, 0.0, 1.0);
    ml::Vec3 direction(0.0, 0.0, 1.0);
    ml::RayForPropagation prop_ray(support, direction*42.0);
    CHECK(prop_ray.direction().norm() == 1.0);
}

TEST_CASE("RayForPropagationTest: distance_passed_after_creation", "[merlict]") {
    ml::Vec3 support(0.0, 0.0, 1.0);
    ml::Vec3 direction(0.0, 0.0, 1.0);
    ml::RayForPropagation prop_ray(support, direction);
    CHECK(prop_ray.accumulated_distance() == 0.0);
}

TEST_CASE("RayForPropagationTest: default_simulation_truth_id", "[merlict]") {
    ml::Vec3 support(0.0, 0.0, 1.0);
    ml::Vec3 direction(0.0, 0.0, 1.0);
    ml::RayForPropagation prop_ray(support, direction);
    CHECK(prop_ray.simulation_truth_id == ml::DEFAULT_SIMULATION_TRUTH);
}
