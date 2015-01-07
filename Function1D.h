//==============================================================================
// include guard
#ifndef __FUNCTION1D_H_INCLUDED__
#define __FUNCTION1D_H_INCLUDED__

//==============================================================================
// forward declared dependencies

//==============================================================================
// included dependencies
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>    // std::lower_bound, std::upper_bound, std::sort
#include <utility>
#include "PugiXml/pugixml.hpp"
#include "TracerException.h"
#include "XmlIoException.h"
#include "XmlFileIo.h"
//==============================================================================
// Function1D allows an assignment of one floating number to annother. 
// For example when one wants to express the absorbtion coefficient of a 
// material in dependence of the wavelength of the traversing photon, 
// one can feed this function 
// wavelegth |-> absorption_coefficient(wavelength)


enum boundary_mode {ZERO,CLOSEST,STRICT};

class Function1D :public XmlFileIo{
private:
	// to parse the information of a function out of a xml file here some 
	// identifier are defined which are expected to show up in a xml file.
	std::string xml_func_node_id = "function1D";
	std::string xml_func_name_id = "name";
	std::string xml_func_pair_id = "pair";
	std::string xml_func_val_unit_id = "unit_y";
	std::string xml_func_arg_unit_id = "unit_x";
	std::string xml_func_arg_id = "x";
	std::string xml_func_val_id = "y";	
	std::string xml_func_boundary_mode_id = "boundary";
	
	//std::string xml_path = "";

	// The boundary mode declares how queries are handled, i.e. values are 
	// returned, when the requested argument is not in the range of the argument 
	// list. For example you want the reflection index of some material so you 
	// define a Function1D and feed it pairs of wavelength as a argument and 
	// reflection coefficients as corresponding values.
	// wavelength [nm] | refl. coefficient [1]
    // ----------------|----------------------
    // 450             | 0.1
    // 521             | 0.2
    // 606             | 0.6
    // 786             | 0.98
    // 1337            | 0.42

	boundary_mode mode = STRICT; // default is strict

	// the actual data structure storing the information of the function
	std::vector< std::pair<double,double> > func;
	std::vector< std::vector< double > > vunc;

	std::string Name = "";
	std::string Unit_of_argument = "";
	std::string Unit_of_value = "";

	//--------------------------------------------------------------------------
	std::string get_print()const;
	//--------------------------------------------------------------------------
	void read();
	//--------------------------------------------------------------------------
	void check_and_sort();
	//--------------------------------------------------------------------------
	static bool comp_upp( const double A, const std::pair<double,double> B ){
		return ( B.first > A);
	};
	//--------------------------------------------------------------------------
	double interpolate_linear(
		const std::pair<double,double> p0, 
		const std::pair<double,double> p1, 
		const double x
	)const;
	//--------------------------------------------------------------------------
public:
	void set(std::string path2xml_input_file);
	//--------------------------------------------------------------------------
	Function1D();
	//--------------------------------------------------------------------------
	Function1D(std::string path2xml_input_file);
	//--------------------------------------------------------------------------
	double at(const double argument)const;
	//--------------------------------------------------------------------------
	std::string name()const;
	//--------------------------------------------------------------------------
	void disp()const{std::cout << get_print() << std::endl;}
	//--------------------------------------------------------------------------
	std::vector< std::pair<double,double> >::const_iterator begin()const{
		return func.begin();
	}
	//--------------------------------------------------------------------------
	std::vector< std::pair<double,double> >::const_iterator end()const{
		return func.end();
	}
	//--------------------------------------------------------------------------
	double weighted_mean()const;
	//--------------------------------------------------------------------------
	double integral()const;
	//--------------------------------------------------------------------------
	double range()const;
	//--------------------------------------------------------------------------
	double at_with_Boundary_Mode_CLOSEST(double func_arg)const;
	double at_with_Boundary_Mode_STRICT(double func_arg)const;
	double at_with_Boundary_Mode_ZERO(double func_arg)const;
	//--------------------------------------------------------------------------	
	std::vector< std::pair<double,double> >::const_iterator get_upper_bound(double func_arg)const;
	void set_boundary_mode(const std::string mode_text);
};
#endif // __FUNCTION1D_H_INCLUDED__
