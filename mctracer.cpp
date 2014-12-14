#include <iostream> 
#include <string>
#include <sstream>

#include "WorldFactory.h"
#include "Vector3D.h"
#include "Functions.h"
//====================
#include "Ray.h"
#include "Photon.h"
#include "CameraRay.h"
//====================
#include "ListOfPropagations.h"
#include "FreeOrbitCamera.h"
#include "CsvHandler.h"

#include "MmcsCorsikaFullEventGetter.h"
#include "MmcsCorsikaPhotonData.h"

int main(){
	ClearScreen();
	
	std::stringstream out;
	//               1         2         3         4         5         6
	//      123456789012345678901234567890123456789012345678901234567890
	out << " ___________________________________________________________ \n";
	out << "|                                                           |\n";
	out << "|                 welcome to MCtracer                       |\n";
	out << "|                                                           |\n";
	out << "|      high performance raytracing tool to design and       |\n";
	out << "|     investigate optical devices for particle physics      |\n";
	out << "|                                                           |\n";
	out << "|     Author: Sebastian Mueller              year 2013      |\n";
	out << "|___________________________________________________________|\n";
	cout << out.str() << endl;

	GlobalSettings settings;
	
	try{

		settings.set_max_number_of_reflections(5);
		settings.set_csv_decimal_presicion(9);
		
		std::string user_input;
		std::cout << "Enter a world file to load: ";
		std::cin  >> user_input;

		WorldFactory file2world;
		file2world.load(user_input);
		CartesianFrame *Mworld = file2world.world();

		string filename = "../test/MMCS_files/cer000005";
		MmcsCorsikaFullEventGetter event_getter(filename);

		while(event_getter.has_still_events_left()) {
			
			MmcsCorsikaEvent event = event_getter.get_next_event();	
			ListOfPropagations *photons = event.transform_to_mcTracer_photons();
			photons->disp();
			photons->propagate(Mworld,&settings);
		}

		FreeOrbitCamera free(Mworld,&settings);
	}catch(std::exception &error){
		
		cerr << error.what(); 
		return 0;
	}	
	return 0;
}
