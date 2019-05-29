// Copyright 2014 Sebastian A. Mueller
#ifndef VISUAL_CAMERAOPERATOR_STEREO3D_H_
#define VISUAL_CAMERAOPERATOR_STEREO3D_H_

namespace merlict {class Frame;}

#include <string>
#include "Verbosity.h"

namespace merlict {
namespace visual {
namespace camera_operator {

class Stereo3D: public Verbosity {
 public:
    explicit Stereo3D(CameraDevice* camera_to_work_with);
    void aquire_stereo_image(
        const Frame* world,
        const Config* visual_config,
        Image* stereo_image);
    void increase_stereo_offset();
    void decrease_stereo_offset();
    void use_same_stereo_offset_as(const Stereo3D *college);
    double stereo_offset()const;

 private:
    CameraDevice* camera;
    Image left_image;
    Image right_image;
    Vec3 initial_camera_pos;
    Vec3 left_camera_pos;
    Vec3 right_camera_pos;

    double stereo_offset_in_m = 6e-2;
    Vec3 offset_to_the_right()const;
    void print_stereo_offset_manipulation(const std::string status)const;
};

}  // namespace camera_operator
}  // namespace visual
}  // namespace merlict

#endif  // VISUAL_CAMERAOPERATOR_STEREO3D_H_
