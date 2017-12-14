#include "TelescopeArrayControl.h"
#include "ZdAzMount.h"
#include <sstream>
//------------------------------------------------------------------------------
void TelescopeArrayControl::move_all_to_Az_Zd(const double Az_Rad, const double Zd_Rad) {

	Rot3 rot = ZdAzMount::get_rot_Az_Zd(Az_Rad, Zd_Rad);

	for(Frame* telescope : telescopes_in_world)
		telescope->update_rotation(rot);
}
//------------------------------------------------------------------------------
std::string TelescopeArrayControl::str()const {

	std::stringstream out;

	out << "___Telescope_Array_Control___\n";

	unsigned int i=0;
	for(Frame* telescope : telescopes_in_world) {
		out << i << " " << telescope->get_name() << "\t";
		out << "Az: " << Rad2Deg(ZdAzMount::get_az_of_frame(telescope)) << ",\t";
		out << "Zd: " << Rad2Deg(ZdAzMount::get_zd_of_frame(telescope)) << " deg";
		out << "\n";
	}

	return out.str();
}
//------------------------------------------------------------------------------
void TelescopeArrayControl::add_to_telescope_array(Frame *telescope) {
	telescopes_in_world.push_back(telescope);
}
//------------------------------------------------------------------------------
double TelescopeArrayControl::get_zenith_distance_of_frame(const Frame* frame)const {

	Vec3 optical_axis_in_world = frame->frame2world()->
		get_transformed_orientation(Vec3::UNIT_Z);

	return Vec3::UNIT_Z.angle_in_between(optical_axis_in_world);	
}
//------------------------------------------------------------------------------
double TelescopeArrayControl::get_azimuth_of_frame(const Frame* frame)const {
	return Deg2Rad(180.0) - frame->get_rotation_in_mother().get_rot_z();
}
//------------------------------------------------------------------------------