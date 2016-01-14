 #include "Plane.h"
//------------------------------------------------------------------------------
void Plane::set_x_y_width(
	const double x_width, 
	const double y_width
){	
	RectBounds.set_x_y_width(x_width, y_width);
	post_initialize_radius_of_enclosing_sphere();
}
//------------------------------------------------------------------------------
void Plane::post_initialize_radius_of_enclosing_sphere() {
	radius_of_sphere_enclosing_all_children = hypot(
		RectBounds.get_half_x_width(), 
		RectBounds.get_half_y_width()
	);
}
//------------------------------------------------------------------------------
std::string Plane::get_print()const {
	std::stringstream out;
	out << SurfaceEntity::get_print();

	out << "plane:\n";
	out << "| x width: " << 2.0*RectBounds.get_half_x_width() << "m\n";
	out << "| y width: " << 2.0*RectBounds.get_half_y_width() << "m\n";
	out << "| area:    " << 4.0*RectBounds.get_half_x_width()*RectBounds.get_half_y_width() << "m^2\n";
	return out.str();
}
//------------------------------------------------------------------------------
#include "Core/Ray.h"
#include "Core/Intersection.h"
//------------------------------------------------------------------------------
const Intersection* Plane::calculate_intersection_with(const Ray* ray)const {

	XyPlaneRayIntersectionEquation xyPlaneRayEquation(ray);

	if( xyPlaneRayEquation.has_causal_solution() ){
		
		double v = xyPlaneRayEquation.get_ray_parameter_for_intersection();
		Vector3D intersection_vector = ray->PositionOnRay(v);		

		if(	RectBounds.is_inside(&intersection_vector) ) {

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