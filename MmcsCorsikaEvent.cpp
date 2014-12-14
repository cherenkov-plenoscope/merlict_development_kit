#include "MmcsCorsikaEvent.h"
//------------------------------------------------------------------------------
MmcsCorsikaEvent::MmcsCorsikaEvent(
		MmcsCorsikaSubBlock event_header,
		MmcsCorsikaPhotonData photon_data,
		MmcsCorsikaSubBlock event_end
){
	this->event_header = event_header;
	this->photon_data = photon_data;
	this->event_end	= event_end;
}
//------------------------------------------------------------------------------
ListOfPropagations* MmcsCorsikaEvent::transform_to_mcTracer_photons()const {
	
	ListOfPropagations *cherenkov_photons;
	cherenkov_photons = new ListOfPropagations("MmcsEvent");

	for(uint i=0; i<photon_data.number_of_photons(); i++) {
		cherenkov_photons->push_back(get_mcTracer_photon(i));
	}

	return cherenkov_photons;
}
//------------------------------------------------------------------------------
Photon* MmcsCorsikaEvent::get_mcTracer_photon(const uint i)const {

	Vector3D causal_dir = causal_direction(i);

	Ray ray_running_upwards_from_ground_to_pos_of_production(
		intersection_with_xy_floor_plane(i),
		causal_dir*(-1)
	);

	double ray_parameter_for_production_height = (
		production_height_in_m(i) - intersection_with_xy_floor_plane(i).z()
		)/-causal_dir.z();

	Vector3D causal_support = 
		ray_running_upwards_from_ground_to_pos_of_production.
		PositionOnRay(ray_parameter_for_production_height);

	Photon* cherenkov_photon;
	cherenkov_photon = new Photon(
		causal_support,
		causal_dir,
		wavelength_in_m_of_photon(i)
	);	

	return cherenkov_photon;
}
//------------------------------------------------------------------------------
Vector3D MmcsCorsikaEvent::causal_direction(const uint i)const {
	return Vector3D(
		double(photon_data.get_cosine_between_dir_and_world_x_axis(i)),
		double(photon_data.get_cosine_between_dir_and_world_y_axis(i)),
		-1.0
	);	
}
//------------------------------------------------------------------------------
Vector3D MmcsCorsikaEvent::intersection_with_xy_floor_plane(const uint i)const {
	return Vector3D(x_pos_on_xy_plane_in_m(i), y_pos_on_xy_plane_in_m(i), 0.0);	
}
//------------------------------------------------------------------------------
double MmcsCorsikaEvent::x_pos_on_xy_plane_in_m(const uint i)const {
	return photon_data.get_x_pos_on_world_x_y_plane_in_cm(i)/1e2;
}
//------------------------------------------------------------------------------
double MmcsCorsikaEvent::y_pos_on_xy_plane_in_m(const uint i)const {
	return photon_data.get_y_pos_on_world_x_y_plane_in_cm(i)/1e2;
}
//------------------------------------------------------------------------------
double MmcsCorsikaEvent::production_height_in_m(const uint i)const {
	return photon_data.get_production_height_in_cm(i)/1e2;
}
//------------------------------------------------------------------------------
double MmcsCorsikaEvent::wavelength_in_m_of_photon(const uint i)const {
	return photon_data.get_wavelength_in_nm(i)/1e9;
}
//------------------------------------------------------------------------------
