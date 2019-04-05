// Copyright 2014 Sebastian A. Mueller
#ifndef MERLICT_TRIANGLEMESH_H_
#define MERLICT_TRIANGLEMESH_H_

#include <stdint.h>
#include <string>
#include <array>
#include <vector>
#include "merlict/Vec3.h"
#include "merlict/Ray.h"

namespace merlict {

struct TriangleMesh {
    std::vector<std::array<float ,3>> vertices;
    std::vector<std::array<uint32_t, 3>> faces;
};

TriangleMesh read_triangle_mesh(const std::string& path);

void write_triangle_mesh(const TriangleMesh& mesh, const std::string& path);

Vec3 vertex_to_vec3(const TriangleMesh &mesh, const uint32_t i);

Vec3 mean(const TriangleMesh &mesh, const std::vector<uint32_t> &vidxs);

Vec3 min(const TriangleMesh &mesh, const std::vector<uint32_t> &vidxs);

Vec3 max(const TriangleMesh &mesh, const std::vector<uint32_t> &vidxs);

uint32_t octant(const Vec3& v);

struct OctTree {
    TriangleMesh mesh;
    Vec3 offset;
    Vec3 lower_corner;
    Vec3 upper_corner;
    OctTree(const TriangleMesh &m);
};

bool ray_intersects_voxel(
    const Ray &ray,
    const Vec3 &upper_corner,
    const Vec3 &lower_corner);

}  // namespace merlict

#endif  // MERLICT_TRIANGLEMESH_H_
