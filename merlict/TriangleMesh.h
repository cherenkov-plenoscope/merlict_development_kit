// Copyright 2014 Sebastian A. Mueller
#ifndef MERLICT_TRIANGLEMESH_H_
#define MERLICT_TRIANGLEMESH_H_

#include <stdint.h>
#include <string>
#include <array>
#include <vector>

namespace merlict {

struct TriangleMesh {
    std::vector<std::array<float ,3>> vertices;
    std::vector<std::array<uint32_t, 3>> faces;
};

TriangleMesh read_triangle_mesh(const std::string& path);

void write_triangle_mesh(const TriangleMesh& mesh, const std::string& path);

}  // namespace merlict

#endif  // MERLICT_TRIANGLEMESH_H_
