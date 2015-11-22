//======================================================================
// include guard
#ifndef __Color_H_INCLUDED__
#define __Color_H_INCLUDED__

//======================================================================
// forward declared dependencies

//======================================================================
// included dependencies
#include <iostream>
#include <string>
#include "Core/Printable.h"

class Color : public Printable{

	double red_0to255;
	double green_0to255;
	double blue_0to255;
public:
	
	static const Color white;
	static const Color sky_blue;
	static const Color gray;
	static const Color dark_gray;
	static const Color black;
	static const Color red;
	static const Color green;
	static const Color blue;

	Color();
	Color(const int r, const int g, const int b);
	Color(const double r, const double g, const double b);

	void set_RGB_0to255(
		const int red,
		const int green,
		const int blue
	);

	void set_RGB_0to255(
		const double red,
		const double green,
		const double blue
	);

	unsigned char get_R_as_uchar()const;
	unsigned char get_G_as_uchar()const;
	unsigned char get_B_as_uchar()const;
	
	double get_R_as_double()const;
	double get_G_as_double()const;
	double get_B_as_double()const;

	std::string get_print()const;

	void reflection_mix(
		const Color *c,
		const double refl_coeff
	);

	void mixture(
		const Color *coulour_to_mix_with,
		const double mixture_coefficient
	);
private:
	void set_to_default_color();
	void assert_is_in_valid_8Bit_range(const int channel)const;
	void assert_is_in_valid_8Bit_range(const double channel)const;
};
#endif // __Color_H_INCLUDED__ 
