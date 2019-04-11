#include "docopt/docopt.h"
#include "merlict/merlict.h"
#include "merlict_json/json.h"
#include "merlict_portal_plenoscope/json_to_plenoscope.h"
#include "merlict_viewer/FlyingCamera.h"
namespace ml = merlict;

static const char USAGE[] =
R"(Show a scenery

  Usage:
    show --scenery=PATH [--config=PATH]
    show (-h | --help)
    show --version

  Options:
    -s --scenery=PATH             Scenery file path.
    -c --config=PATH              Visual config file path.
    -h --help                     Show this screen.
    --version                     Show version.
)";

int main(int argc, char* argv[]) {
  try {
    std::map<std::string, docopt::value> args = docopt::docopt(
      USAGE,
      {argv + 1, argv + argc},
      true,        // show help if requested
      "0.1");  // version string

    ml::ospath::Path scenery_path = ml::ospath::Path(
      args.find("--scenery")->second.asString());

    plenoscope::PlenoscopeScenery scenery;

    plenoscope::json::append_to_frame_in_scenery(
      &scenery.root, &scenery, scenery_path.path);

    ml::visual::Config visual_config;
    if (args.find("--config")->second) {
      visual_config = ml::json::load_visual_config(
        args.find("--config")->second.asString());}

    scenery.root.init_tree_based_on_mother_child_relations();
    ml::visual::FlyingCamera free(&scenery.root, &visual_config);

  } catch (std::exception &error) {
    std::cerr << error.what();
  }
  return 0;
}