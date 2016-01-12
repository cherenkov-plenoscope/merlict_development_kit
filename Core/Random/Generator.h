//==============================================================================
// include guard
#ifndef __PRNG_Generator_H_INCLUDED__
#define __PRNG_Generator_H_INCLUDED__

//==============================================================================
// forward declared dependencies

//==============================================================================
// included dependencies
#include <iostream>
#include <random>
#include "Core/Vector3D.h"
//==============================================================================

// the Pseudo random generators
namespace Random {

	class Generator {
	protected:

		unsigned seed;
	public:

		uint get_seed()const;
		virtual double uniform() = 0;
		virtual void set_seed(const uint _seed);
		void set_seed_now_using_system_clock();
		Vector3D get_point_on_xy_disc_within_radius(const double radius);
		Vector3D get_point_on_unitsphere_within_polar_distance(
			const double max_dist
		);
	};
}
#endif // __PRNG_Generator_H_INCLUDED__