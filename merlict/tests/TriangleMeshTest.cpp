// Copyright 2014 Sebastian A. Mueller
#include "catch.hpp"
#include <iostream>
#include <vector>
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

TEST_CASE("TriangleMeshTest: to_Vec3", "[merlict]") {
    ml::TriangleMesh m;
    m.vertices.push_back({1., 2., 3.});

    ml::Vec3 a = ml::vertex_to_vec3(m, 0);

    CHECK(a.x == 1.);
    CHECK(a.y == 2.);
    CHECK(a.z == 3.);
}

TEST_CASE("TriangleMeshTest: mean_pos", "[merlict]") {
    ml::TriangleMesh m;
    m.vertices.push_back({1., 0., 0.});
    m.vertices.push_back({0., 1., 0.});
    m.vertices.push_back({0., 0., 1.});

    std::vector<uint32_t> vidxs;
    for (uint32_t i = 0; i < m.vertices.size(); i++) vidxs.push_back(i);

    ml::Vec3 mean = ml::mean(m, vidxs);

    CHECK(mean.x == 1./3.);
    CHECK(mean.y == 1./3.);
    CHECK(mean.z == 1./3.);
}

TEST_CASE("TriangleMeshTest: min_max_pos", "[merlict]") {
    ml::TriangleMesh m;
    m.vertices.push_back({1., 0., 0.});
    m.vertices.push_back({0., 1., 0.});
    m.vertices.push_back({0., 0., 1.});
    m.vertices.push_back({0., 0., -1.});

    std::vector<uint32_t> vidxs;
    for (uint32_t i = 0; i < m.vertices.size(); i++) vidxs.push_back(i);

    ml::Vec3 min = ml::min(m, vidxs);
    CHECK(min.x == 0.);
    CHECK(min.y == 0.);
    CHECK(min.z == -1.);

    ml::Vec3 max = ml::max(m, vidxs);
    CHECK(max.x == 1.);
    CHECK(max.y == 1.);
    CHECK(max.z == 1.);
}

TEST_CASE("TriangleMeshTest: octant", "[merlict]") {
    CHECK(ml::octant(ml::Vec3( 1.,  1.,  1.)) == 0);
    CHECK(ml::octant(ml::Vec3(-1.,  1.,  1.)) == 1);
    CHECK(ml::octant(ml::Vec3( 1., -1.,  1.)) == 2);
    CHECK(ml::octant(ml::Vec3(-1., -1.,  1.)) == 3);
    CHECK(ml::octant(ml::Vec3( 1.,  1., -1.)) == 4);
    CHECK(ml::octant(ml::Vec3(-1.,  1., -1.)) == 5);
    CHECK(ml::octant(ml::Vec3( 1., -1., -1.)) == 6);
    CHECK(ml::octant(ml::Vec3(-1., -1., -1.)) == 7);
}

TEST_CASE("TriangleMeshTest: OctTree", "[merlict]") {
    ml::TriangleMesh m;
    m.vertices.push_back({1., 0., 0.});
    m.vertices.push_back({0., 1., 0.});
    m.vertices.push_back({0., 0., 1.});

    ml::OctTree tree(m);

    CHECK(tree.offset.x == 1./3.);
    CHECK(tree.offset.y == 1./3.);
    CHECK(tree.offset.z == 1./3.);

    std::vector<uint32_t> vidxs;
    for (uint32_t i = 0; i < tree.mesh.vertices.size(); i++) vidxs.push_back(i);
    ml::Vec3 mean = ml::mean(tree.mesh, vidxs);

    CHECK(mean.x == Approx(0.).margin(1e-7));
    CHECK(mean.y == Approx(0.).margin(1e-7));
    CHECK(mean.z == Approx(0.).margin(1e-7));
}

