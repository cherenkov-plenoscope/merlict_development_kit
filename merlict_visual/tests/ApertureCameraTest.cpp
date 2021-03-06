// Copyright 2014 Sebastian A. Mueller
#include "merlict/tests/catch.hpp"
#include "merlict/merlict.h"
#include "merlict_visual/visual.h"
namespace ml = merlict;


TEST_CASE("ApertureCameraTest: set_up", "[merlict]") {
  ml::visual::ApertureCamera Mamiya645("Mamiya645", 640, 480);
  Mamiya645.set_pos_rot_fov(
  	ml::Vec3(0, 0, -5),
  	ml::Rot3(0, 0, 0),
  	ml::deg2rad(45));
  Mamiya645.set_fstop_sensorwidth_focus(2.3, 6e-2, 999.);
}
