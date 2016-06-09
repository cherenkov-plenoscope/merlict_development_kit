//=================================
// include guard
#ifndef __SphereCapWithHexagonalBound_H_INCLUDED__
#define __SphereCapWithHexagonalBound_H_INCLUDED__

//=================================
// forward declared dependencies

//=================================
// included dependencies
#include "Scenery/Geometry/SphericalCapRayIntersectionEquation.h"
#include "Scenery/Geometry/HexagonalPrismZ.h"
#include "Scenery/Geometry/SurfaceWithOuterPrismBound.h"

//=================================
class SphereCapWithHexagonalBound :public SurfaceWithOuterPrismBound {
protected:	
	HexagonalPrismZ hexBounds;

	double focal_length;
	double curvature_radius;
	double outer_hex_radius;
public:

	void set_curvature_radius_and_outer_hex_radius(
		const double curvature_radius,
		const double outer_hex_radius
	);
	std::string get_print()const;
	void calculate_intersection_with(
        const Ray* ray, 
        vector<Intersection> *intersections
    )const;
    double get_focal_length()const;
private:

	void restrict_outer_hex_radius_to_curvature_radius();
	void post_initialize_radius_of_enclosing_sphere();
};
#endif // __SphereCapWithHexagonalBound_H_INCLUDED__