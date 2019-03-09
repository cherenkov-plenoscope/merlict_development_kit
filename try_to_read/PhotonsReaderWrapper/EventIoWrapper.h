//=================================
// include guard
#ifndef __PhotonsReaderEventIoWrapper_H_INCLUDED__
#define __PhotonsReaderEventIoWrapper_H_INCLUDED__

//=================================
// forward declared dependencies

//=================================
// included dependencies
#include "BasicWrapper.h"
#include "merlict_corsika_eventio/eventio.h"
#include <vector>
#include <string>

namespace merlict {
//=================================
class EventIoWrapper: public BasicWrapper {

	eventio::Run corsika_run;
public:

	EventIoWrapper(const std::string path);
	bool has_still_photons_left()const;
	std::vector<Photon> next(random::Generator* prng);	
};

}  // namespace merlict

#endif // __PhotonsReaderEventIoWrapper_H_INCLUDED__ 
