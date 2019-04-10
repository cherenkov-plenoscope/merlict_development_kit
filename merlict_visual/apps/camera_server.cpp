// Copyright 2018 Sebastian A. Mueller
#include <stdint.h>
#include <iostream>
#include "docopt/docopt.h"
#include "merlict/merlict.h"
#include "merlict_json/json.h"
namespace ml = merlict;


static const char USAGE[] =
R"(Show a scenery

    Usage:
      cameraserver --scenery=SCENERY_PATH [--config=CONFIG_PATH]
      cameraserver (-h | --help)
      cameraserver --version

    Options:
      -s --scenery=SCENERY_PATH     Scenery file path.
      -c --config=CONFIG_PATH       Visual config file path.
      -h --help                     Show this screen.
      --version                     Show version.
)";

const uint64_t MAGIC_INSTRUCTION_SYNC = 645;

struct ApertureCameraInstructions {
    uint64_t magic_sync;
    ml::Vec3 position;
    ml::Rot3 orientation;
    double object_distance;
    double sensor_size_along_columns;
    double field_of_view_along_columns;
    double focal_length_over_aperture_diameter;
    uint64_t num_columns;
    uint64_t num_rows;
    uint64_t noise_level;
};

ApertureCameraInstructions read_from_stream(std::istream &fin) {
    ApertureCameraInstructions inst;
    inst.magic_sync = ml::read_uint64(fin);
    double x, y, z;
    x = ml::read_float64(fin);
    y = ml::read_float64(fin);
    z = ml::read_float64(fin);
    inst.position = ml::Vec3(x, y, z);
    x = ml::read_float64(fin);
    y = ml::read_float64(fin);
    z = ml::read_float64(fin);
    inst.orientation = ml::Rot3(x, y, z);
    inst.object_distance = ml::read_float64(fin);
    inst.sensor_size_along_columns = ml::read_float64(fin);
    inst.field_of_view_along_columns = ml::read_float64(fin);
    inst.focal_length_over_aperture_diameter = ml::read_float64(fin);
    inst.num_columns = ml::read_uint64(fin);
    inst.num_rows = ml::read_uint64(fin);
    inst.noise_level = ml::read_uint64(fin);
    return inst;
}

int main(int argc, char* argv[]) {
    try {
        std::map<std::string, docopt::value> args = docopt::docopt(
            USAGE,
            { argv + 1, argv + argc },
            true,        // show help if requested
            "0.1");  // version string

        ml::ospath::Path scenery_path(
            args.find("--scenery")->second.asString());

        ml::Scenery scenery;
        ml::json::append_to_frame_in_scenery(
            &scenery.root,
            &scenery,
            scenery_path.path);

        ml::visual::Config visual_config;
        if (args.find("--config")->second) {
            visual_config = ml::json::load_visual_config(
                args.find("--config")->second.asString());
        }
        scenery.root.init_tree_based_on_mother_child_relations();

        while (true) {
            ApertureCameraInstructions ins = read_from_stream(std::cin);
            if (ins.magic_sync != MAGIC_INSTRUCTION_SYNC)
                break;

            ml::visual::Image image(
                ins.num_columns,
                ins.num_rows);

            ml::visual::ApertureCamera cam(
                "camera",
                ins.num_columns,
                ins.num_rows);
            cam.set_fstop_sensorwidth_focus(
                ins.focal_length_over_aperture_diameter,
                ins.sensor_size_along_columns,
                ins.object_distance);
            cam.set_pos_rot_fov(
                ins.position,
                ins.orientation,
                ins.field_of_view_along_columns);
            visual_config.snapshot.noise_level = ins.noise_level;
            cam.acquire_image(&scenery.root, &visual_config, &image);
            ml::visual::append_image_to_file(image, std::cout);
        }
    }catch(std::exception &error) {
        std::cerr << error.what();
    }
    return 0;
}
