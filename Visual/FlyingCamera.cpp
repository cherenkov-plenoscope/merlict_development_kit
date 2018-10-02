// Copyright 2014 Sebastian A. Mueller
#include "Visual/FlyingCamera.h"
#include <cv.h>
#include <highgui.h>
#include <sstream>
#include <fstream>
#include <opencv2/opencv.hpp>
#include "Core/RayAndFrame.h"
#include "Core/Intersection.h"
#include "Visual/PortablePixMap.h"


using std::cout;
using std::stringstream;
using std::string;

namespace Visual {

void image_to_opencv_image(const Image& image, cv::Mat* out) {
    for (unsigned int col = 0; col < image.number_cols; col++) {
        for (unsigned int row = 0; row < image.number_rows; row++) {
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
    fov_operator(CameraOperator::FieldOfView(&camera)),
    translation_operator(CameraOperator::Translation(&camera)),
    rotation_operator(CameraOperator::Rotation(&camera)),
    stereo_operator(CameraOperator::Stereo3D(&camera)) {
    reset_camera();
    time_stamp.update_now();
    create_display();
    enter_interactive_display();
}

FlyingCamera::~FlyingCamera() {
    destroy_display();
}

void FlyingCamera::reset_camera() {
    translation_operator.set_default_position(Vec3(0.0, 0.0, 0.0));
    rotation_operator.set_default_rotation(Rot3(0.0, Deg2Rad(-90.0), 0.0));
    fov_operator.set_default();
}

void FlyingCamera::enter_interactive_display() {
    UserInteraction::print_welcome_screen();
    print_help();

    int user_input_key = 0;
    bool key_stroke_requires_image_update = true;

    while (!UserInteraction::is_Escape_key(user_input_key)) {
        user_input_counter++;

        if (time_to_print_help())
            print_help();

        if (key_stroke_requires_image_update)
            update_display_preview();

        key_stroke_requires_image_update = true;
        user_input_key = wait_for_user_key_stroke();

        switch (user_input_key) {
            case 't': toggle_stereo3D();
            break;
            case 'w': translation_operator.move_forward();
            break;
            case 's': translation_operator.move_back();
            break;
            case 'a': translation_operator.move_left();
            break;
            case 'd': translation_operator.move_right();
            break;
            case 'q': translation_operator.move_up();
            break;
            case 'e': translation_operator.move_down();
            break;
            case 'n': fov_operator.increase_when_possible();
            break;
            case 'm': fov_operator.decrease_when_possible();
            break;
            case 'i': rotation_operator.look_further_up_when_possible();
            break;
            case 'k': rotation_operator.look_further_down_when_possible();
            break;
            case 'j': rotation_operator.look_left();
            break;
            case 'l': rotation_operator.look_right();
            break;
            case 'x': stereo_operator.increase_stereo_offset();
            break;
            case 'y': stereo_operator.decrease_stereo_offset();
            break;
            case 'g': translation_operator.move_to(
                UserInteraction::get_Vec3());
            break;
            case UserInteraction::space_key: {
                update_display();
                key_stroke_requires_image_update = false;
            }
            break;
            case 'p': {
                cout << world->get_tree_print() << "\n";
                key_stroke_requires_image_update = false;
            }
            break;
            case 'h': {
                print_help();
                key_stroke_requires_image_update = false;
            };
            break;
            default: key_stroke_requires_image_update = false;
        }
    }
}

int FlyingCamera::wait_for_user_key_stroke() {
    return cvWaitKey(0);
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
    cout << "Full resolution image "
    << camera.number_cols <<"x"
    << camera.number_rows <<", "
    << camera.number_cols*camera.number_rows/1e6
    << " MPixel\n";
    acquire_image_with_camera(&camera, &image);
    image_to_opencv_image(image, &display_image);
    cv::imshow(display_name, display_image);
}

void FlyingCamera::update_display_preview() {
    camera_preview.set_FoV_in_rad(camera.get_FoV_in_rad());
    camera_preview.update_position_and_orientation(
        camera.get_position_in_world(),
        camera.get_rotation_in_world());
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

void FlyingCamera::toggle_stereo3D() {
    stereo3D = !stereo3D;
    cout << "Stereo 3D : " << (stereo3D ? "On" : "Off") << "\n";
}

string FlyingCamera::get_snapshot_filename() {
    snapshot_counter++;
    stringstream filename;
    filename << time_stamp.yyyy() << time_stamp.mm() << time_stamp.dd() << "_";
    filename << time_stamp.HH() << time_stamp.MM() << time_stamp.SS();
    filename << "_" << snapshot_counter << ".ppm";
    return filename.str();
}

ApertureCamera FlyingCamera::get_aperture_camera_based_on_camera()const {
    ApertureCamera apcam("Imax70mm",
        visual_config->snapshot.cols,
        visual_config->snapshot.rows);
    apcam.set_fStop_sesnorWidth(
        visual_config->snapshot.focal_length_over_aperture_diameter,
        visual_config->snapshot.image_sensor_size_along_a_row);
    apcam.set_FoV_in_rad(camera.get_FoV_in_rad());
    apcam.update_position_and_orientation(
        camera.get_position_in_world(),
        camera.get_rotation_in_world());
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
    apcam.set_focus_to(object_distance_to_focus_on);
    cout << apcam.str();
    Image apcam_img = Image(apcam.number_cols, apcam.number_rows);
    acquire_image_with_camera(&apcam, &apcam_img);
    ppm::write_image_to_path(apcam_img, get_snapshot_filename());
}

void FlyingCamera::acquire_image_with_camera(CameraDevice* cam, Image* img) {
    if (stereo3D) {
        CameraOperator::Stereo3D op(cam);
        op.use_same_stereo_offset_as(&stereo_operator);
        op.aquire_stereo_image(world, visual_config, img);
    } else {
        cam->acquire_image(world, visual_config, img);
    }
}

void FlyingCamera::print_ray_for_pixel_col_row(int col, int row) {
    Ray probing_ray = camera.get_ray_for_pixel_in_row_and_col(row, col);
    Intersection intersec = RayAndFrame::first_intersection(
        &probing_ray,
        world);
    UserInteraction::ClearScreen();
    stringstream out;
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
    cout << out.str();
}

string FlyingCamera::get_intersection_info_print(
    const Intersection intersec
)const {
    stringstream out;
    out << "| Object: ";
    out << intersec.get_object()->get_path_in_tree_of_frames() << "\n";
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
    out << StringTools::place_first_infront_of_each_new_line_of_second(
        "| ",
        intersec.get_object()->str());
    return out.str();
}

void FlyingCamera::print_help()const {
    UserInteraction::ClearScreen();
    stringstream out;

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
    cout << out.str();
}

void FlyingCamera::continue_with_new_scenery_and_visual_config(
    const Frame *world,
    const Config *visual_config
) {
    this->world = world;
    this->visual_config = visual_config;
    enter_interactive_display();
}

}  // namespace Visual
