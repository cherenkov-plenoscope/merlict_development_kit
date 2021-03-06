// Copyright 2014 Sebastian A. Mueller
#ifndef MERLICT_PROPAGATIONCONFIG_H_
#define MERLICT_PROPAGATIONCONFIG_H_

#include "Color.h"
#include "Vec3.h"

namespace merlict {

struct PropagationConfig {
    unsigned int max_num_interactions_per_photon;
    PropagationConfig();
};

const PropagationConfig DEFAULT_PROPAGATIONCONFIG;

}  // namespace merlict

#endif  // MERLICT_PROPAGATIONCONFIG_H_
