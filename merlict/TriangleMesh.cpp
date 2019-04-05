#include "merlict/TriangleMesh.h"
#include <sstream>
#include <fstream>
#include <iostream>
#include <regex>

namespace merlict {

TriangleMesh read_triangle_mesh(const std::string& path) {
    TriangleMesh mesh;
    std::ifstream fin(path.c_str());
    if (!fin.is_open()) {
        std::stringstream info;
        info << "Unable to read object-file '" << path << "'.";
        throw std::runtime_error(info.str());
    }

    std::regex off_head_regex("^OFF");
    std::regex num_elements_regex(
        "^"
        "([0-9]*)\\s+"
        "([0-9]*)\\s+"
        "([0-9]*)");
    std::regex vertex_regex(
        "^(-?[0-9.]+(?:(e|E)-?[0-9]+)?)\\s+"
        "(-?[0-9.]+(?:(e|E)-?[0-9]+)?)\\s+"
        "(-?[0-9.]+(?:(e|E)-?[0-9]+)?)");
    std::regex face_regex(
        "^3"
        "\\s+([0-9]+)"
        "\\s+([0-9]+)"
        "\\s+([0-9]+)");

    std::smatch match;
    std::string line;
    std::getline(fin, line);

    if (!std::regex_search(line, match, off_head_regex))
        throw std::invalid_argument("First line is not 'OFF'");

    std::getline(fin, line);
    while (line.size() == 0 || line.at(0) == '#')
        std::getline(fin, line);

    if (!std::regex_search(line, match, num_elements_regex))
        throw std::invalid_argument("Need at least num_vertices and num_faces");

    const int64_t __num_vertices = std::stoi(match[1]);
    if (__num_vertices < 0)
        throw std::invalid_argument("Expect num_vertices to be positive");
    const uint64_t num_vertices = __num_vertices;

    const int64_t __num_faces = std::stoi(match[2]);
    if (__num_faces < 0)
        throw std::invalid_argument("Expect num_faces to be positive");
    const uint64_t num_faces = __num_faces;

    std::getline(fin, line);
    while (line.size() == 0 || line.at(0) == '#')
        std::getline(fin, line);

    for (uint64_t vertex_idx = 0; vertex_idx < num_vertices; vertex_idx++) {
        if (!std::regex_search(line, match, vertex_regex))
            throw std::invalid_argument("Vertex line is evil");
        mesh.vertices.push_back(
            {
                std::stof(match[1]),
                std::stof(match[3]),
                std::stof(match[5])
            });
        std::getline(fin, line);
    }

    while (line.size() == 0 || line.at(0) == '#')
        std::getline(fin, line);

    for (uint64_t face_idx = 0; face_idx < num_faces; face_idx++) {
        if (!std::regex_search(line, match, face_regex))
            throw std::invalid_argument("face line is evil");
        uint32_t fx, fy, fz;
        fx = std::stoi(match[1]);
        fy = std::stoi(match[2]);
        fz = std::stoi(match[3]);
        mesh.faces.push_back({fx, fy, fz});
        std::getline(fin, line);
    }

    fin.close();
    return mesh;
}


void write_triangle_mesh(const TriangleMesh& mesh, const std::string& path) {
    std::ofstream fout(path.c_str());
    if (!fout.is_open()) {
        std::stringstream info;
        info << "Unable to write object-file '" << path << "'.";
        throw std::runtime_error(info.str());
    }
    fout << "OFF\n";
    fout << mesh.vertices.size() << " " << mesh.faces.size() << " 0\n";
    fout << "\n";
    for (uint64_t vidx = 0; vidx < mesh.vertices.size(); vidx++) {
        fout << mesh.vertices.at(vidx).at(0) << " ";
        fout << mesh.vertices.at(vidx).at(1) << " ";
        fout << mesh.vertices.at(vidx).at(2) << "\n";
    }
    fout << "\n";
    for (uint64_t fidx = 0; fidx < mesh.faces.size(); fidx++) {
        fout << "3 ";
        fout << mesh.faces.at(fidx).at(0) << " ";
        fout << mesh.faces.at(fidx).at(1) << " ";
        fout << mesh.faces.at(fidx).at(2) << "\n";
    }
    fout.close();
}


Vec3 vertex_to_vec3(const TriangleMesh &mesh, const uint32_t i) {
    return Vec3(
        mesh.vertices.at(i)[0],
        mesh.vertices.at(i)[1],
        mesh.vertices.at(i)[2]);
}


Vec3 mean(const TriangleMesh &mesh, const std::vector<uint32_t> &vidxs) {
    Vec3 pos(0., 0., 0.);
    for (const uint32_t vidx : vidxs)
        pos = pos + vertex_to_vec3(mesh, vidx);
    pos = pos*(1./vidxs.size());
    return pos;
}

Vec3 min(const TriangleMesh &mesh, const std::vector<uint32_t> &vidxs) {
    double x = 0.;
    double y = 0.;
    double z = 0.;
    for (const uint32_t vidx : vidxs) {
        Vec3 npos = vertex_to_vec3(mesh, vidx);
        x = npos.x < x ? npos.x : x;
        y = npos.y < y ? npos.y : y;
        z = npos.z < z ? npos.z : z;
    }
    return Vec3(x, y, z);
}

Vec3 max(const TriangleMesh &mesh, const std::vector<uint32_t> &vidxs) {
    double x = 0.;
    double y = 0.;
    double z = 0.;
    for (const uint32_t vidx : vidxs) {
        Vec3 npos = vertex_to_vec3(mesh, vidx);
        x = npos.x > x ? npos.x : x;
        y = npos.y > y ? npos.y : y;
        z = npos.z > z ? npos.z : z;
    }
    return Vec3(x, y, z);
}

uint32_t octant(const Vec3& v) {
    //
    //                       Z
    //                       |
    //                    ___|______________
    //                   /   |    /        /
    //                 /    3|  /    1   / |
    //               /_______|/________/   |
    //             /        /        /|  1 |
    //           /    2   /    0   /  |    /
    //         /________/________/    |  / |
    //        |        |        |  0  |/________Y
    //        |    2   |    0   |    /|  5 |
    //        |        |        |  /  |    /
    //        |________|________|/    |  /
    //        |       /|        |  4  |/
    //        |   6 /  |    4   |    /
    //        |   /    |        |  /
    //        |_X______|________|/
    //
    //
    const uint32_t x = v.x >= 0. ? 0 : 1;
    const uint32_t y = v.y >= 0. ? 0 : 1;
    const uint32_t z = v.z >= 0. ? 0 : 1;
    return x*1 + y*2 + z*4;
}

OctTree::OctTree(const TriangleMesh &m) {
    std::vector<uint32_t> vidxs;
    for (uint32_t i = 0; i < m.vertices.size(); i++)
        vidxs.push_back(i);
    offset = mean(m, vidxs);
    for (uint32_t i = 0; i < m.vertices.size(); i++)
        mesh.vertices.push_back({
            m.vertices[i][0] - static_cast<float>(offset.x),
            m.vertices[i][1] - static_cast<float>(offset.y),
            m.vertices[i][2] - static_cast<float>(offset.z)});
    mesh.faces = m.faces;
    lower_corner = min(mesh, vidxs);
    upper_corner = max(mesh, vidxs);
}

double param_ray_x_plane(const Ray &ray, const double px) {
    return (px - ray.support().x)/ray.direction().x;
}

double param_ray_y_plane(const Ray &ray, const double py) {
    return (py - ray.support().y)/ray.direction().y;
}

double param_ray_z_plane(const Ray &ray, const double pz) {
    return (pz - ray.support().z)/ray.direction().z;
}

bool ray_intersects_voxel(
    const Ray &ray,
    const Vec3 &upper_corner,
    const Vec3 &lower_corner) {
    //
    //          c3  +------------+ c1
    //            / |          / |
    //          /   |        /   |
    //        /     |      /     |
    //    c2 +-----------+ c0    |
    //       |      |    |       |
    //       |   c7 +----|-------+ c5
    //       |    /      |     /
    //       |  /        |   /
    //       |/          | /
    //    c6 +-----------+ c4
    //
    const Vec3 u = upper_corner;
    const Vec3 l = lower_corner;

    if (ray.direction().x != 0.) {
        const double rxut = param_ray_x_plane(ray, u.x);
        if (rxut >= 0.) {
            const Vec3 rxu = ray.position_at(rxut);
            if (rxu.y >= l.y && rxu.y < u.y && rxu.z >= l.z && rxu.z < u.z)
                return true;
        }
        const double rxlt = param_ray_x_plane(ray, l.x);
        if (rxlt >= 0.) {
            const Vec3 rxl = ray.position_at(rxlt);
            if (rxl.y >= l.y && rxl.y < u.y && rxl.z >= l.z && rxl.z < u.z)
                return true;
        }
    }
    if (ray.direction().y != 0.) {
        const double ryut = param_ray_y_plane(ray, u.y);
        if (ryut >= 0.) {
            const Vec3 ryu = ray.position_at(ryut);
            if (ryu.x >= l.x && ryu.x < u.x && ryu.z >= l.z && ryu.z < u.z)
                return true;
        }
        const double rylt = param_ray_y_plane(ray, l.y);
        if (rylt >= 0.) {
            const Vec3 ryl = ray.position_at(rylt);
            if (ryl.x >= l.x && ryl.x < u.x && ryl.z >= l.z && ryl.z < u.z)
                return true;
        }
    }
    if (ray.direction().z != 0.) {
        const double rzut = param_ray_z_plane(ray, u.z);
        if (rzut >= 0.) {
            const Vec3 rzu = ray.position_at(rzut);
            if (rzu.x >= l.x && rzu.x < u.x && rzu.y >= l.y && rzu.y < u.y)
                return true;
        }
        const double rzlt = param_ray_z_plane(ray, l.z);
        if (rzlt >= 0.) {
            const Vec3 rzl = ray.position_at(rzlt);
            if (rzl.x >= l.x && rzl.x < u.x && rzl.y >= l.y && rzl.y < u.y)
                return true;
        }
    }
    return false;
}

}  // namespace merlict
