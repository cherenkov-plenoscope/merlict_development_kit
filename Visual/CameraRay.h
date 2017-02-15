//=================================
// include guard
#ifndef __CAMERARAY_H_INCLUDED__
#define __CAMERARAY_H_INCLUDED__

//=================================
// forward declared dependencies
class Frame;
class Intersection;
//=================================
// included dependencies
#include "Core/Ray.h"
#include "Core/Color.h"
#include "Config.h"

namespace Visual {

class CameraRay :public Ray{
protected:
	
	Color color;
public:
	CameraRay(){};
	CameraRay(const Vec3 support, const Vec3 direction);
	std::string get_print()const;
	Color trace(
		const Frame* world,
		uint refl_count,
		const Config *visual_config
	);
private:

	Color shadow_of_sky_light(
		const Frame* world,
		const Config *visual_config,
		const Intersection &intersection
	)const;
	bool is_iluminated_by_sky_light_source(
		const Frame* world,
		const Config *visual_config,
		const Intersection &intersection
	)const;
	bool surface_normal_is_facing_camera(
		const Intersection &intersection
	)const;
};

}//Visual

#endif // __CAMERARAY_H_INCLUDED__ 