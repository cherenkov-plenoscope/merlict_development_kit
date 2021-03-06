// Copyright 2014 Sebastian A. Mueller
#include "merlict_visual/PinHoleCamera.h"
#include <exception>
#include "Tracer.h"
#include "merlict/random/random.h"
#include "merlict_multi_thread/vitaliy_vitsentiy_thread_pool.h"

namespace merlict {
namespace visual {

void PinHoleCamera::set_pos_rot_fov(
    const Vec3 position,
    const Rot3 rotation,
    const double field_of_view
) {
    CameraDevice::set_pos_rot_fov(
        position,
        rotation,
        field_of_view);

    // calculate sensor vectors
    sensor_col = __camera2root.orientation(VEC3_UNIT_Y);
    sensor_row = __camera2root.orientation(VEC3_UNIT_X);

    /*
     calculate principal point (intersection of optical axis and
     image sensor plane)
     calculate distance of camera base and principal point
     using field_of_view angle

                 camera base
                      /|  \
                     / |  |
                    /  |  |
                   /   |  |
                  /    |  |
                 /fov/2|  |
                /______|  |
               /       |   \
              /        |   /distance_to_principal_point
             /         |  |
            /          |  |
           /       ____|  |
          /       | o  |  |
         /________|____|  /
                principal_point
        \______  ______/
               \/
              row/2
    */

    distance_to_principal_point = ((num_cols/2.0)/tan(__field_of_view/2.0));
    principal_point = __optical_axis.direction()*distance_to_principal_point;
}

std::string PinHoleCamera::pin_hole_camera_str()const {
    std::stringstream out;
    out << camera_str();
    out << "| distance to principal point: ";
    out << distance_to_principal_point << "m\n";
    return out.str();
}

CameraRay PinHoleCamera::get_ray_for_pixel_in_row_and_col(
    const int row,
    const int col
)const {
    return CameraRay(
        __position, get_direction_of_ray_for_pixel(row, col));
}

Vec3 PinHoleCamera::get_direction_of_ray_for_pixel(
    const int row,
    const int col
)const {
    return get_intersection_of_ray_on_image_sensor_for_pixel(row, col);
}

Vec3 PinHoleCamera::get_intersection_of_ray_on_image_sensor_for_pixel(
    const int row,
    const int col
)const {
    const int row_idx_on_sensor = row - num_rows/2;
    const int col_idx_on_sensor = col - num_cols/2;

    return principal_point +
        sensor_row * row_idx_on_sensor +
        sensor_col * col_idx_on_sensor;
}

Color __do_one_pixel(
    int id,
    const Frame* world,
    const Config* visual_config,
    random::Mt19937 *prng,
    const unsigned int pixel,
    const PinHoleCamera* cam
) {
    (void)id;
    const unsigned int row = pixel / cam->num_cols;
    const unsigned int col = pixel % cam->num_cols;
    CameraRay cam_ray = cam->get_ray_for_pixel_in_row_and_col(row, col);
    Tracer tracer(&cam_ray, world, visual_config, prng);
    return tracer.color;
}

void PinHoleCamera::acquire_image(
    const Frame* world,
    const Config* visual_config,
    Image* image
) {
    assert_resolution(image);
    const unsigned int num_pixel = num_cols*num_rows;

    uint64_t num_threads = std::thread::hardware_concurrency();
    ctpl::thread_pool pool(num_threads);
    random::Mt19937 prng;
    std::vector<std::future<Color>> results(num_pixel);

    for (uint64_t i = 0; i < num_pixel; ++i) {
        results[i] = pool.push(
            __do_one_pixel,
            world,
            visual_config,
            &prng,
            i,
            this);
    }

    for (uint64_t i = 0; i < num_pixel; i ++) {
        const unsigned int row = i / num_cols;
        const unsigned int col = i % num_cols;
        image->set_col_row(col, row, results[i].get());
    }
}

}  // namespace visual
}  // namespace merlict
