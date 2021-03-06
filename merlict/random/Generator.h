// Copyright 2014 Sebastian A. Mueller
#ifndef MERLICT_RANDOM_GENERATOR_H_
#define MERLICT_RANDOM_GENERATOR_H_

#include <stdint.h>
#include "merlict/Vec3.h"

namespace merlict {
namespace random {

class Generator {
 protected:
    uint64_t seed_;

 public:
    uint64_t seed()const;
    virtual double uniform() = 0;
    virtual uint64_t create_seed() = 0;
    virtual void set_seed(const uint64_t seed);
    Vec3 get_point_on_xy_disc_within_radius(const double r);
    Vec3 get_point_on_xy_disc_within_radius_slow(const double r);
    double expovariate(const double rate);
    virtual double normal(const double mean, const double std_dev) = 0;
};

}  // namespace random
}  // namespace merlict

#endif  // MERLICT_RANDOM_GENERATOR_H_
