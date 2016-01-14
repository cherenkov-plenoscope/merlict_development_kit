//=================================
// include guard
#ifndef __PLANE_H_INCLUDED__
#define __PLANE_H_INCLUDED__

//=================================
// forward declared dependencies

//=================================
// included dependencies
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#include <math.h>
#include "Core/SurfaceEntity.h"
#include "Core/Intersection.h"
#include "XyPlaneRayIntersectionEquation.h"
#include "RectangularPrismZ.h"
//=================================
class Plane :public SurfaceEntity{
protected:	

	RectangularPrismZ RectBounds;
public:

	Plane() {};
	
	Plane(
		const std::string new_name,
        const Vector3D    new_pos,
        const Rotation3D  new_rot)
		: SurfaceEntity(new_name, new_pos, new_rot) {};

	void set_x_y_width(
		const double x_width,
		const double y_width
	);

	std::string get_print()const;

	const Intersection* calculate_intersection_with(const Ray* ray)const;
private:
	
	void post_initialize_radius_of_enclosing_sphere();
};
#endif // __PLANE_H_INCLUDED__