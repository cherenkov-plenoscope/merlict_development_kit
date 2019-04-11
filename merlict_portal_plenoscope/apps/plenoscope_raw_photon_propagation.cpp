// Copyright 2015 Sebastian A. Mueller
#include <experimental/filesystem>
#include <iostream>
#include "docopt/docopt.h"
#include "merlict/merlict.h"
#include "merlict_corsika/eventio.h"
#include "merlict_corsika/corsika.h"
#include "merlict_corsika/PhotonFactory.h"
#include "merlict_portal_plenoscope/night_sky_background/Light.h"
#include "merlict_portal_plenoscope/EventHeader.h"
#include "merlict_portal_plenoscope/SimulationTruthHeader.h"
#include "merlict_portal_plenoscope/night_sky_background/Injector.h"
#include "merlict_portal_plenoscope/json_to_plenoscope.h"
#include "merlict_signal_processing/signal_processing.h"
namespace fs = std::experimental::filesystem;
namespace sp = signal_processing;
namespace ml = merlict;


static const char USAGE[] =
R"(Propagation of raw photons for the Portal Cherenkov-plenoscope

    Usage:
      plenoscope-raw-photon-propagation -l=PATH -c=PATH -i=PATH -o=PATH [-r=SEED] [--all_truth]
      plenoscope-raw-photon-propagation (-h | --help)
      plenoscope-raw-photon-propagation --version

    Options:
      -l --lixel=PATH           Light field calibration directory of the plenoscope.
      -c --config=PATH          Config path.
      -i --input=PATH           Photon input path.
      -o --output=PATH          Output path.
      -r --random_seed=SEED     Seed for pseudo random number generator.
      --all_truth               Write all simulation truth avaiable into the output.
      -h --help                 Show this screen.
      --version                 Show version.
)";

