//=================================
// include guard
#ifndef __GLOBALSETTINGS_H_INCLUDED__
#define __GLOBALSETTINGS_H_INCLUDED__

//=================================
// forward declared dependencies

//=================================
// included dependencies
#include "ColourProperties.h"
#include <chrono>
#include "TracerException.h"
#include <string>
#include <sstream>

//=================================
class GlobalSettings {
private:
	//--------------------------------------------------------------------------
	// propagation options

	uint number_of_max_reflections;
		// A Ray, CameraRay or a Photon will only be traced until the max 
		// number of reflections is reached.

	bool flag_store_only_final_intersection;
		// Each Photon emitted has a history object storing its propagation
		// history. When flag_store_only_final_intersection is true only the
		// very last interaction, i.e. the absorbtion of the Photon will be 
		// stored in the history. 

	ColourProperties default_colour;
		// The default background colour in the raytracing images 

	//--------------------------------------------------------------------------
	// Multithread options

	bool multithread = true;
		// When multithread is true tmcTracer will use all threads availabe to
		// propagate the Ray,CameraRays,Photons.
		//
		// Pro: 
		//	Less computation time
		//
		// Contra: 
		//	The results can not be reproduced, althugh using the same 
		// 	seed for the random number generator. 

	//--------------------------------------------------------------------------
	// Randon number generator

	unsigned seed_for_random_number_generator_mt19937;
	
		// mt19937 is a standard mersenne_twister_engine
		//std::mt19937 pRNG_mt19937;

public:
	GlobalSettings();
	
	bool StoreOnlyLastIntersection()const;

	void SetStoreOnlyLastIntersection(bool flag);
	
	bool MultiThread()const;

	void SetMultiThread( bool flag );
	
	void set_max_number_of_reflections(const int new_max_number_of_reflections);

	ColourProperties get_default_colour()const;
	
	int get_max_number_of_reflections()const;
	
	unsigned get_decimal_precision_for_csv_output()const;
	
	void initialize_random_rumber_generator_seed_mt19937();
	
	unsigned get_seed_for_random_number_generator_mt19937()const;
	
	bool max_number_of_reflections_is_not_reached_yet(
		const uint reflection_counter
	)const;
};

#endif // __GLOBALSETTINGS_H_INCLUDED__ 
