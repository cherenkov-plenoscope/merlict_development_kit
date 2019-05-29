// Copyright 2014 Sebastian A. Mueller
#include "FlyingCamera.h"
#include <cv.h>
#include <highgui.h>
#include <sstream>
#include <fstream>
#include <opencv2/opencv.hpp>


namespace merlict {
namespace visual {

static const unsigned int SPACE_KEY = 32;
static const unsigned int ESCAPE_KEY = 27;

std::string welcome_screen() {
    std::stringstream out;
    //               1         2         3         4         5         6
    //      123456789012345678901234567890123456789012345678901234567890
    out << "                                                            \n";
    out << "                          merlict                           \n";
    out << "                                                            \n";
    out << "         propagating photons in complex sceneries           \n";
    out << "                   Astro particle physics                   \n";
    out << "                                                            \n";
    out << "                  Sebastian Achim Mueller                   \n";
    out << "                        ETH Zurich                          \n";
    out << "                                                            \n";
    return out.str();
}

void clear_screen() {
    std::cout << std::string(5, '\n');
}

Vec3 cin_Vec3() {
    Vec3 vec;
    std::string input;
    while (true) {
        std::cout << "Enter 3D vector: '[x,y,z]'\n>";
        getline(std::cin, input);
        try {
            vec = txt::to_Vec3(input);
            break;
        } catch (...) {}
    }
    return Vec3(vec.x, vec.y, vec.z);
}

void image_to_opencv_image(const Image& image, cv::Mat* out) {
    for (unsigned int col = 0; col < image.num_cols; col++) {
        for (unsigned int row = 0; row < image.num_rows; row++) {
            const unsigned idx = image._idx(col, row);
            cv::Vec3b intensity;
            intensity.val[0] = image.raw.at(idx).b;
            intensity.val[1] = image.raw.at(idx).g;;
            intensity.val[2] = image.raw.at(idx).r;;
            out->at<cv::Vec3b>(row, col) = intensity;
        }
    }
}

FlyingCamera::FlyingCamera(
    const Frame *_world,
    const Config *_visual_config
):
    display_name(
        std::string("Monte Carlo Ray Tracer")),
    world(_world),
    visual_config(_visual_config),
    camera_preview(
        PinHoleCamera(
            "Cam",
            visual_config->preview.cols,
            visual_config->preview.rows)),
    camera(
        PinHoleCamera(
            "CamFull",
            visual_config->preview.cols*visual_config->preview.scale,
            visual_config->preview.rows*visual_config->preview.scale)),
    image(
        Image(
            visual_config->preview.cols*visual_config->preview.scale,
            visual_config->preview.rows*visual_config->preview.scale)),
    image_preview(
        Image(
            visual_config->preview.cols,
            visual_config->preview.rows)),
    display_image(
        cv::Mat(
            visual_config->preview.rows*visual_config->preview.scale,
            visual_config->preview.cols*visual_config->preview.scale,
            CV_8UC3)),
    fov_operator(camera_operator::FieldOfView(&camera)),
    translation_operator(camera_operator::Translation(&camera)),
    rotation_operator(camera_operator::Rotation(&camera)),
    stereo_operator(camera_operator::Stereo3D(&camera)) {
    reset_camera();
    time_stamp.update_now();
    create_display();
    enter_interactive_display();
}

FlyingCamera::~FlyingCamera() {
    destroy_display();
}

void FlyingCamera::reset_camera() {
    translation_operator.set_default_position(Vec3(0., 0., 0.));
    rotation_operator.set_default_rotation(Rot3(0., deg2rad(-90.), 0.));
    fov_operator.set_default();
}

void FlyingCamera::enter_interactive_display() {
    std::cout << welcome_screen();
    print_help();

    int user_input_key = 0;
    bool key_stroke_requires_image_update = true;

    while (user_input_key != ESCAPE_KEY) {
        user_input_counter++;

        if (time_to_print_help())
            print_help();

        if (key_stroke_requires_image_update) {
            std::cout << camera_status() << std::endl;
            update_display_preview();
        }

        key_stroke_requires_image_update = true;
        user_input_key = wait_for_user_key_stroke();

        switch (user_input_key) {
            case ESCAPE_KEY:
                break;
            case 't':
                toggle_stereo3D();
                break;
            case 'w':
                translation_operator.move_forward();
                break;
            case 's':
                translation_operator.move_back();
                break;
            case 'a':
                translation_operator.move_left();
                break;
            case 'd':
                translation_operator.move_right();
                break;
            case 'q':
                translation_operator.move_up();
                break;
            case 'e':
                translation_operator.move_down();
                break;
            case 'n':
                fov_operator.increase_when_possible();
                break;
            case 'm':
                fov_operator.decrease_when_possible();
                break;
            case 'i':
                rotation_operator.look_further_up_when_possible();
                break;
            case 'k':
                rotation_operator.look_further_down_when_possible();
                break;
            case 'j':
                rotation_operator.look_left();
                break;
            case 'l':
                rotation_operator.look_right();
                break;
            case 'x':
                stereo_operator.increase_stereo_offset();
                break;
            case 'y':
                stereo_operator.decrease_stereo_offset();
                break;
            case 'g':
                translation_operator.move_to(cin_Vec3());
                break;
            case SPACE_KEY:
                update_display();
                key_stroke_requires_image_update = false;
                break;
            case 'p':
                std::cout << world->tree_str() << "\n";
                key_stroke_requires_image_update = false;
                break;
            case 'h':
                print_help();
                key_stroke_requires_image_update = false;
                break;
            default:
                std::cout << "Key Press unknown: ";
                std::cout << user_input_key << std::endl;
                key_stroke_requires_image_update = false;
        }
    }
}

int FlyingCamera::wait_for_user_key_stroke() {
    int key = cvWaitKey(0);
    if (key == -1) {
        return key;
    } else {
        return key & 255;
    }
}

bool FlyingCamera::time_to_print_help() {
    if (user_input_counter > 15) {
        user_input_counter = 0;
        return true;
    } else {
        return false;
    }
}

void FlyingCamera::create_display() {
    cv::namedWindow(display_name, CV_WINDOW_AUTOSIZE);
    FlyingCamera* p = this;
    cv::setMouseCallback(
        display_name.c_str(),
        mouse_button_event,
        reinterpret_cast<void *>(p));
}

void FlyingCamera::destroy_display() {
    cv::destroyWindow(display_name);
}

void FlyingCamera::update_display() {
    std::cout << "Full resolution image "
    << camera.num_cols <<"x"
    << camera.num_rows <<", "
    << camera.num_cols*camera.num_rows/1e6
    << " MPixel\n";
    acquire_image_with_camera(&camera, &image);
    image_to_opencv_image(image, &display_image);
    cv::imshow(display_name, display_image);
}

void FlyingCamera::update_display_preview() {
    camera_preview.set_pos_rot_fov(
        camera.position(),
        camera.rotation(),
        camera.field_of_view());
    acquire_image_with_camera(&camera_preview, &image_preview);
    scale_up(image_preview, visual_config->preview.scale, &image);
    image_to_opencv_image(image, &display_image);
    cv::imshow(display_name, display_image);
}

void FlyingCamera::mouse_button_event(
    int event, int col, int row, int flags, void *param
) {
    (void)flags;
    FlyingCamera* p = reinterpret_cast<FlyingCamera*>(param);
    if (event == cv::EVENT_LBUTTONDOWN)
        p->print_ray_for_pixel_col_row(col, row);
    else if (event == cv::EVENT_RBUTTONDOWN)
        p->aquire_image_focused_on_pixel_col_row(col, row);
    else
        return;
}

std::string FlyingCamera::camera_status()const {
    std::stringstream out;
    out <<  "  pos-x/m    pos-y/m    pos-z/m     "
            "rot-y/deg  rot-z/deg      fov/deg   3D-off/m\n";
    char buffer[100];
    snprintf(buffer, 100, "% .3e % .3e % .3e   % .3e % .3e   % .3e % .3e",
        camera_preview.position().x,
        camera_preview.position().y,
        camera_preview.position().z,
        rad2deg(camera_preview.rotation().rot_y()),
        rad2deg(camera_preview.rotation().rot_z()),
        rad2deg(camera_preview.field_of_view()),
        stereo_operator.stereo_offset());
    out << buffer;
    return out.str();
}


void FlyingCamera::toggle_stereo3D() {
    stereo3D = !stereo3D;
    std::cout << "Stereo 3D : " << (stereo3D ? "On" : "Off") << "\n";
}


std::string FlyingCamera::get_snapshot_filename() {
    snapshot_counter++;
    std::stringstream filename;
    filename << time_stamp.yyyy() << time_stamp.mm() << time_stamp.dd() << "_";
    filename << time_stamp.HH() << time_stamp.MM() << time_stamp.SS();
    filename << "_" << snapshot_counter << ".ppm";
    return filename.str();
}

ApertureCamera FlyingCamera::get_aperture_camera_based_on_camera()const {
    ApertureCamera apcam("Imax70mm",
        visual_config->snapshot.cols,
        visual_config->snapshot.rows);
    apcam.set_fstop_sensorwidth_focus(
        visual_config->snapshot.focal_length_over_aperture_diameter,
        visual_config->snapshot.image_sensor_size_along_a_row,
        999.9);
    apcam.set_pos_rot_fov(
        camera.position(),
        camera.rotation(),
        camera.field_of_view());
    return apcam;
}

void FlyingCamera::aquire_image_focused_on_pixel_col_row(int col, int row) {
    Ray probing_ray = camera.get_ray_for_pixel_in_row_and_col(row, col);
    DistanceMeter dist_meter(&probing_ray, world);
    double object_distance_to_focus_on;
    if (dist_meter.faces_an_object)
        object_distance_to_focus_on = dist_meter.distance_to_closest_object;
    else
        object_distance_to_focus_on = 9e99;  // a very large distance
    ApertureCamera apcam = get_aperture_camera_based_on_camera();
    apcam.set_fstop_sensorwidth_focus(
        apcam.f_stop_number(),
        apcam.sensor_width(),
        object_distance_to_focus_on);
    std::cout << apcam.str();
    Image apcam_img = Image(apcam.num_cols, apcam.num_rows);
    acquire_image_with_camera(&apcam, &apcam_img);
    write_image_to_path(apcam_img, get_snapshot_filename());
}

void FlyingCamera::acquire_image_with_camera(CameraDevice* cam, Image* img) {
    if (stereo3D) {
        camera_operator::Stereo3D op(cam);
        op.use_same_stereo_offset_as(&stereo_operator);
        op.aquire_stereo_image(world, visual_config, img);
    } else {
        cam->acquire_image(world, visual_config, img);
    }
}

void FlyingCamera::print_ray_for_pixel_col_row(int col, int row) {
    Ray probing_ray = camera.get_ray_for_pixel_in_row_and_col(row, col);
    Intersection intersec = rays_first_intersection_with_frame(
        &probing_ray,
        world);
    clear_screen();
    std::stringstream out;
    //      0        1         2         3         4         5         6
    //      123456789012345678901234567890123456789012345678901234567890

    out << " _Info_for_Pixel_(_" << col << "_|_" << row << "_)______________\n";
    out << "|\n";
    out << "| Ray emitted by camera:\n";
    out << "| " << probing_ray.str() << "\n";
    out << "|\n";
    if (intersec.does_intersect())
        out << get_intersection_info_print(intersec);
    else
        out << "| No intersection with any object.\n";
    out << "|___________________________________________________________\n";
    std::cout << out.str();
}

std::string FlyingCamera::get_intersection_info_print(
    const Intersection intersec
)const {
    std::stringstream out;
    out << "| Object: ";
    out << intersec.object()->path_in_tree() << "\n";
    out << "| Distance to first intersection: ";
    out << intersec.distance_to_ray_support() << "m\n";
    out << "|\n";
    out << "| In frame of intersecting object\n";
    out << "| | intesection point: ";
    out << intersec.position_in_object_frame().str() << "\n";
    out << "| | surface normal   : ";
    out << intersec.surface_normal_in_object_frame().str() << "\n";
    out << "| | facing surface   : ";
    out << (intersec.from_outside_to_inside() ? "outside" : "inside") << "\n";
    out << "|\n";
    out << "| In world frame\n";
    out << "| | intesection point: ";
    out << intersec.position_in_root_frame().str() << "\n";
    out << "| | surface normal   : ";
    out << intersec.surface_normal_in_root_frame().str() << "\n";
    out << "|\n";
    out << txt::place_first_infront_of_each_new_line_of_second(
        "| ",
        intersec.object()->str());
    return out.str();
}

void FlyingCamera::print_help()const {
    clear_screen();
    std::stringstream out;

//  0         1         2         3         4         5         6         7
//   12345678901234567890123456789012345678901234567890123456789012345678901234
    out <<
    "_Position______________________   _Orientation___________________\n"
    " move forward............[ w ]     look up.................[ i ]\n"
    " move backward...........[ s ]     look down...............[ k ]\n"
    " move left...............[ a ]     look left...............[ j ]\n"
    " move right..............[ d ]     look right..............[ l ]\n"
    " move up.................[ q ]\n"
    " move down...............[ e ]\n"
    " move to custom pos......[ g ]\n"
    "\n"
    "_Stereo3D_left:red_right:blue__   _Field_of_View_________________\n"
    " on/off..................[ t ]     increace................[ n ]\n"
    " increase................[ x ]     decreace................[ m ]\n"
    " decrease................[ y ]\n"
    "                                  _interaction__________________\n"
    "_Aperture_camera_______________    print help..............[ h ]\n"
    " manual focus..[ right mouse ]     print geometry tree.....[ p ]\n"
    "                                   exit/next photon........[ESC]\n"
    "\n"
    "[  space key  ] full resolution.\n"
    "[ left mouse  ] click image for additional information.\n"
    "[ right mouse ] click image for manual focus with snapshot.\n\n";
    std::cout << out.str();
}

void FlyingCamera::continue_with_new_scenery_and_visual_config(
    const Frame *world,
    const Config *visual_config
) {
    this->world = world;
    this->visual_config = visual_config;
    enter_interactive_display();
}

}  // namespace visual
}  // namespace merlict
