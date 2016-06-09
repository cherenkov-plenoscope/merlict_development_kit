#include "Disc.h"
//------------------------------------------------------------------------------
void Disc::set_radius(const double radius){

	cylinder_bounds.set_radius(radius);
	post_initialize_radius_of_enclosing_sphere();
}
//------------------------------------------------------------------------------
void Disc::post_initialize_radius_of_enclosing_sphere(){
	radius_of_sphere_enclosing_all_children = cylinder_bounds.get_radius();
}
//------------------------------------------------------------------------------
std::string Disc::get_print()const {
	std::stringstream out;
	out << SurfaceEntity::get_print();

	out << "disc:\n";
	out << "| radius: " << cylinder_bounds.get_radius() << "m\n";
	out << "| area:   " << get_area() << "m^2\n";
	return out.str();
}
//------------------------------------------------------------------------------
double Disc::get_area()const {
	return cylinder_bounds.get_radius()*cylinder_bounds.get_radius()*M_PI;
}
//------------------------------------------------------------------------------
void Disc::calculate_intersection_with(
    const Ray* ray, 
    vector<Intersection> *intersections
)const {

	XyPlaneRayIntersectionEquation xyPlaneRayEquation(ray);

	if( xyPlaneRayEquation.has_causal_solution() ){
		
		double v = xyPlaneRayEquation.get_ray_parameter_for_intersection();
		Vec3 intersection_vector = ray->get_pos_at(v);		

		if(cylinder_bounds.is_inside(&intersection_vector) ) {

			if(ray->get_support() != intersection_vector) {
				intersections->push_back(
					Intersection(
						this,
						intersection_vector,
						xyPlaneRayEquation.get_plane_normal_vector(),
						v,
						ray->get_direction()
					)
				);
			}
		}
	}	
}