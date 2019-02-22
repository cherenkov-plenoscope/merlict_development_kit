//=================================
// include guard
#ifndef __SegmentedReflectorConfig_H_INCLUDED__
#define __SegmentedReflectorConfig_H_INCLUDED__

//=================================
// forward declared dependencies

//=================================
// included dependencies
#include "Core/Color.h"
#include "Core/Function/Function.h"

namespace relleums {
namespace SegmentedReflector {

	struct Config {

		double focal_length;
		double DaviesCotton_over_parabolic_mixing_factor;
		double max_outer_aperture_radius;
		double min_inner_aperture_radius;
		double facet_inner_hex_radius;
		double gap_between_facets;	

		const Color *mirror_color;
		const Color *inner_mirror_color;
		const Function::Func1* reflectivity;

		Config();
	};

	static const Function::Func1 typical_reflectivity(
		{
			{200e-9, .8},
			{1200e-9, .8}
		});

	static const Function::Func1 perfect_reflectivity(
		{
			{200e-9, 1},
			{1200e-9, 1}
		});
} // SegmentedReflector
}  // namespace relleums
#endif // __SegmentedReflectorConfig_H_INCLUDED__ 