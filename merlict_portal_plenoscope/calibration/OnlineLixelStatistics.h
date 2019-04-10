// Copyright 2014 Sebastian A. Mueller
#ifndef MERLICT_PORTAL_PLENOSCOPE_CALIBRATION_ONLINESTATISTICS_H_
#define MERLICT_PORTAL_PLENOSCOPE_CALIBRATION_ONLINESTATISTICS_H_

#include "./OnlineStatistics.h"

namespace plenoscope {
namespace calibration {

struct OnlineLixelStatistics {
    unsigned int count;
    OnlineStatistics cx;
    OnlineStatistics cy;
    OnlineStatistics x;
    OnlineStatistics y;
    OnlineStatistics timed_delay;
    OnlineLixelStatistics();
};

}  // namespace calibration
}  // namespace plenoscope

#endif  // MERLICT_PORTAL_PLENOSCOPE_CALIBRATION_ONLINESTATISTICS_H_
