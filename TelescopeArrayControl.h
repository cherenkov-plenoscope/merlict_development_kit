//=================================
// include guard
#ifndef __TELESCOPEREGISTER_H_INCLUDED__
#define __TELESCOPEREGISTER_H_INCLUDED__

//=================================
// forward declared dependencies

//=================================
// included dependencies
#include "Core/Frame.h"

class TelescopeArrayControl {

	std::vector<Frame*> telescopes_in_world;
public:	

	void add_to_telescope_array(Frame *telescope);
	void move_all_to_Az_Zd(const double Az_Rad, const double Zd_Rad);
	double get_zenith_distance_of_frame(const Frame* frame)const;
	double get_azimuth_of_frame(const Frame* frame)const;
	std::string get_print()const;
};
#endif // __TELESCOPEREGISTER_H_INCLUDED__