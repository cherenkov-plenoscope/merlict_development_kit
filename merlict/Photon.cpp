// Copyright 2014 Sebastian A. Mueller
#include <sstream>
#include "merlict/Photon.h"
#include "merlict/PhysicalConstants.h"
#include "merlict/RayAndFrame.h"

namespace merlict {

Photon::Photon(): wavelength(0.) {}

Photon::Photon(
    const Vec3 support,
    const Vec3 direction,
    const double _wavelength
):
    RayForPropagation(support, direction),
    wavelength(_wavelength
) {
    assert_wavelength_is_positive();
}

void Photon::assert_wavelength_is_positive()const {
    if (wavelength <= 0.0) {
        std::stringstream info;
        info << __FILE__ << " " << __LINE__ << "\n";
        info << "Expected photon wavelength lambda > 0.0nm, but actual ";
        info << "it is: " << wavelength*1e9 << "nm\n";
        throw std::invalid_argument(info.str());
    }
}

std::string Photon::str()const {
    std::stringstream out;
    out << RayForPropagation::str();
    out << "wavelength " << wavelength*1e9 << "nm\n";
    return out.str();
}

double Photon::time_to_travel_distance_in_refractive_index(
    const double distance_in_medium,
    const double refractive_index
)const {
    return refractive_index*distance_in_medium/VACUUM_SPPED_OF_LIGHT;
}

double Photon::time_of_flight()const {
    double time_of_flight = 0.0;

    unsigned int i = 0;
    for (const Intersection &intersec : intersection_history) {
        if (i != 0)
            time_of_flight += time_to_travel_distance_in_refractive_index(
                intersec.distance_to_ray_support(),
                intersec.refractive_index_coming_from(wavelength));
        i++;
    }
    return time_of_flight;
}

}  // namespace merlict
