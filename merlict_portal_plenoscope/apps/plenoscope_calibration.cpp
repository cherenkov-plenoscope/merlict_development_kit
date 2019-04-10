// Copyright 2015 Sebastian A. Mueller
#include <experimental/filesystem>
#include <iostream>
#include "docopt/docopt.h"
#include "merlict_portal_plenoscope/calibration/Calibrator.h"
#include "merlict_portal_plenoscope/json_to_plenoscope.h"
#include "merlict_corsika/corsika.h"
#include "merlict/merlict.h"
namespace fs = std::experimental::filesystem;
namespace ml = merlict;


static const char USAGE[] =
R"(Light-field-calibration for the Portal Cherenkov-plenoscope

    Usage:
      plenoscope-calibration -s=SCENERY_PATH -n=NUMBER_MEGA_PHOTONS -o=OUTPUT_PATH
      plenoscope-calibration (-h | --help)
      plenoscope-calibration --version

    Options:
      -s --scenery=SCENERY_PATH                     Scenery directory path.
      -n --num_mega_photons=NUMBER_MEGA_PHOTONS     Number count [Mega Photons]
                                                    of calibration photons to be
                                                    emitted.
      -o --output=OUTPUT_PATH                       Output path.
      -h --help                                     Show this screen.
      --version                                     Show version.
)";

int main(int argc, char* argv[]) {
    try {
        std::map<std::string, docopt::value> args = docopt::docopt(
            USAGE,
            { argv + 1, argv + argc },
            true,        // show help if requested
            "0.1");  // version string

        int num_mega_photons = 0;
        try {
            num_mega_photons = ml::txt::to_int(
                args.find("--num_mega_photons")->second.asString());
        } catch (std::invalid_argument &error) {
            std::stringstream info;
            info << __FILE__ << ", " << __LINE__ << "\n";
            info << "Expected ";
            info << "'--num_mega_photons', '-n' to be an integer number, ";
            info << "but actual: " << args.find("--num_mega_photons"
                )->second.asString();
            throw std::invalid_argument(info.str());
        }

        if (num_mega_photons <= 0.0) {
            std::stringstream info;
            info << __FILE__ << ", " << __LINE__ << "\n";
            info << "Expected '--num_mega_photons', ";
            info << "'-n' to be >= 0, but actual: " << num_mega_photons;
            throw std::invalid_argument(info.str());
        }

        ml::ospath::Path out_path(args.find("--output")->second.asString());
        ml::ospath::Path scenery_path(args.find("--scenery")->second.asString());

        // 1) create output directory
        fs::create_directory(out_path.path);
        // 2) copy input into output directory
        ml::ospath::Path input_copy_path = ml::ospath::join(out_path.path, "input");
        fs::create_directory(input_copy_path.path);
        fs::copy(
            scenery_path.path,
            ml::ospath::join(input_copy_path.path, "scenery"));
        // 3) use the copied input files
        scenery_path = ml::ospath::join(input_copy_path.path, "scenery");
        ml::ospath::Path scenery_file_path = ml::ospath::join(
            scenery_path.path,
            "scenery.json");

        // SET UP SCENERY
        plenoscope::PlenoscopeScenery scenery;
        plenoscope::json::append_to_frame_in_scenery(
            &scenery.root,
            &scenery,
            scenery_file_path.path);
        scenery.root.init_tree_based_on_mother_child_relations();

        if (scenery.plenoscopes.size() == 0)
            throw std::invalid_argument(
                "There is no plenoscope in the scenery");
        else if (scenery.plenoscopes.size() > 1)
            throw std::invalid_argument(
                "There is more than one plenoscope in the scenery");
        plenoscope::PlenoscopeInScenery* pis = &scenery.plenoscopes.at(0);

        corsika::write_273_f4_to_path(
            pis->light_field_sensor_geometry.get_info_header(),
            ml::ospath::join(out_path.path, "light_field_sensor_geometry.header.bin"));

        pis->light_field_sensor_geometry.write_lixel_positions(
            ml::ospath::join(out_path.path, "lixel_positions.bin"));

        pis->light_field_sensor_geometry.write_info_header(
            ml::ospath::join(out_path.path, "info.md"));

        // CALIBRATION CONFIG
        plenoscope::calibration::Config calib_config;
        calib_config.num_blocks = num_mega_photons;
        calib_config.photons_per_block = static_cast<int>(1e6);

        // RUN PLENOSCOPE CALIBRATION
        plenoscope::calibration::Calibrator calibrator(
            calib_config,
            pis,
            &scenery.root);

        ml::random::Mt19937 prng(0);

        plenoscope::calibration::run_calibration(
            calibrator,
            ml::ospath::join(out_path.path, "lixel_statistics.bin"),
            &prng);

    } catch (std::exception &error) {
        std::cerr << error.what();
    }
    return 0;
}
