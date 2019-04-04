// Copyright 2014 Sebastian A. Mueller
#ifndef MERLICT_POLYGONMESH_H_
#define MERLICT_POLYGONMESH_H_

#include <stdint.h>
#include <string>
#include <array>
#include <vector>

namespace merlict {

struct PolygonMesh {
    std::vector<std::array<float ,3>> vertices;
    std::vector<std::array<uint32_t, 3>> faces;
};

PolygonMesh read_polygon_mesh(const std::string& path);

void write_polygon_mesh(const PolygonMesh& mesh, const std::string& path);

}  // namespace merlict

#endif  // MERLICT_POLYGONMESH_H_
