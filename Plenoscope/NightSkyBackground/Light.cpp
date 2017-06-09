#include "Plenoscope/NightSkyBackground/Light.h"
#include "Core/PhysicalConstants.h"
#include "Tools/Tools.h"
#include "NightSkyBackground.h"

namespace Plenoscope {
namespace NightSkyBackground {
//------------------------------------------------------------------------------
Light::Light(
	const LightFieldSensor::Geometry *_sensor_geometry, 
	const Function::Func1D* _flux_vs_wavelength
):
	flux_vs_wavelength(_flux_vs_wavelength),
	wavelength_probability(_flux_vs_wavelength),
	sensor_geometry(_sensor_geometry)
{
	fov_radius = 
		FOV_RADIUS_OVERHEAD*
		sensor_geometry->max_FoV_radius();

	fov_solid_angle = get_solid_angle_for_opening_angle(
		fov_radius);

	aperture_radius = 
		APERTURE_RADIUS_OVERHEAD*
		sensor_geometry->expected_imaging_system_max_aperture_radius();

	aperture_area = M_PI*aperture_radius*aperture_radius;

	rate =
		wavelength_probability.get_total_integral_of_distribution()*
		aperture_area*
		fov_solid_angle;
}
//------------------------------------------------------------------------------
string Light::__repr__()const {

	std::stringstream out;
	out << "NightSkyBackground\n";
	out << "  rate................. " << rate << " Hz\n";
	out << "  FoV solid angle...... " << fov_solid_angle << " sr\n";
	out << "  FoV radius........... " << Rad2Deg(fov_radius) << " deg\n";
	out << "  aperture area........ " << aperture_area << " m^2\n";
	out << "  aperture radius...... " << aperture_radius << " m\n";
	out << "  wavelength integral.. " << 
		wavelength_probability.get_total_integral_of_distribution() << 
		" 1/(s sr m^2)\n";
	out << "  flux vs wavelength... " << *flux_vs_wavelength;
	return out.str();
}
//------------------------------------------------------------------------------
}// NightSkyBackground
}// Plenoscope