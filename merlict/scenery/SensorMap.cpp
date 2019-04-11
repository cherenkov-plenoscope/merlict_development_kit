// Copyright 2014 Sebastian A. Mueller
#include "merlict/scenery/SensorMap.h"
#include <sstream>
#include <utility>


namespace merlict {

void SensorMap::add(const uint64_t key, const Frame* frame) {
    assert_not_in_use_yet(key);
    sensor::Sensor* sens = new sensor::Sensor(key, frame);
    sensors.push_back(sens);
    sensor_map.insert(
        std::pair<uint64_t, sensor::Sensor*>(key, sens));
}

sensor::Sensor* SensorMap::get(const uint64_t key) {
    assert_has(key);
    return sensor_map.find(key)->second;
}

bool SensorMap::has(const uint64_t key)const {
    return sensor_map.find(key) != sensor_map.end();
}

void SensorMap::assert_has(const uint64_t key)const {
    if (!has(key)) {
        std::stringstream info;
        info << "SensorMap " << __FILE__ << ", " << __LINE__ << "\n";
        info << "There is no sensor with id '" << key << "'. \n";
        throw NoSuchKey(info.str());
    }
}

void SensorMap::assert_not_in_use_yet(const uint64_t key) {
    if (has(key)) {
        std::stringstream info;
        info << "SensorMap " << __FILE__ << ", " << __LINE__ << "\n";
        info << "The key '" << key << "' is already in use.\n";
        throw KeyAlreadyInUse(info.str());
    }
}

SensorMap::~SensorMap() {
    for (sensor::Sensor* sens : sensors)
        delete sens;
}

}  // namespace merlict
