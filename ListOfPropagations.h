//=================================
// include guard
#ifndef __LISTOFRAYS_H_INCLUDED__
#define __LISTOFRAYS_H_INCLUDED__

//=================================
// forward declared dependencies

//=================================
// included dependencies
#include <iostream>
#include "Ray.h"
#include "CsvHandler.h"
#include <list>
#include <sstream>
using namespace std;

//=================================
class ListOfPropagations{
	
	std::string name_of_list_of_propagations;
	std::list<Ray*> list_of_ptrs_to_propagations;
	std::vector<std::string> list_of_files_fed_into_list;
	
//=================================	
public:
ListOfPropagations(std::string new_name_for_list_of_propagations);
void push_back(Ray* ptr_to_ray_to_push_back);
void export_csv(std::string name_of_csv_file_to_be_exported)const;
void disp()const;
void propagate()const;
private:
std::string get_info_string()const;
std::string get_csv_table()const;
public:
friend std::ostream& operator<<(std::ostream& os, const ListOfPropagations& list);
};
#endif // __LISTOFRAYS_H_INCLUDED__
