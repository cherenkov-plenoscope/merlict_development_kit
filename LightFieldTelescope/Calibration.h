//=================================
// include guard
#ifndef __LightFieldTelescopeCalibration_H_INCLUDED__
#define __LightFieldTelescopeCalibration_H_INCLUDED__

//=================================
// forward declared dependencies

//=================================
// included dependencies
#include "LightFieldTelescope/LightFieldTelescope.h"
#include "Core/Random/Random.h"

namespace LightFieldTelescope {

	struct CalibRow {
		bool reached_sensor;
		uint32_t sub_pixel_id;
		float x_pos_on_principal_aperture;
		float y_pos_on_principal_aperture;
		float x_tilt_vs_optical_axis;
		float y_tilt_vs_optical_axis;
		float relative_time_of_flight;

		CalibRow() {
			reached_sensor = false;
		};
	};

	struct SubPixelStatistics {
		double geometric_efficiency;
		double mean_cx;
		double mean_cy;
		double mean_x;
		double mean_y;
		double mean_time;

		SubPixelStatistics() {
			geometric_efficiency = 0.0;
			mean_cx = 0.0;
			mean_cy = 0.0;
			mean_x = 0.0;
			mean_y = 0.0;
			mean_time = 0.0;			
		}
	};

	class Calibration: public Printable {

		const Geometry &telescope_geometry;
		uint number_of_photons;
		uint number_of_photons_per_sub_pixel;
		uint number_of_photons_per_block;
		uint number_of_blocks;
		double max_principal_aperture_radius_to_trow_photons_on;
		double max_tilt_vs_optical_axis_to_throw_photons_in;
		double distance_to_travel_before_intersecting_principal_aperture;
		double callibration_photon_wavelenght;

		Frame* telescope;
		PhotonSensors::Sensors *sub_pixels;

		TracerSettings settings;
		PropagationEnvironment telescope_environment;
		Random::Mt19937 prng;

		std::vector<CalibRow> table;

		// statistics
		std::vector<SubPixelStatistics> statistics;

		void init_statistics();
		void fill_statistics_from_table();
		void normalize_statistics();
	public:
		void export_sub_pixel_statistics(const std::string path)const;

		Calibration(const Geometry &geometry);
		void set_up_photon_properties();
		void set_up_principal_aperture_range();
		void set_up_field_of_view_range();
		void set_up_telescope();
		void set_up_telescope_environment();
		void run_calibration();
		void fill_calibration_block_to_table();
		Photon* get_photon_given_pos_and_angle_on_principal_aperture(
			Vector3D pos_on_principal_aperture,
			Vector3D direction_on_principal_aperture
		)const;
		std::string get_print()const;
	};
}
#endif // __LightFieldTelescopeCalibration_H_INCLUDED__ 
