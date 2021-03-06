// Copyright 2014 Sebastian A. Mueller
#include "merlict_visual/CameraRay.h"
#include <sstream>
#include "merlict/RayAndFrame.h"
#include "merlict/Intersection.h"

namespace merlict {
namespace visual {

CameraRay::CameraRay() {}

CameraRay::CameraRay(const Vec3 support, const Vec3 direction) {
    set_support_and_direction(support, direction);
}

std::string CameraRay::str()const {
    std::stringstream out;
    out << Ray::str() << ", color: " << color.str();
    return out.str();
}

}  // namespace visual
}  // namespace merlict
