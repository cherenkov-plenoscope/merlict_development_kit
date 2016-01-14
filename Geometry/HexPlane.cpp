 #include "HexPlane.h"
//------------------------------------------------------------------------------
void HexPlane::set_outer_hex_radius(const double outer_hex_radius) {
	hex_bounds.set_outer_radius(outer_hex_radius);
	post_initialize_radius_of_enclosing_sphere();
}
//------------------------------------------------------------------------------
void HexPlane::post_initialize_radius_of_enclosing_sphere() {
	radius_of_sphere_enclosing_all_children = hex_bounds.get_outer_radius();
}
//------------------------------------------------------------------------------
std::string HexPlane::get_print()const {
	std::stringstream out;
	out << SurfaceEntity::get_print();

	out << "hexagonal plane:\n";
	out << "| outer radius: " << hex_bounds.get_outer_radius() << "m\n";
	out << "| area: "<< hex_bounds.get_area() << "m^2\n";
	return out.str();
}
//------------------------------------------------------------------------------
#include "Core/Ray.h"
#include "Core/Intersection.h"
//------------------------------------------------------------------------------
const Intersection* HexPlane::calculate_intersection_with(const Ray* ray)const {

	XyPlaneRayIntersectionEquation xyPlaneRayEquation(ray);

	if( xyPlaneRayEquation.has_causal_solution() ){
		
		double v = xyPlaneRayEquation.get_ray_parameter_for_intersection();
		Vector3D intersection_vector = ray->PositionOnRay(v);		

		if(	hex_bounds.is_inside(&intersection_vector) ) {

			Intersection* intersec;
			intersec = new Intersection(
				this,
				intersection_vector,
				xyPlaneRayEquation.get_plane_normal_vector(),
				v,
				ray->Direction()
			);

			return intersec;
		}
	}
	return empty_intersection();	
}