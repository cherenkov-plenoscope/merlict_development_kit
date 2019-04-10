// Copyright 2014 Sebastian A. Mueller
#ifndef MERLICT_PORTAL_PLENOSCOPE_CALIBRATION_CONFIG_H_
#define MERLICT_PORTAL_PLENOSCOPE_CALIBRATION_CONFIG_H_

#include <stdint.h>

namespace plenoscope {
namespace calibration {

struct Config {
    uint32_t num_blocks;
    uint32_t photons_per_block;
    Config();
};

}  // namespace calibration
}  // namespace plenoscope

#endif  // MERLICT_PORTAL_PLENOSCOPE_CALIBRATION_CONFIG_H_
