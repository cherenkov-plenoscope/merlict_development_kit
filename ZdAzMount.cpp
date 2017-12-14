#include "ZdAzMount.h"

namespace ZdAzMount {
	//--------------------------------------------------------------------------
	Rot3 get_rot_Az_Zd(const double az, const double zd) {
		return Rot3(0.0, zd, Deg2Rad(180.0) - az);
	}
	//--------------------------------------------------------------------------
	double get_zd_of_frame(const Frame* frame) {
		Vec3 optical_axis_in_world = frame->frame2world()->
			get_transformed_orientation(Vec3::UNIT_Z);

		return 
			Vec3::UNIT_Z.angle_in_between(optical_axis_in_world);	
	}
	//--------------------------------------------------------------------------
	double get_az_of_frame(const Frame* frame) {
		return Deg2Rad(180.0) - frame->get_rotation_in_mother().get_rot_z();
	}
	//--------------------------------------------------------------------------
} // AltitudeAzimuthMount
