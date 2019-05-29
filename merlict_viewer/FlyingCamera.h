// Copyright 2014 Sebastian A. Mueller
#ifndef VISUAL_FLYINGCAMERA_H_
#define VISUAL_FLYINGCAMERA_H_

#include <cv.h>
#include <string>
#include "merlict/merlict.h"
#include "merlict_viewer/Time.h"
#include "merlict_viewer/camera_operator/camera_operator.h"

namespace merlict {
namespace visual {

class FlyingCamera {
// Fly through the scene like you want!
// This FlyingCamera class allows the user to interact with the
// pin_hole_camera during run time.
// A pin_hole_cam is created. Its position, orientation and field of
// view can be modified by the user during run time.
// The image is displayed in a additional window and not written to the
// harddisk.
// Above this one can take snapshots using a aperture camera.
// Also 3D stereo images are possible
 public:
    FlyingCamera(
        const Frame *world,
        const Config *visual_config);
    void continue_with_new_scenery_and_visual_config(
        const Frame *world,
        const Config *visual_config);
    ~FlyingCamera();

 private:
    const std::string display_name;
    const Frame *world;
    const Config *visual_config;
    PinHoleCamera camera_preview;
    PinHoleCamera camera;
    Image image;
    Image image_preview;
    cv::Mat display_image;
    camera_operator::FieldOfView fov_operator;
    camera_operator::Translation translation_operator;
    camera_operator::Rotation rotation_operator;
    camera_operator::Stereo3D stereo_operator;
    bool stereo3D = false;
    int snapshot_counter = 0;
    unsigned int user_input_counter = 0;
    Time::Stamp time_stamp;
    int wait_for_user_key_stroke();
    void toggle_stereo3D();
    void enter_interactive_display();
    void aquire_image();
    void update_display_preview();
    void update_display();
    void aquire_image_focused_on_pixel_col_row(int col, int y);
    void print_help()const;
    static void mouse_button_event(
        int event,
        int col,
        int y,
        int flags,
        void *param);
    void print_ray_for_pixel_col_row(int x, int y);
    std::string get_intersection_info_print(const Intersection intersec)const;
    void update_current_camera_position(Vec3 new_translation_World2Camera);
    ApertureCamera get_aperture_camera_based_on_camera()const;
    void create_display();
    void destroy_display();
    void reset_camera();
    std::string get_snapshot_filename();
    std::string camera_status()const;
    bool time_to_print_help();
    void acquire_image_with_camera(CameraDevice* cam, Image* img);
};

}  // namespace visual
}  // namespace merlict

#endif  // VISUAL_FLYINGCAMERA_H_
