// Copyright 2014 Sebastian A. Mueller
#include "Scenery/ColorMap.h"
#include <utility>
#include <sstream>
using std::stringstream;
using std::string;
using std::map;

namespace relleums {

bool ColorMap::has(const string key)const {
    return colors.find(key) != colors.end();
}

Color* ColorMap::get(const string key) {
    assert_has(key);
    return &colors.find(key)->second;
}

void ColorMap::add(const string key, const Color color) {
    assert_not_in_use_yet(key);
    colors.insert(std::pair<string, Color>(key, color));
}

void ColorMap::assert_has(const string key)const {
    if (!has(key)) {
        stringstream info;
        info << "ColorMap " << __FILE__ << ", " << __LINE__ << "\n";
        info << "There is no color called '" << key << "'. \n";
        throw NoSuchKey(info.str());
    }
}

void ColorMap::assert_not_in_use_yet(const string key) {
    if (has(key)) {
        stringstream info;
        info << "ColorMap " << __FILE__ << ", " << __LINE__ << "\n";
        info << "The key '" << key << "' is already in use.\n";
        throw KeyAlreadyInUse(info.str());
    }
}

}  // namespace relleums
