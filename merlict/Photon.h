// Copyright 2014 Sebastian A. Mueller
#ifndef MERLICT_PHOTON_H_
#define MERLICT_PHOTON_H_

namespace merlict {class Propagator;}

#include <string>
#include "merlict/RayForPropagation.h"
#include "Fresnel.h"

namespace merlict {

class Photon :public RayForPropagation{
    friend class Propagator;

 public:
    double wavelength;
    Photon();
    Photon(
        const Vec3 support,
        const Vec3 direction,
        const double wavelength);
    std::string str()const;
    double time_of_flight()const;

    double time_to_travel_distance_in_refractive_index(
        const double distance_in_medium,
        const double refractive_index)const;
    void assert_wavelength_is_positive()const;
};

}  // namespace merlict

#endif  // MERLICT_PHOTON_H_
