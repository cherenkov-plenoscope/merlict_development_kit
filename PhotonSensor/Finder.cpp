#include "Finder.h"
#include <algorithm>
//------------------------------------------------------------------------------
namespace PhotonSensors {
	using PhotonSensor::Sensor;
//------------------------------------------------------------------------------
FindSensorByFrame::FindSensorByFrame(
	const Frame* final_frame,
	const vector<Sensor*> *sensors
) {
	vector<PhotonSensor::Sensor*>::const_iterator it = std::upper_bound(
		sensors->begin(),
		sensors->end(), 
		final_frame, 
		Sensor::FrameSensorByFramePointerCompare()
	);

	photon_is_absorbed_by_known_sensor = true;

	if(it == sensors->begin()) {
		// final frame pointer is smaller than all known sensor frame pointers 
		photon_is_absorbed_by_known_sensor = false;
		return;
	}

	if(it == sensors->end() && (*(it-1))->get_frame() != final_frame) {
		// final frame pointer is latger than all known sensor frame pointers
		photon_is_absorbed_by_known_sensor = false;
		return;
	}

	if((*(it-1))->get_frame() != final_frame) {
		photon_is_absorbed_by_known_sensor = false; 
		return;
	}

	closest_sensor = (*(it-1));
    index = (it - sensors->begin());
}
//------------------------------------------------------------------------------
bool FindSensorByFrame::frame_is_in_sensors()const {
	return photon_is_absorbed_by_known_sensor;
}
//------------------------------------------------------------------------------
Sensor* FindSensorByFrame::get_sensor()const {
	return closest_sensor;
}
//------------------------------------------------------------------------------
uint FindSensorByFrame::get_index()const {
    return index;
}
//------------------------------------------------------------------------------
} // PhotonSensors