#include "Sphere.h"
//======================
Sphere::Sphere(){
}
//======================
void Sphere::set_sphere(double new_radius){

	if(new_radius <= 0.0){
		std::stringstream info;
		info << "Sphere::set_sphere\n";
		info << "The radius of a sphere must be larger than 0.0m !\n";
		info << "Expected: >0.0, but actual: " << new_radius << "\n";
		throw TracerException(info.str());
	}
	
	radius = new_radius;
	radius_of_sphere_enclosing_all_children = new_radius;

}
//======================
void Sphere::disp()const {
	std::stringstream out;
	out << "sphere:" <<name_of_frame;
	out << "_________________________________\n";
	out << get_frame_string();
	out << get_surface_propertie_prompt();
	out << get_sphere_string();
	out << "_________________________________\n";
	std::cout << out.str();
}
//======================
std::string Sphere::get_sphere_string()const {
	std::stringstream out;
	out<<"||| radius of sphere: "<<radius<<std::endl;
	return out.str();
}
//======================
void Sphere::hit(Vector3D *base,Vector3D *dir, Intersection *intersection)const{	
	// 
	// 		r = sqrt( x^2 + y^2 + z^2 )
	// 		g:  b + v*d
	// 		put g into sphere eq
	// 		r = sqrt((bx+v*dx)^2 + (by+v*dy)^2 + (bz+v*dz)^2 )
	// 		r = sqrt( v^2 (dd) + v 2(bd) + (bb) )
	// 	    r^2 = v^2 (dd) + v 2(bd) + (bb)
	// 	   	0 = v^2 + v 2(bd/dd) + (bb/dd -r^2)
	//		solve quadrativ eqaution in v

	QuadraticEquation quadratic_eq_in_v(
		2*( (*base * *dir) / (*dir * *dir) ),
		(*base * *base) / (*dir * *dir) -pow(radius,2.0)
	);

	if( quadratic_eq_in_v.has_valid_solutions() ){
		// at least one hit

		double v_Plus  = quadratic_eq_in_v.plus_solution();
		double v_Minus = quadratic_eq_in_v.minus_solution();

		Vector3D vec_intersection;
		Vector3D vec_surface_normal;

		if(v_Plus >= 0.0 && v_Minus >= 0.0){
			// facing the sphere from the outside
			
			vec_intersection = *base + *dir *v_Minus;
			vec_surface_normal =
			vec_intersection/vec_intersection.norm2();
			
			// intersection 
			intersection->set_intersection_flag(true);
			intersection->set_pointer_to_intersecting_object(this);
			intersection->set_intersection(
				&vec_intersection,
				&vec_surface_normal,
				&v_Minus
			);
			
		}else{
			
			if(v_Plus < 0.0 && v_Minus < 0.0){
				// looking away from the sphere from outside
			}else{
				// ray starts inside of the sphere
				vec_intersection = *base + *dir *v_Plus;
				vec_surface_normal =
				vec_intersection/vec_intersection.norm2();
				
				// intersection 
				intersection->set_intersection_flag(true);
				intersection->set_pointer_to_intersecting_object(this);
				intersection->set_intersection(
					&vec_intersection,
					&vec_surface_normal,
					&v_Plus
				);	
			}
		}
	} 
}
