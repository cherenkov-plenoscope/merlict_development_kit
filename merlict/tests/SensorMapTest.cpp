// Copyright 2014 Sebastian A. Mueller
#include "catch.hpp"
#include "merlict/scenery/SensorMap.h"
namespace ml = merlict;


TEST_CASE("SensorMapTest: init_one", "[merlict]") {
    ml::SensorMap smap;
    ml::Frame hans;
    smap.add(0u, &hans);
    CHECK(1u == smap.sensors.size());
}

TEST_CASE("SensorMapTest: init_two", "[merlict]") {
    ml::SensorMap smap;
    ml::Frame hans;
    smap.add(0u, &hans);
    smap.add(1u, &hans);
    CHECK(2u == smap.sensors.size());
}

TEST_CASE("SensorMapTest: init_user_defined_ids", "[merlict]") {
    ml::SensorMap smap;
    ml::Frame hans;
    smap.add(42u, &hans);
    smap.add(1337u, &hans);
    CHECK(2u == smap.sensors.size());
}

TEST_CASE("SensorMapTest: add_first_with_id_uneq_one", "[merlict]") {
    ml::SensorMap smap;
    ml::Frame hans;
    smap.add(13u, &hans);
    CHECK_THROWS_AS(smap.add(13u, &hans), ml::ResourceMap::KeyAlreadyInUse);
}

TEST_CASE("SensorMapTest: get_by_id", "[merlict]") {
    ml::SensorMap smap;
    ml::Frame hans;
    smap.add(13u, &hans);
    smap.add(42u, &hans);
    CHECK(smap.get(13u)->id == 13u);
    CHECK(smap.get(42u)->id == 42u);
    CHECK_THROWS_AS(smap.get(37u), ml::ResourceMap::NoSuchKey);
}
