// Copyright 2014 Sebastian A. Mueller
#ifndef VISUAL_CAMERADEVICE_H_
#define VISUAL_CAMERADEVICE_H_

#include <iostream>
#include <string>
#include <sstream>
#include "Core/Rot3.h"
#include "Core/Vec3.h"
#include "Core/HomTra3.h"
#include "CameraRay.h"
#include "Image.h"

namespace Visual {

class CameraDevice{
 public:
    CameraDevice(
        const std::string name,
        const unsigned int sensor_cols,
        const unsigned int sensor_rows);
    virtual void acquire_image(
        const Frame* world,
        const Config* visual_config) = 0;
    virtual void update_position(const Vec3 position);
    virtual void update_orientation(const Rot3 rotation);
    virtual void update_position_and_orientation(
        const Vec3 position,
        const Rot3 rotation);
    void set_pointing_direction(
        Vec3 camera_pointing_direction_in_World,
        Vec3 camera_image_upwards_image_dir_in_world);
    virtual void set_FoV_in_rad(const double field_of_view);
    double get_FoV_in_rad()const;
    Vec3 get_normalized_pointing_get_direction()const;
    Vec3 direction_to_the_right_of_the_camera()const;
    Ray get_optical_axis_in_world()const;
    std::string get_name()const;
    std::string str()const;
    virtual const Image* get_image()const;
    Vec3 get_position_in_world()const;
    Rot3 get_rotation_in_world()const;
    Vec3 get_image_upwards_direction_in_world_frame()const;
    unsigned int get_number_of_sensor_cols()const;
    unsigned int get_number_of_sensor_rows()const;

 protected:
    const std::string name;
    Image image;
    HomTra3 camera2root;
    Vec3 position;
    Rot3 rotation;
    Vec3 pointing;
    CameraRay optical_axis;
    double field_of_view;
    void set_position_and_orientation(
        const Vec3 position,
        const Rot3 rotation);
    std::string get_camera_print()const;
    void assert_field_of_view_is_valid(const double field_of_view)const;
    void update_optical_axis_and_orientation();
};

}  // namespace Visual

#endif  // VISUAL_CAMERADEVICE_H_
