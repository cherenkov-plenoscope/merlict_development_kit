// Copyright 2014 Sebastian A. Mueller
#ifndef MCTRACER_CORSIKA_EVENTIO_PHOTONFACTORY_H_
#define MCTRACER_CORSIKA_EVENTIO_PHOTONFACTORY_H_

#include <array>
#include "Core/Photon.h"
#include "Core/Random/Random.h"

namespace EventIo {

class PhotonFactory {
    static const unsigned int IDX_X_POS = 0;
    static const unsigned int IDX_Y_POS = 1;
    static const unsigned int IDX_X_DIR_COSINE = 2;
    static const unsigned int IDX_Y_DIR_COSINE = 3;
    static const unsigned int IDX_ARRIVAL_TIME = 4;
    static const unsigned int IDX_Z_EMISSION_HEIGHT = 5;
    static const unsigned int IDX_PHOTON_WEIGTH = 6;
    static const unsigned int IDX_WAVELENGTH = 7;
    bool passed_through_atmosphere_flag = false;
    unsigned int id;
    std::array<float, 8> corsika_photon;

 public:
    class BadCorsikaPhotonShape : public std::runtime_error {
        using runtime_error::runtime_error;
    };
    class BadPhotonWeight : public std::runtime_error {
        using runtime_error::runtime_error;
    };
    PhotonFactory(
        const std::array<float, 8>& _corsika_photon,
        const unsigned int id,
        Random::Generator *prng);
    bool passed_atmosphere()const;
    Photon get_photon();
    Vec3 causal_get_direction()const;
    Vec3 intersection_with_xy_floor_plane()const;
    double ray_parameter_for_production_point()const;
    double x_pos_on_xy_plane_in_m()const;
    double y_pos_on_xy_plane_in_m()const;
    double production_height_in_m()const;
    double wavelength_in_m()const;
    double production_distance_offset()const;
    double relative_arrival_time_on_ground()const;
    float x_pos_on_world_x_y_plane_in_cm()const;
    float y_pos_on_world_x_y_plane_in_cm()const;
    float wavelength_in_nm()const;
    float production_height_in_cm()const;
    float photon_survival_probability()const;

 private:
    void check_once_if_passed_atmosphere(Random::Generator *prng);
    void assert_corsika_photon_has_correct_length()const;
    void assert_photon_weight_is_between_zero_and_one()const;
};

}  // namespace EventIo

#endif  // MCTRACER_CORSIKA_EVENTIO_PHOTONFACTORY_H_
