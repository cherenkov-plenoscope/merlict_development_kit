// Copyright 2014 Sebastian A. Mueller
#include "catch.hpp"
#include "merlict/TriangleMesh.h"
namespace ml = merlict;


TEST_CASE("TriangleMeshTest: ctor", "[merlict]") {
    ml::TriangleMesh m;
    CHECK(0 == m.vertices.size());
    CHECK(0 == m.faces.size());
}

TEST_CASE("TriangleMeshTest: read_not_existing_file", "[merlict]") {
    CHECK_THROWS_AS(
        ml::read_triangle_mesh("merlict/tests/resources/does_not_exist.off"),
        std::runtime_error);
}

TEST_CASE("TriangleMeshTest: read_off", "[merlict]") {
    ml::TriangleMesh m = ml::read_triangle_mesh(
        "merlict/tests/resources/text_on_cube.off");
    CHECK(1015u == m.vertices.size());
    CHECK(2026u == m.faces.size());

    ml::write_triangle_mesh(
        m,
        "merlict/tests/resources/back_text_on_cube.off.tmp");

    ml::TriangleMesh m_back = ml::read_triangle_mesh(
        "merlict/tests/resources/back_text_on_cube.off.tmp");

    REQUIRE(m.vertices.size() == m_back.vertices.size());
    for (uint64_t vidx = 0; vidx < m.vertices.size(); vidx++) {
        for (uint64_t dim = 0; dim < 3; dim++) {
            CHECK(
                m.vertices.at(vidx).at(dim) ==
                Approx(m_back.vertices.at(vidx).at(dim)).margin(1e-9));
        }
    }

    REQUIRE(m.faces.size() == m_back.faces.size());
    for (uint64_t fidx = 0; fidx < m.faces.size(); fidx++) {
        for (uint64_t dim = 0; dim < 3; dim++) {
            CHECK(
                m.faces.at(fidx).at(dim) ==
                Approx(m_back.faces.at(fidx).at(dim)).margin(1e-9));
        }
    }
}