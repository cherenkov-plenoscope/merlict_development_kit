// Copyright 2014 Sebastian A. Mueller
#ifndef MERLICT_PORTAL_PLENOSCOPE_CALIBRATION_CALIBRATIONPHOTONRESULT_H_
#define MERLICT_PORTAL_PLENOSCOPE_CALIBRATION_CALIBRATIONPHOTONRESULT_H_

#include <stdint.h>

namespace plenoscope {

struct CalibrationPhotonResult {
    bool reached_sensor;
    uint32_t lixel_id;
    float x_pos_on_principal_aperture;
    float y_pos_on_principal_aperture;
    float x_tilt_vs_optical_axis;
    float y_tilt_vs_optical_axis;
    float relative_time_of_flight;

    CalibrationPhotonResult();
};

}  // namespace plenoscope

#endif  // MERLICT_PORTAL_PLENOSCOPE_CALIBRATION_CALIBRATIONPHOTONRESULT_H_
