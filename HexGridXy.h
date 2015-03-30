//=================================
// include guard
#ifndef __HexGridXy_H_INCLUDED__
#define __HexGridXy_H_INCLUDED__

//=================================
// forward declared dependencies

//=================================
// included dependencies
#include "Vector3D.h"
#include <iostream> 
#include <string>
#include <sstream>
#include <vector>

//=================================
class HexGridXy{

	Vector3D unit_hex_a;
	Vector3D unit_hex_b;

	std::vector<Vector3D> grid;
public:
	HexGridXy(const double outer_radius, const double spacing);
	HexGridXy(const double outer_radius, const double inner_radius ,const double spacing);
	std::vector<Vector3D> get_grid()const; 
	std::string get_print()const;
private:
	void init_unit_vectors_hex_grid_with_length(const double spacing);
	void init_grid(
		const double outer_radius, 
		const double inner_radius,
		const double spacing
	);
};
#endif // __HexGridXy_H_INCLUDED__ 
