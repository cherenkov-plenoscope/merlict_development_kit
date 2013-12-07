//=================================
// include guard
#ifndef __WORLDFACTORY_H_INCLUDED__
#define __WORLDFACTORY_H_INCLUDED__

//=================================
// forward declared dependencies


//=================================
// included dependencies
#include <iostream> 
#include <string>
#include <sstream>
#include "CartesianFrame.h"
#include "Triangle.h"
#include "Plane.h"
#include "Sphere.h"
#include "Cylinder.h"
#include "Disc.h"
#include "FactTelescope.h"
#include "Vector3D.h"
#include "Rotation3D.h"
#include "ReflectionProperties.h"
#include "ColourProperties.h"
// XML parser library 
#include "pugixml.hpp"

//======================================================================
class tuple3{
public:
	double x;
	double y;
	double z;
tuple3(){ x=0.0; y=0.0; z=0.0; }	
};
//======================================================================
class WorldFactoryException{
public:
	virtual void ReportException() = 0;
	void ExceptionPrompt(){
		
		std::stringstream out;
		out.str("");
		out << "Frame factory: ";
		std::cout << out.str();		
	}
};
//==========================
class UnknowenObject : public WorldFactoryException{
public:
UnknowenObject(std::string new_name_of_unknowen_object){
	name_of_unknown_object = new_name_of_unknowen_object;
}
void ReportException(){ 
	ExceptionPrompt();
	
	std::stringstream out;	
	out.str("");
	out << "Do not know an object called ";
	out << "'"<<name_of_unknown_object<<"'"<<"!";
	std::cout << out.str() << endl;
}
private:
	std::string name_of_unknown_object;
};
//==========================
class SyntaxError : public WorldFactoryException{
public:
SyntaxError(
std::string new_syntax_template,
std::string new_syntax_causing_error){
	syntax_template = new_syntax_template;
	syntax_causing_error = new_syntax_causing_error;	
	I_have_an_idea_of_what_caused_the_syntax_error = false;	
}
SyntaxError(
std::string new_syntax_template,
std::string new_syntax_causing_error,
std::string new_my_idea_of_what_caused_the_syntax_error){
	
	syntax_template = 
	new_syntax_template;
	
	syntax_causing_error = 
	new_syntax_causing_error;
		
	my_idea_of_what_caused_the_syntax_error = 
	new_my_idea_of_what_caused_the_syntax_error;
	
	I_have_an_idea_of_what_caused_the_syntax_error = true;	
}
void ReportException(){ 
	ExceptionPrompt();
	
	std::stringstream out;
	out.str("");
	out << "Syntax error! ";
	out << "I expect a syntax like >" << syntax_template << "< ";
	out << "but I found this >" << syntax_causing_error << "<.";
	
	if(I_have_an_idea_of_what_caused_the_syntax_error){
		out << " I think it is because of: ";
		out << my_idea_of_what_caused_the_syntax_error;
	}
	
	out << "\n";
	std::cout << out.str() << endl;
}
private:
	bool 		I_have_an_idea_of_what_caused_the_syntax_error;
	std::string syntax_template;
	std::string syntax_causing_error;
	std::string my_idea_of_what_caused_the_syntax_error;
};
//==========================
class MissingItem : public WorldFactoryException{
public:
MissingItem(std::string new_name_of_missing_item){
	name_of_missing_item = new_name_of_missing_item;
	I_have_more_percise_information = false;
}
MissingItem(
std::string new_name_of_missing_item,
std::string new_more_percise_information){
	name_of_missing_item = new_name_of_missing_item;
	more_percise_information = new_more_percise_information;
	I_have_more_percise_information = true;
}
void ReportException(){ 
	ExceptionPrompt();
	
	std::stringstream out;	
	out.str("");
	out << "I miss an item called ";
	out << "'"<<name_of_missing_item<<"'"<<"!";
	if(I_have_more_percise_information){
		out << " " << more_percise_information;
	}
	std::cout << out.str() << endl;
}
private:
	std::string name_of_missing_item;
	std::string more_percise_information;
	bool I_have_more_percise_information;
};
//======================================================================
class WorldFactory   {
	
	CartesianFrame* root_of_World;
	
public:
WorldFactory();
//=================================
bool load_file(std::string filename);
//=================================
CartesianFrame* get_pointer_to_world();
//=================================
private:
void frame_factory
(CartesianFrame* mother,const pugi::xml_node frame_node);
//=================================
void go_on_with_children_of_node
(CartesianFrame* mother,const pugi::xml_node node);
//=================================
CartesianFrame* produceCartesianFrame
(CartesianFrame* mother,const pugi::xml_node frame_node);
//=================================
CartesianFrame* producePlane
(CartesianFrame* mother,const pugi::xml_node frame_node);
//=================================
CartesianFrame* produceSphere
(CartesianFrame* mother,const pugi::xml_node node);
//=================================
CartesianFrame* produceCylinder
(CartesianFrame* mother,const pugi::xml_node node);
//=================================
CartesianFrame* produceFactReflector
(CartesianFrame* mother,const pugi::xml_node node);
//=================================
CartesianFrame* produceDisc
(CartesianFrame* mother,const pugi::xml_node node);
//=================================
CartesianFrame* produceTriangle
(CartesianFrame* mother,const pugi::xml_node node);
//=================================
bool parse3tuple(tuple3 &tuple,const std::string text);
//=================================
void parseFloatingNumber(
double &FloatingNumber,
std::string text_to_parse);
//=================================
bool set_frame(std::string &name,Vector3D &position,
Rotation3D &rotation,const pugi::xml_node frame_node);
//=================================
bool set_surface(ReflectionProperties &reflection_cefficient,
ColourProperties &colour,const pugi::xml_node node);
//=================================
bool set_plane(
double &min_x, double &max_x, double &min_y, double &max_y,
const pugi::xml_node node);
//=================================
bool set_sphere(double &radius,const pugi::xml_node node);
//=================================
bool set_Cylinder(double &cylinder_radius,Vector3D &start_of_cylinder,
Vector3D &end_of_cylinder,const pugi::xml_node node);
//=================================
bool set_fact_reflector(double &alpha,const pugi::xml_node node);
//=================================
bool set_Disc(double &radius,const pugi::xml_node node);
//=================================
bool set_Triangle(
Vector3D &point_A,
Vector3D &point_B,
Vector3D &point_C,
const pugi::xml_node node);
//=================================
bool check_name_for_multiple_usage(const 
CartesianFrame *entitie_in_world,std::string name);
//=================================
};

#endif // __WORLDFACTORY_H_INCLUDED__ 
