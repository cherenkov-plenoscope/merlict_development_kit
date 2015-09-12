//=================================
// include guard
#ifndef __TelescopeFrame_H_INCLUDED__
#define __TelescopeFrame_H_INCLUDED__

//=================================
// forward declared dependencies

//=================================
// included dependencies
#include "Core/Frame.h"

class TelescopeFrame : public Frame {

	double Az_Rad;
	double Zd_Rad;
public:
	TelescopeFrame(
		const std::string new_name,
		const Vector3D    new_pos,
		const Rotation3D  new_rot
    );
	void move_to_Az_Zd(const double Az_Rad, const double Zd_Rad);
	std::string get_pointing_print()const;
};
#endif // __TelescopeFrame_H_INCLUDED__