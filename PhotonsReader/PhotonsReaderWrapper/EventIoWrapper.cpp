#include "EventIoWrapper.h"
#include "Corsika/EventIo/PhotonFactory.h"
using std::string;
using std::vector;
using std::array;

//------------------------------------------------------------------------------
EventIoWrapper::EventIoWrapper(const string _path): 
	BasicWrapper(_path),
	corsika_run(_path) {}
//------------------------------------------------------------------------------
bool EventIoWrapper::has_still_photons_left()const {
	return corsika_run.has_still_events_left();
}
//------------------------------------------------------------------------------
vector<Photon> EventIoWrapper::next(Random::Generator* prng) {
		
	EventIo::Event event = corsika_run.next_event();

	vector<Photon> photons;
    unsigned int photon_id = 0;
    for(array<float, 8> corsika_photon: event.photons) {
        
        EventIo::PhotonFactory cpf(corsika_photon, photon_id++, prng);

        if(cpf.passed_atmosphere())
            photons.push_back(cpf.get_photon());
    }
    return photons;
}