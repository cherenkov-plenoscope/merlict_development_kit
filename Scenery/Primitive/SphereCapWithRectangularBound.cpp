#include "SphereCapWithRectangularBound.h"
#include <algorithm>
#include <sstream>
#include <cmath>
using std::string;
using std::vector;
using std::stringstream;

//------------------------------------------------------------------------------
void SphereCapWithRectangularBound::set_curvature_radius_and_x_y_width(
	const double _curvature_radius,
	const double x_width,
	const double y_width
) {
	curvature_radius =_curvature_radius;
	rect_bounds.set_x_y_width(x_width, y_width);
	post_initialize_radius_of_enclosing_sphere();	
}
//------------------------------------------------------------------------------
double SphereCapWithRectangularBound::get_focal_length()const {
	return curvature_radius*0.5;
}
//------------------------------------------------------------------------------
void SphereCapWithRectangularBound::post_initialize_radius_of_enclosing_sphere() {
	bounding_sphere_radius = hypot(
		rect_bounds.get_half_x_width(),
		rect_bounds.get_half_y_width()
	);	
}
//------------------------------------------------------------------------------
std::string SphereCapWithRectangularBound::str()const {
	const double xw = 2.0*rect_bounds.get_half_x_width();
	const double yw = 2.0*rect_bounds.get_half_y_width();

	std::stringstream out;
	out << SurfaceEntity::str();
	out << "SphereCapWithRectangularBound:\n";
	out << "| focal length: " << get_focal_length() << "m\n";
	out << "| curvature radius: " << curvature_radius << "m\n";
	out << "| x, y width: " << xw << ", " << yw << "m\n";
	out << "| f/D: " << get_focal_length()/std::max(xw, yw) << "\n";
	return out.str();
}
//------------------------------------------------------------------------------
void SphereCapWithRectangularBound::calculate_intersection_with(
    const Ray* ray, 
    vector<Intersection> *intersections
)const {

	SphericalCapRayIntersectionEquation sphereCapRayEq(curvature_radius, ray);

	if(sphereCapRayEq.has_solutions()) {
		
		add_causeal_intersection(
			&sphereCapRayEq,
			&rect_bounds,
			ray,
			intersections
		);
	}
}