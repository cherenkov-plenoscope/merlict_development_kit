// Copyright 2014 Sebastian A. Mueller
#include "catch.hpp"
#include "merlict/PropagationEnvironment.h"

using namespace merlict;



TEST_CASE("PropagationEnvironmentTest: default_instance", "[merlict]") {
    PropagationEnvironment env;
    CHECK(env.root_frame == &VOID_FRAME);
    CHECK(env.config == &DEFAULT_PROPAGATIONCONFIG);
    CHECK(env.prng == &random::VOID_PRNG);
}