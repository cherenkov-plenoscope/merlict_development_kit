#include "merlict/PolygonMesh.h"
#include <sstream>
#include <fstream>
#include <iostream>
#include <regex>

namespace merlict {

PolygonMesh read_polygon_mesh(const std::string& path) {
    PolygonMesh mesh;
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


void write_polygon_mesh(const PolygonMesh& mesh, const std::string& path) {
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

}  // namespace merlict
