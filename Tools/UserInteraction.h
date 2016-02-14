//=================================
// include guard
#ifndef __UserInteraction_H_INCLUDED__
#define __UserInteraction_H_INCLUDED__

//=================================
// forward declared dependencies

//=================================
// included dependencies
#include <iostream>
#include <string>
#include "Core/Vector3D.h"

namespace UserInteraction {
	
	void print_welcome_screen();	
	void ClearScreen();
	bool is_Escape_key(const int user_input_key);
	Vector3D get_Vector3D();
	static const uint space_key = 32;
}
#endif // __UserInteraction_H_INCLUDED__