#include <iostream>
#include <fstream>
#include <sstream>
#include "Corsika/EventIo/Export.h"

namespace EventIo {

    void write_raw_photons(
        const string &path, 
        const vector<array<float, 8>> &photons
    ) {

        std::ofstream file;
        file.open(path, std::ios::out | std::ios::binary);
        
        if(!file.is_open()) {
            std::stringstream info;
            info << __FILE__ << " " << __LINE__ << "\n";
            info << "EventIo: Unable to write file: '" << path << "'\n";
            throw TracerException(info.str());      
        }

        for(const array<float, 8> &photon: photons)
            file.write((char*)&photon, sizeof(array<float, 8>));
        file.close();
    }
} //namespace EventIo