TEST_CASE("TriangleMeshTest: ray_intersects_voxel_on_edge", "[merlict]") {
    const ml::Vec3 upper_corner(1., 1., 1.);
    const ml::Vec3 lower_corner(-1., -1., -1.);

    CHECK(ml::ray_intersects_voxel(
        ml::Ray(ml::Vec3(-1, -10, 0), ml::Vec3(0, 1, 0)),
        upper_corner,
        lower_corner));
    CHECK(!ml::ray_intersects_voxel(
        ml::Ray(ml::Vec3(+1, -10, 0), ml::Vec3(0, 1, 0)),
        upper_corner,
        lower_corner));

    CHECK(ml::ray_intersects_voxel(
        ml::Ray(ml::Vec3(-10, -1, 0), ml::Vec3(1, 0, 0)),
        upper_corner,
        lower_corner));
    CHECK(!ml::ray_intersects_voxel(
        ml::Ray(ml::Vec3(-10, +1, 0), ml::Vec3(1, 0, 0)),
        upper_corner,
        lower_corner));

    CHECK(ml::ray_intersects_voxel(
        ml::Ray(ml::Vec3(-10, 0, -1), ml::Vec3(1, 0, 0)),
        upper_corner,
        lower_corner));
    CHECK(!ml::ray_intersects_voxel(
        ml::Ray(ml::Vec3(-10, 0, +1), ml::Vec3(1, 0, 0)),
        upper_corner,
        lower_corner));
}

TEST_CASE("TriangleMeshTest: ray_intersects_voxel_hit_and_miss", "[merlict]") {
    const ml::Vec3 upper_corner(1., 1., 1.);
    const ml::Vec3 lower_corner(-1., -1., -1.);

    CHECK(!ml::ray_intersects_voxel(
        ml::Ray(ml::Vec3(-2, -10, 0), ml::Vec3(0, 1, 0)),
        upper_corner,
        lower_corner));
    CHECK(ml::ray_intersects_voxel(
        ml::Ray(ml::Vec3(0, -10, 0), ml::Vec3(0, 1, 0)),
        upper_corner,
        lower_corner));
    CHECK(!ml::ray_intersects_voxel(
        ml::Ray(ml::Vec3(+2, -10, 0), ml::Vec3(0, 1, 0)),
        upper_corner,
        lower_corner));

    CHECK(!ml::ray_intersects_voxel(
        ml::Ray(ml::Vec3(-10, -2, 0), ml::Vec3(1, 0, 0)),
        upper_corner,
        lower_corner));
    CHECK(ml::ray_intersects_voxel(
        ml::Ray(ml::Vec3(-10, 0, 0), ml::Vec3(1, 0, 0)),
        upper_corner,
        lower_corner));
    CHECK(!ml::ray_intersects_voxel(
        ml::Ray(ml::Vec3(-10, +2, 0), ml::Vec3(1, 0, 0)),
        upper_corner,
        lower_corner));

    CHECK(!ml::ray_intersects_voxel(
        ml::Ray(ml::Vec3(-10, 0, -2), ml::Vec3(1, 0, 0)),
        upper_corner,
        lower_corner));
    CHECK(ml::ray_intersects_voxel(
        ml::Ray(ml::Vec3(-10, 0, 0), ml::Vec3(1, 0, 0)),
        upper_corner,
        lower_corner));
    CHECK(!ml::ray_intersects_voxel(
        ml::Ray(ml::Vec3(-10, 0, +2), ml::Vec3(1, 0, 0)),
        upper_corner,
        lower_corner));
}

TEST_CASE("TriangleMeshTest: ray_intersects_voxel_causality", "[merlict]") {
    const ml::Vec3 upper_corner(1., 1., 1.);
    const ml::Vec3 lower_corner(-1., -1., -1.);

    CHECK(ml::ray_intersects_voxel(
        ml::Ray(ml::Vec3(-10, 0, 0), ml::Vec3(1, 0, 0)),
        upper_corner,
        lower_corner));
    CHECK(!ml::ray_intersects_voxel(
        ml::Ray(ml::Vec3(+10, 0, 0), ml::Vec3(1, 0, 0)),
        upper_corner,
        lower_corner));

    CHECK(ml::ray_intersects_voxel(
        ml::Ray(ml::Vec3(0, -10, 0), ml::Vec3(0, 1, 0)),
        upper_corner,
        lower_corner));
    CHECK(!ml::ray_intersects_voxel(
        ml::Ray(ml::Vec3(0, +10, 0), ml::Vec3(0, 1, 0)),
        upper_corner,
        lower_corner));

    CHECK(ml::ray_intersects_voxel(
        ml::Ray(ml::Vec3(0, 0, -10), ml::Vec3(0, 0, 1)),
        upper_corner,
        lower_corner));
    CHECK(!ml::ray_intersects_voxel(
        ml::Ray(ml::Vec3(0, 0, +10), ml::Vec3(0, 0, 1)),
        upper_corner,
        lower_corner));
}