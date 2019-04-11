#include <iostream>
#include "docopt/docopt.h"
#include "merlict/merlict.h"
#include "merlict_json/json.h"
namespace ml = merlict;


static const char USAGE[] =
R"(Photon-propagation

    Usage:
      propagate -s=PATH -c=PATH -i=PATH -o=PATH [-r=SEED] [-b]
      propagate (-h | --help)
      propagate --version

    Options:
      -s --scenery=PATH         Scenery path.
      -c --config=PATH          Config path to steer the simulation.
      -i --input=PATH           Photon path (e.g. a CORSIKA run).
      -o --output=PATH          Output path.
      -r --random_seed=SEED     Seed for pseudo random number generator.
      -b --binary               Exports a binary float32 table for each sensor.
      -h --help                 Show this screen.
      --version                 Show version.
)";

int main(int argc, char* argv[]) {
    try{

    std::map<std::string, docopt::value> args = docopt::docopt(
        USAGE,
        { argv + 1, argv + argc },
        true,        // show help if requested
        "0.1");  // version string

    ml::ospath::Path out_path(args.find("--output")->second.asString());
    ml::ospath::Path scenery_path(args.find("--scenery")->second.asString());
    ml::ospath::Path photon_path(args.find("--input")->second.asString());
    ml::ospath::Path config_path(args.find("--config")->second.asString());
    const bool export_binary = args.find("--binary")->second.asBool();

    // BASIC SETTINGS
    ml::PropagationConfig settings = ml::json::load_PropagationConfig(
        config_path.path);

    // Random
    ml::random::Mt19937 prng;
    if(args.find("--random_seed")->second)
        prng.set_seed(args.find("--random_seed")->second.asLong());

    // scenery
    ml::Scenery scenery;
    ml::json::append_to_frame_in_scenery(
        &scenery.root,
        &scenery,
        scenery_path.path);
    scenery.root.init_tree_based_on_mother_child_relations();

    // sensors in scenery
    ml::sensor::Sensors sensors(scenery.sensors.sensors);

    // photon source
    std::vector<ml::Photon> photons = ml::raw_matrix2photons(
        ml::tsvio::gen_table_from_file(photon_path.path));
    const unsigned int event_counter = 1;

    // photon propagation
    ml::propagate_photons_in_frame_with_config(
        &photons,
        &scenery.root,
        &settings,
        &prng);

    // detect photons in sensors
    sensors.clear_history();
    sensors.assign_photons(&photons);

    // write each sensors to file
    for (unsigned int i = 0; i < sensors.size(); i++) {
        std::stringstream outname;
        outname << out_path.path << event_counter << "_" << i;

        if (export_binary) {
            std::ofstream out;
            out.open(outname.str(), std::ios::out | std::ios::binary);
            ml::sensor::write_arrival_information_to_file(
                &(sensors.at(i)->photon_arrival_history),
                &out);
            out.close();
        } else {
            std::stringstream header;
            header << "scenery: " << scenery_path.path << "\n";
            header << "sensor:  ";
            header << sensors.at(i)->frame->path_in_tree();
            header << ", ID: " << i << "\n";
            header << "photons: " << photon_path.path << "\n";
            header << "-------------\n";
            header << ml::sensor::arrival_table_header();

            ml::tsvio::write_table_to_file_with_header(
                ml::sensor::history_to_table(
                    sensors.at(i)->photon_arrival_history),
                outname.str(),
                header.str()
            );
        }
    }

    } catch (std::exception &error) {
        std::cerr << error.what();
    }
    return 0;
}