int main(int argc, char* argv[]) {
    try {
    std::map<std::string, docopt::value> args = docopt::docopt(
        USAGE,
        { argv + 1, argv + argc },
        true,        // show help if requested
        "0.1");    // version string

    ml::ospath::Path lixel_calib_path(args.find("--lixel")->second.asString());
    ml::ospath::Path config_path(args.find("--config")->second.asString());
    ml::ospath::Path input_path(args.find("--input")->second.asString());
    ml::ospath::Path output_path(args.find("--output")->second.asString());
    const bool export_all_simulation_truth =
        args.find("--all_truth")->second.asBool();

    // 1) create output directory
    fs::create_directory(output_path.path);
    // 2) copy input into output directory
    ml::ospath::Path input_copy_path = ml::ospath::join(
        output_path.path,
        "input");
    fs::create_directory(input_copy_path.path);
    fs::copy(
        config_path.path,
        ml::ospath::join(input_copy_path.path, "propagation_config.json"));
    fs::create_hard_link(
        input_path.path,
        ml::ospath::join(input_copy_path.path, input_path.basename));
    fs::copy(
        lixel_calib_path.path,
        ml::ospath::join(input_copy_path.path, "plenoscope"),
        fs::copy_options::recursive);

    config_path = ml::ospath::join(input_copy_path.path, "propagation_config.json");
    lixel_calib_path = ml::ospath::join(
        ml::ospath::join(input_copy_path.path, "plenoscope"),
        "lixel_statistics.bin");
    input_path = ml::ospath::join(input_copy_path.path, input_path.basename);
    ml::ospath::Path scenery_path = ml::ospath::join(
        ml::ospath::join(input_copy_path.path, "plenoscope"),
        "input/scenery/scenery.json");



    //--------------------------------------------------------------------------
    //  111
    // 1111      set up simulation
    //   11
    //   11
    //   11
    //   11
    // 111111 11
    //--------------------------------------------------------------------------
    // BASIC SETTINGS
    ml::PropagationConfig settings;
    settings.max_num_interactions_per_photon = 10;

    //--------------------------------------------------------------------------
    // INIT PRNG
    ml::random::Mt19937 prng;
    if (args.find("--random_seed")->second)
        prng.set_seed(args.find("--random_seed")->second.asLong());

    //--------------------------------------------------------------------------
    // SET UP SCENERY
    plenoscope::PlenoscopeScenery scenery;
    plenoscope::json::append_to_frame_in_scenery(
        &scenery.root,
        &scenery,
        scenery_path.path);
    scenery.root.init_tree_based_on_mother_child_relations();

    if (scenery.plenoscopes.size() == 0)
        throw std::invalid_argument("There is no plenoscope in the scenery");
    else if (scenery.plenoscopes.size() > 1)
        throw std::invalid_argument(
            "There is more then one plenoscope in the scenery");
    plenoscope::PlenoscopeInScenery* pis = &scenery.plenoscopes.at(0);

    ml::sensor::Sensors* light_field_channels = pis->light_field_channels;

    //--------------------------------------------------------------------------
    // load light field calibration result
    std::vector<plenoscope::calibration::LixelStatistic>
        optics_calibration_result =
            plenoscope::calibration::read(lixel_calib_path.path);

    // assert number os sub_pixel matches simulated plenoscope
    if (light_field_channels->size() != optics_calibration_result.size()) {
        std::stringstream info;
        info << "The light field calibration results, read from file '";
        info << lixel_calib_path.path;
        info << "', do no not match the plenoscope simulated here.\n";
        info << "Expected number of light field channels: ";
        info << light_field_channels->size();
        info << ", but actual: " << optics_calibration_result.size();
        info << "\n";
        throw std::invalid_argument(info.str());
    }

    ml::json::Object plcfg = ml::json::load(config_path.path);
    //--------------------------------------------------------------------------
    // INIT NIGHT SKY BACKGROUND
    ml::json::Object nsb_obj = plcfg.obj("night_sky_background_ligth");
    const ml::function::Func1 nsb_flux_vs_wavelength =
        ml::json::json_to_linear_interpol_function(
            nsb_obj.obj("flux_vs_wavelength"));

    plenoscope::night_sky_background::Light nsb(
        &pis->light_field_sensor_geometry,
        &nsb_flux_vs_wavelength);
    const double nsb_exposure_time = nsb_obj.f8("exposure_time");

    //--------------------------------------------------------------------------
    // SET UP PhotoElectricConverter
    ml::json::Object pec_obj = plcfg.obj("photo_electric_converter");
    const ml::function::Func1 quantum_efficiency_vs_wavelength =
        ml::json::json_to_linear_interpol_function(
            pec_obj.obj("quantum_efficiency_vs_wavelength"));

    sp::PhotoElectricConverter::Config converter_config;
    converter_config.dark_rate = pec_obj.f8("dark_rate");
    converter_config.probability_for_second_puls = pec_obj.f8(
        "probability_for_second_puls");
    converter_config.quantum_efficiency_vs_wavelength =
        &quantum_efficiency_vs_wavelength;

    sp::PhotoElectricConverter::Converter sipm_converter(
        &converter_config);

    //--------------------------------------------------------------------------
    // SET SINGLE PULSE OUTPUT
    ml::json::Object phs_obj = plcfg.obj("photo_electric_converter");
    const double time_slice_duration = phs_obj.f8("time_slice_duration");
    const double arrival_time_std = phs_obj.f8(
        "single_photon_arrival_time_resolution");

    //--------------------------------------------------------------------------
    //  2222
    // 22  22      run the simulation
    //    22
    //   22
    //  22
    // 22
    // 222222 22
    //--------------------------------------------------------------------------
    // open raw photon file
    std::vector<ml::Photon> photons = ml::raw_matrix2photons(
        ml::tsvio::gen_table_from_file(input_path.path));

    //--------------------------------------------------------------------------
    // propagate photons
    const unsigned int event_counter = 1;

    ml::propagate_photons_in_frame_with_config(
        &photons, &scenery.root, &settings, &prng);

    light_field_channels->clear_history();
    light_field_channels->assign_photons(&photons);

    std::vector<std::vector<sp::PipelinePhoton>> photon_pipelines =
        sp::get_photon_pipelines(light_field_channels);

    //-----------------------------
    // Night Sky Background photons
    plenoscope::night_sky_background::inject_nsb_into_photon_pipeline(
        &photon_pipelines,
        nsb_exposure_time,
        &optics_calibration_result,
        &nsb,
        &prng);

    //--------------------------
    // Photo Electric conversion
    std::vector<std::vector<sp::ElectricPulse>> electric_pipelines;
    electric_pipelines.reserve(photon_pipelines.size());
    for (
        std::vector<sp::PipelinePhoton> ph_pipe :
        photon_pipelines
    ) {
        electric_pipelines.push_back(
            sipm_converter.get_pulse_pipeline_for_photon_pipeline(
                ph_pipe,
                nsb_exposure_time,
                &prng));
    }

    //-------------------------
    // Single-photon-extraction
    sp::PhotonStream::Stream record;
    record.time_slice_duration = time_slice_duration;
    record.photon_stream = sp::extract_pulses(
        electric_pipelines,
        time_slice_duration,
        arrival_time_std,
        &prng);

    //-------------
    // export event
    ml::ospath::Path event_output_path = ml::ospath::join(
        output_path.path,
        std::to_string(event_counter));
    fs::create_directory(event_output_path.path);

    sp::PhotonStream::write(
        record.photon_stream,
        record.time_slice_duration,
        ml::ospath::join(
            event_output_path.path,
            "raw_light_field_sensor_response.phs"));

    plenoscope::EventHeader event_header;
    event_header.set_event_type(plenoscope::EventTypes::SIMULATION);
    event_header.set_trigger_type(
        plenoscope::TriggerType::EXTERNAL_RANDOM_TRIGGER);
    event_header.set_plenoscope_geometry(
        pis->light_field_sensor_geometry.config);
    corsika::write_273_f4_to_path(
        event_header.raw,
        ml::ospath::join(
            event_output_path.path,
            "event_header.bin"));

    //-------------
    // export Simulation Truth
    ml::ospath::Path event_mc_truth_path = ml::ospath::join(
        event_output_path.path,
        "simulation_truth");
    fs::create_directory(event_mc_truth_path.path);
    std::array<float, 273> run_header;
    for (
        unsigned int i = 0;
        i < run_header.size();
        i++
    ) {run_header[i] = 0.0f;}
    run_header[0] = corsika::str2float("RUNH");
    run_header[1] = 1.0f;  // run-number
    run_header[2] = 20180101.0f;  // date
    run_header[3] = -1.0f;  // program version
    run_header[4] = 1.0f;  // number observation-levels
    run_header[5] = 5e3f;  // height observation-levels
    corsika::write_273_f4_to_path(
        run_header,
        ml::ospath::join(
            event_mc_truth_path.path,
            "corsika_run_header.bin"));
    std::array<float, 273> evt_header;
    for (
        unsigned int i = 0;
        i < evt_header.size();
        i++
    ) {evt_header[i] = 0.0f;}
    evt_header[0] = corsika::str2float("EVTH");
    evt_header[1] = static_cast<float>(event_counter + 1);  // evt-number
    evt_header[2] = -1.0f;  // particle id
    evt_header[3] = -1.0f;  // energy
    evt_header[4] = 1.0f;  // number observation-levels
    evt_header[5] = 5e3f;  // height observation-levels
    evt_header[44-1] = run_header[1];  // run-number
    evt_header[45-1] = run_header[2];  // date
    evt_header[46-1] = run_header[3];  // program version
    evt_header[47-1] = run_header[4];  // number observation-levels
    evt_header[48-1] = run_header[5];  // height observation-levels
    corsika::write_273_f4_to_path(
        evt_header,
        ml::ospath::join(event_mc_truth_path.path, "corsika_event_header.bin"));
    plenoscope::SimulationTruthHeader sim_truth_header;
    sim_truth_header.set_random_seed_of_run(prng.seed());
    corsika::write_273_f4_to_path(
        sim_truth_header.raw,
        ml::ospath::join(
            event_mc_truth_path.path,
            "mctracer_event_header.bin"));

    if (export_all_simulation_truth) {
        sp::PhotonStream::write_simulation_truth(
            record.photon_stream,
            ml::ospath::join(
                event_mc_truth_path.path,
                "detector_pulse_origins.bin"));

        std::vector<std::array<float, 8>> raw_photons;
        for (unsigned int p = 0; p < photons.size(); p++) {
            const double lambda =
                photons.at(p).support().z/photons.at(p).direction().z;

            std::array<float, 8> raw_photon;
            raw_photon[0] = 1e2*(
                photons.at(p).support().x +
                lambda*photons.at(p).direction().x);  // x
            raw_photon[1] = 1e2*(
                photons.at(p).support().y +
                lambda*photons.at(p).direction().y);  // y
            raw_photon[2] = photons.at(p).direction().x;  // cx
            raw_photon[3] = photons.at(p).direction().y;  // cy
            raw_photon[4] = 0.0f;  // relative arrival-time on ground
            raw_photon[5] = 1e2*photons.at(p).support().z;
            // production height
            raw_photon[6] = 1.0f;  // survival probability
            raw_photon[7] = 1e9*photons.at(p).wavelength;
            raw_photons.push_back(raw_photon);
        }

        eventio::write_photon_bunches_to_path(
            raw_photons,
            ml::ospath::join(
                event_mc_truth_path.path,
                "air_shower_photon_bunches.bin"));
    }
    } catch (std::exception &error) {
        std::cerr << error.what();
    }
    return 0;
}
