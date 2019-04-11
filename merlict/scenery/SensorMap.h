// Copyright 2014 Sebastian A. Mueller
#ifndef MERLICT_SCENERY_SENSORMAP_H_
#define MERLICT_SCENERY_SENSORMAP_H_

#include <stdint.h>
#include <map>
#include <vector>
#include "ResourceMap.h"
#include "merlict/sensor/Sensor.h"

namespace merlict {

struct SensorMap: public ResourceMap {
    std::map<uint64_t, sensor::Sensor*> sensor_map;
    std::vector<sensor::Sensor> sensors_storage;
    std::vector<sensor::Sensor*> sensors;
    void add(const uint64_t key, const Frame* frame);
    bool has(const uint64_t key)const;
    sensor::Sensor* get(const uint64_t key);
    void assert_has(const uint64_t key)const;
    void assert_not_in_use_yet(const uint64_t key);
    ~SensorMap();
};

}  // namespace merlict

#endif  // MERLICT_SCENERY_SENSORMAP_H_
