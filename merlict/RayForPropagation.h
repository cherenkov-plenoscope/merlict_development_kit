// Copyright 2014 Sebastian A. Mueller
#ifndef MERLICT_RAYFORPROPAGATION_H_
#define MERLICT_RAYFORPROPAGATION_H_

#include <vector>
#include <stdint.h>
#include <string>
#include "merlict/Ray.h"
#include "merlict/Intersection.h"
#include "merlict/SurfaceEntity.h"

namespace merlict {

enum Interaction {
    PRODUCTION,
    ABSORPTION_IN_VOID,

    ABSORPTION_IN_MEDIUM,
    ABSORPTION_ON_SURFACE,

    FRESNEL_REFLECTION_ON_SURFACE,
    REFLECTION_ON_SURFACE,

    REFRACTION_TO_OUTSIDE,
    REFRACTION_TO_INSIDE,

    SCATTERING
};

std::string interaction_str(Interaction type);

class RayForPropagation :public Ray{
    friend class TrajectoryFactory;

 protected:
    std::vector<Intersection> intersection_history;
    std::vector<Interaction> interaction_history;

 public:
    int32_t simulation_truth_id;
    // The sumilation truth identifier is a encoding additional information of
    // the photon.
    //
    // If simulation_truth_id >= 0,
    // encodes the index of the input photons e.g. out of a CORSIKA Cherenkov
    // photon file

    RayForPropagation();
    RayForPropagation(const Vec3 support, const Vec3 direction);
    void push_back_intersection_and_interaction(
        const Intersection& interact,
        const Interaction type);
    std::string str()const;
    double accumulated_distance()const;
    unsigned int num_interactions()const;
    const Intersection& intersection_at(const unsigned int index)const;
    Interaction final_interaction()const;
    const Intersection& final_intersection()const;
    virtual double time_of_flight()const;
    Vec3 final_intersection_incident_direction_wrt_frame()const;

 protected:
    void push_back_production();
    std::string history_str()const;
};

}  // namespace merlict

#endif  // MERLICT_RAYFORPROPAGATION_H_
