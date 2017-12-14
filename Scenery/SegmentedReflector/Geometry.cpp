#include "Scenery/SegmentedReflector/Geometry.h"
#include "Scenery/Geometry/HexGridAnnulus.h"
#include <iomanip>
#include "Tools/StringTools.h"
#include "Core/HomTra3.h"
#include <sstream>
#include <math.h>
using std::vector;
using std::string;
using std::stringstream;

namespace SegmentedReflector {

	Geometry::Geometry(const Config config): cfg(config) {
		init_focal_point();
		init_facet_xy_positions();
		init_facet_z_positions();
		make_average_facet_distance_to_focal_point_match_focal_length();
		init_facet_orientations();
	}

	void Geometry::init_focal_point() {
		_focal_point = Vec3(0.0, 0.0, focal_length());
	}

	void Geometry::init_facet_xy_positions() {

		HexGridAnnulus hex_grid(
			max_outer_aperture_radius() - facet_spacing()/2.0,
			min_inner_aperture_radius() + facet_spacing()/2.0,
			facet_spacing()
		);

		_facet_positions = hex_grid.get_grid();
	}

	void Geometry::init_facet_z_positions() {

		for(unsigned int i=0; i<_facet_positions.size(); i++) {

			_facet_positions.at(i).set(
				_facet_positions.at(i).x,
				_facet_positions.at(i).y,
				z_pos_given_dist_to_optical_axis(
					_facet_positions.at(i).get_projection_on_XY_plane().norm()
				)
			);
		}
	}

	void Geometry::make_average_facet_distance_to_focal_point_match_focal_length() {

		_z_offset_makeing_avg_facet_dist_to_f_point_match_f = 0.0;
		unsigned int iteration_conter = 0;
		double deviation = 0.0;

		do{
			abort_if_too_many_iterations(++iteration_conter);

			deviation = focal_length() - average_image_distances_of_facets();

			double step = -deviation*0.5;
			move_all_facets_in_z(step);
			_z_offset_makeing_avg_facet_dist_to_f_point_match_f += step;

		}while( fabs(deviation) > focal_length()*1e-6);
	}
	
	void Geometry::abort_if_too_many_iterations(const unsigned int iteration_conter) {
		
		if(iteration_conter > 100) {
			stringstream info;
			info << __FILE__ << ", " << __LINE__<< "\n";
			info << "Exceeded max number of 100 iterations. ";
			info << "Can not reach best reflector z position. ";
			throw std::runtime_error(info.str());
		}
	}

	double Geometry::average_image_distances_of_facets()const {

		double sum_of_image_distances = 0.0;

		for(Vec3 facet_pos: _facet_positions)
			sum_of_image_distances = sum_of_image_distances + 
				(facet_pos - focal_point()).norm();

		return sum_of_image_distances/number_of_facets();
	}

	void Geometry::move_all_facets_in_z(const double movement) {

		for(unsigned int i=0; i<_facet_positions.size(); i++) {

			_facet_positions.at(i).set(
				_facet_positions.at(i).x,
				_facet_positions.at(i).y,
				_facet_positions.at(i).z + movement
			);
		}	
	}

	void Geometry::init_facet_orientations() {

		for(Vec3 facet_pos :_facet_positions)
			_facet_orientations.push_back(
				get_rotation_for_facet_position(facet_pos)
			);		
	}

	Rot3 Geometry::get_rotation_for_facet_position(const Vec3 facet_position)const {
		
		Vec3 focal_point_to_facet_pos = _focal_point - facet_position;
			
		Vec3 rotation_axis = 
			focal_point_to_facet_pos.cross(Vec3::UNIT_Z);

		double angle_between_unit_z_and_focal_point_to_mirror_pos =
			Vec3::UNIT_Z.angle_in_between(focal_point_to_facet_pos);

		double rot_angle = 0.5*angle_between_unit_z_and_focal_point_to_mirror_pos;

		return Rot3(rotation_axis, -rot_angle);	
	}

	double Geometry::z_offset_makeing_avg_facet_dist_to_f_point_match_f()const {
		return _z_offset_makeing_avg_facet_dist_to_f_point_match_f;
	}

	double Geometry::focal_length()const {
		return cfg.focal_length;
	}

	double Geometry::DaviesCotton_over_parabolic_mixing_factor()const {
		return cfg.DaviesCotton_over_parabolic_mixing_factor;
	}
	
	double Geometry::max_outer_aperture_radius()const {
		return cfg.max_outer_aperture_radius;
	}
	
	double Geometry::min_inner_aperture_radius()const {
		return cfg.min_inner_aperture_radius;
	}
	
	double Geometry::facet_inner_hex_radius()const {
		return cfg.facet_inner_hex_radius;
	}

	double Geometry::facet_outer_hex_radius()const {
		return facet_inner_hex_radius()*2.0/sqrt(3.0);
	}
	
	double Geometry::gap_between_facets()const {
		return cfg.gap_between_facets;
	}

