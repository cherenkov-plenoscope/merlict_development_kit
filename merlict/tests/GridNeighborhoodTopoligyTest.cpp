// Copyright 2014 Sebastian A. Mueller
#include "catch.hpp"
#include "merlict/scenery/geometry/HexGridAnnulus.h"
#include "merlict/scenery/geometry/GridNeighborhoodTopoligy.h"
namespace ml = merlict;


TEST_CASE("GridNeighborhoodTopoligyTest: init", "[merlict]") {
    double outer_R = 1.0;
    double spacing = 0.2;

    ml::HexGridAnnulus grid_gen(outer_R, spacing);

    std::vector<ml::Vec3> grid = grid_gen.get_grid();

    ml::GridNeighborhoodTopoligy topo(&grid, spacing*1.1);

    std::vector<std::vector<unsigned int>> neighbors =
        topo.get_neighbor_relations();

    /*unsigned int i=0;
    for (std::vector<unsigned int> ne : neighbors) {
        std::cout << i << ") ";
        for (unsigned int n : ne) {
            std::cout << n << ", ";
        }
        std::cout << "\n";
        i++;
    }

    for (int i = 0; i < grid.size(); ++i)
    {
       std::cout << grid[i].str() << "\n";
    }*/
}
