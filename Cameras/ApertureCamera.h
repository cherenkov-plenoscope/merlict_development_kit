//=================================
// include guard
#ifndef __APERTURECAMERA_H_INCLUDED__
#define __APERTURECAMERA_H_INCLUDED__

//=================================
// forward declared dependencies
class Frame;
class GlobalSettings;
//=================================
// included dependencies
#include "DistanceMeter.h"
#include "CameraDevice.h"
#include "PseudoRandomNumberGenerator.h"
//=================================
class ApertureCamera :public CameraDevice{
public:
	using CameraDevice::CameraDevice;

	void set_focus_to(const double ObjectDistance_in_m);

	void set_FoV_in_rad(const double FoV_in_rad);

	void set_fStop_sesnorWidth_rayPerPixel(	
		const double new_FStopNumber,
		const double new_SensorSizeX,
		const uint rays_per_pixel 
	);
	
	CameraRay get_ray_for_pixel_in_row_and_col(const uint row, const uint col);

	void acquire_image(
		const Frame* world,
		const GlobalSettings* settings
	);

	void auto_focus(
		const Frame* world,
		const GlobalSettings* settings
	);

	std::string get_print()const;
private:
	double FStopNumber;			
	double ApertureRadius_in_m;	
	double FocalLength_in_m; 	
	double ObjectDistance_in_m; 
	double SensorDistance_in_m; 
	double sensor_width_in_m; 	
	double sensor_height_in_m; 	
	double PixelPitch_in_m; 
	const double default_object_distance_in_m = 25.0;	

	int rays_per_pixel;
	
	PseudoRandomNumberGenerator dice;

	void set_F_stop_number(const double new_FStopNumber);
	void set_sensor_size_using_width(const double sensor_width_in_m);
	void update_sensor_pixel_pitch();
	void update_sensor_distance_given_focal_and_object_distance();
	void set_object_distance(const double ObjectDistance_in_m);
	void set_number_of_rays_per_pixel(const uint rays_per_pixel);
	Vector3D get_random_point_on_bounded_aperture_plane();

	Vector3D get_intersec_of_cam_ray_for_pix_row_col_with_obj_plane(
		const uint row, const uint col
	);

	Vector3D camera_ray_support_vector_in_world_frame(
		const Vector3D &cam_ray_support_in_cam_frame
	)const;

	Vector3D camera_ray_direction_vector_in_world_frame(
		const Vector3D &cam_ray_direction
	)const;

	double get_object_size_for_image_size(const double image_size_in_m)const;
	void update_focal_length();
	void update_aperture_radius();
	void set_default_object_distance();
	std::string get_aperture_camera_print()const;
	
	uint get_random_row();
	uint get_random_col();

	double get_average_object_distance(
		const Frame* world,
		const GlobalSettings* settings
	);
	uint _5_permil_of_pixels()const;
};
#endif // __APERTURECAMERA_H_INCLUDED__