	double Geometry::z_pos_given_dist_to_optical_axis(const double dist)const {

		const double zDC = z_Davies_Cotton_given_dist_to_optical_axis(dist);
		const double zPa = z_Parabola_given_dist_to_optical_axis(dist);
		const double DC_over_Pa = DaviesCotton_over_parabolic_mixing_factor();

		return DC_over_Pa*zDC + (1.0 - DC_over_Pa)*zPa;
	}

	double Geometry::z_Davies_Cotton_given_dist_to_optical_axis(double dist)const {
		return focal_length() - sqrt(focal_length()*focal_length() - dist*dist);
	}

	double Geometry::z_Parabola_given_dist_to_optical_axis(double dist)const {
		return 1.0/(4.0*focal_length())*dist*dist;
	}

	Vec3 Geometry::focal_point()const {
		return _focal_point;
	}

	vector<Vec3> Geometry::facet_positions()const {
		return _facet_positions;
	}

	double Geometry::DaviesCotton_weight()const {
		return DaviesCotton_over_parabolic_mixing_factor();
	}

	double Geometry::Parabolic_weight()const {
		return 1.0 - DaviesCotton_over_parabolic_mixing_factor();
	}

	double Geometry::facet_spacing()const {
		return facet_inner_hex_radius()*2.0 + gap_between_facets();
	}

	double Geometry::naive_f_over_D()const {
		return focal_length()/(2.0*max_outer_aperture_radius());
	}

	double Geometry::effective_f_over_D()const {
		return focal_length()/(effective_radius()*2.0);
	}

	double Geometry::naive_area()const {
		double Ro = max_outer_aperture_radius();
		double Ri = min_inner_aperture_radius();
		return M_PI*(Ro*Ro - Ri*Ri);
	}

	double Geometry::effective_area()const {
		return number_of_facets()*facet_area();
	}

	double Geometry::effective_radius()const {
		return sqrt(effective_area()/M_PI);
	}

	double Geometry::facet_area()const {
		return facet_inner_hex_radius()*facet_inner_hex_radius()*6.0/sqrt(3.0); 
	}

	double Geometry::number_of_facets()const {
		return _facet_positions.size();
	}

	double Geometry::thickness_of_dish()const {
		return z_pos_given_dist_to_optical_axis(max_outer_aperture_radius());
	}

	string Geometry::str()const {

		stringstream out;
		out << std::setprecision(4);
		out << "Segmented_Reflector__\n";

		stringstream tab;
		tab << "focal length................... " << focal_length() << "m\n";
		tab << "focal point.................... " << focal_point().str() << "\n";
		tab << "max outer aperture diameter.... " << 2.0*max_outer_aperture_radius() << "m\n";
		tab << "min inner aperture diameter.... " << 2.0*min_inner_aperture_radius() << "m\n";
		tab << "naive f over D................. " << naive_f_over_D() << "\n";
		tab << "naive area..................... " << naive_area() << "m^2\n";
		tab << "thickness of dish.............. " << thickness_of_dish() << "m\n";
		tab << "z offset for optimal focus..... " << z_offset_makeing_avg_facet_dist_to_f_point_match_f() << "m\n";
		tab << "effective F-Number............. " << effective_f_over_D() << "\n";
		tab << "effective area................. " << effective_area() << "m^2\n";
		tab << "effective diameter............. " << 2.0*effective_radius() << "m\n";
		tab << "number of facets............... " << number_of_facets() << "\n";
		tab << "facet area..................... " << facet_area() << "m^2\n";
		tab << "facet inner radius............. " << facet_inner_hex_radius() << "m\n";
		tab << "facet spacing.................. " << facet_spacing() << "m\n";
		tab << "gap between facets............. " << gap_between_facets()*1e3 << "mm\n";
		tab << "Davies Cotton weight........... " << DaviesCotton_weight() << "\n";
		tab << "Parabolic weight............... " << Parabolic_weight() << "\n";
			
		out << StringTools::place_first_infront_of_each_new_line_of_second(
			"  ", 
			tab.str()
		);

		return out.str();		
	}

	string Geometry::facet_positions_and_normals_to_text()const {
		stringstream out;
		out << "x[m] " << "y[m] " << "z[m] " << "nx[1] " << "ny[1] " << "nz[1]\n"; 
		for(unsigned int i=0; i<_facet_positions.size(); i++) {

			HomTra3 trafo;
			trafo.set_transformation(_facet_orientations[i], Vec3::ORIGIN);

			Vec3 pos = _facet_positions[i];
			Vec3 normal = trafo.get_transformed_orientation(Vec3::UNIT_Z);

			out << std::fixed << std::setprecision(4);
			out << pos.x << " ";
			out << pos.y << " ";
			out << pos.z << " ";

			out << std::fixed << std::setprecision(9);
			out << normal.x << " ";
			out << normal.y << " ";
			out << normal.z << "\n";
		}
		return out.str();
	}
} // SegmentedReflector