//=================================
// include guard
#ifndef __SPHERECAPWITHCYLINDERBOUND_H_INCLUDED__
#define __SPHERECAPWITHCYLINDERBOUND_H_INCLUDED__

//=================================
// forward declared dependencies

//=================================
// included dependencies
#include "SphericalCapRayIntersectionEquation.h"
#include "HexagonalPrismZ.h"
#include "SurfaceWithOuterPrismBound.h"

//=================================
class SphereCapWithHexagonalBound :public SurfaceWithOuterPrismBound {
protected:	
	HexagonalPrismZ hexBounds;

	double focal_length;
	double curvature_radius;
	double outer_hex_radius;
public:

	void set_focal_length_and_outer_hex_radius(
		const double focal_length,
		const double outer_hex_radius
	);

	void print()const;

	Intersection* calculate_intersection_with(const Ray* ray)const;
private:
	void restrict_outer_hex_radius_to_curvature_radius();

	std::string get_SphereCapWithHexagonalBound_print()const;

	void post_initialize_radius_of_enclosing_sphere();
};
#endif // __SPHERECAPWITHCYLINDERBOUND_H_INCLUDED__
