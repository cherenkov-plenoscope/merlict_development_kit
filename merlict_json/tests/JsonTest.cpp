// Copyright 2014 Sebastian A. Mueller
#include "merlict/tests/catch.hpp"
#include "merlict_json/niels_lohmann_json.hpp"
#include "merlict_json/json.h"
#include "merlict/merlict.h"
#include "merlict_visual/visual.h"
namespace nlo = nlohmann;
namespace ml = merlict;


TEST_CASE("JsonTest: nlohmann_getter", "[merlict]") {
    nlo::json j = R"({"f8": -0.898})"_json;
    CHECK(-0.898 == j["f8"].get<double>());
    CHECK(0 == j["f8"].get<int64_t>());
    CHECK_THROWS_AS(j["f8"].get<std::string>(), std::exception);
    CHECK(1u == j.size());
}

TEST_CASE("JsonTest: object_wrapper_simple", "[merlict]") {
    nlo::json j = R"({"f8": -0.898})"_json;
    ml::json::Object obj(j);
    CHECK(-0.898 == obj.f8("f8"));
}

TEST_CASE("JsonTest: object_wrapper_multiple_objects", "[merlict]") {
    nlo::json j = R"({"f8": -0.898, "hans": {"A":1, "B":2}})"_json;
    ml::json::Object obj(j);
    CHECK(-0.898 == obj.f8("f8"));
    CHECK(1 == obj.obj("hans").i8("A"));
    CHECK(2 == obj.obj("hans").i8("B"));
}

TEST_CASE("JsonTest: object_wrapper_lists", "[merlict]") {
    nlo::json j = R"({"f8": -0.898, "peter": [1, 2, 3]})"_json;
    ml::json::Object obj(j);
    CHECK(-0.898 == obj.f8("f8"));
    CHECK_THROWS_AS(obj.obj("peter").i8("A"), ml::json::MissingKey);
    CHECK(3u == obj.obj("peter").size());
    CHECK(1 == obj.obj("peter").i8(0));
    CHECK(2 == obj.obj("peter").i8(1));
    CHECK(3 == obj.obj("peter").i8(2));
    CHECK_THROWS_AS(obj.obj("peter").i8(3), ml::json::ListTooShort);
}

TEST_CASE("JsonTest: object_wrapper_vec3", "[merlict]") {
    nlo::json j = R"({"f8": -0.898, "peter": [1, 2, 3]})"_json;
    ml::json::Object obj(j);
    CHECK(-0.898 == obj.f8("f8"));
    CHECK(ml::Vec3(1, 2, 3) == obj.vec3("peter"));
}

TEST_CASE("JsonTest: object_wrapper_bad_vec3", "[merlict]") {
    nlo::json j = R"({"peter": [1, 2]})"_json;
    ml::json::Object obj(j);
    CHECK_THROWS_AS(obj.vec3("peter"), ml::json::BadTriple);
}

TEST_CASE("JsonTest: empty_path", "[merlict]") {
    const std::string path = "";
    ml::Scenery s;
    CHECK_THROWS_AS(ml::json::append_to_frame_in_scenery(&s.root, &s, path), std::runtime_error);
}

TEST_CASE("JsonTest: mini_scenery_with_stl", "[merlict]") {
    const std::string path =  "merlict/tests/resources/scenery/mini_scenery.json";
    ml::Scenery s;
    ml::json::append_to_frame_in_scenery(&s.root, &s, path);
    s.root.init_tree_based_on_mother_child_relations();
    //visual::Config cfg;
    //visual::FlyingCamera(&s.root, &cfg);
}

TEST_CASE("JsonTest: valid_color", "[merlict]") {
  auto j = R"({"red": [255, 0, 0]})"_json;
  ml::json::Object o(j);
  ml::Color c = o.color("red");
  CHECK(255 == c.r);
  CHECK(0 == c.g);
  CHECK(0 == c.b);
}

TEST_CASE("JsonTest: color_rg_no_b", "[merlict]") {
  auto j = R"({"red": [255, 0]})"_json;
  ml::json::Object o(j);
  CHECK_THROWS_AS(o.color("red"), ml::json::BadTriple);
}

TEST_CASE("JsonTest: color_no_array_but_string", "[merlict]") {
  auto j = R"({"red": "woot"})"_json;
  ml::json::Object o(j);
  CHECK_THROWS_AS(o.color("red"), ml::json::BadTriple);
}

TEST_CASE("JsonTest: fine_colors", "[merlict]") {
    auto j = R"(
    {
      "colors": [
        {"name":"red", "rgb":[255, 0, 0]},
        {"name":"green", "rgb":[0, 255, 0]},
        {"name":"blue", "rgb":[0, 0, 255]}
      ]
    }
    )"_json;
    ml::ColorMap cmap;
    ml::json::assert_key(j, "colors");
    ml::json::add_colors(&cmap, j["colors"]);
    CHECK(cmap.has("red"));
    CHECK(ml::Color(255, 0, 0) == *cmap.get("red"));
    CHECK(cmap.has("green"));
    CHECK(ml::Color(0, 255, 0) == *cmap.get("green"));
    CHECK(cmap.has("blue"));
    CHECK(ml::Color(0, 0, 255) == *cmap.get("blue"));
}

TEST_CASE("JsonTest: empty_colors", "[merlict]") {
    nlo::json j = R"({"colors": {}})"_json;
    ml::ColorMap cmap;
    ml::json::assert_key(j, "colors");
    ml::json::add_colors(&cmap, j["colors"]);
    CHECK(0u == cmap.colors.size());
}

TEST_CASE("JsonTest: parse_mini_scenery", "[merlict]") {
    auto jscenery = R"(
    { "functions":[
        { "name":"leaf_reflection",
          "argument_versus_value":[
            [200e-9, 0.5],
            [1200e-9, 0.5]]},
        { "name":"pole_dull",
          "argument_versus_value":[
            [200e-9, 0.0],
            [1200e-9, 0.0]]}
      ],
      "colors":
      [
        {"name":"grass_green", "rgb":[22, 91, 49]},
        {"name":"orange", "rgb":[255, 91, 49]},
        {"name":"leaf_green", "rgb":[22, 200, 49]},
        {"name":"wood_brown", "rgb":[200, 200, 0]}
      ],
      "children": [
        {
          "type": "Frame",
          "name": "tree",
          "pos": [5, 5, -2],
          "rot": [0, 0, 0],
          "children": [
            {
              "type": "Sphere",
              "name": "leaf_ball",
              "pos": [0, 0, 3],
              "rot": [0, 0, 0],
              "radius": 1.5,
              "surface": {
                "outer_color": "leaf_green",
                "inner_color": "leaf_green"},
              "children": []
            },
            {
              "type": "Cylinder",
              "name": "pole",
              "start_pos": [0, 0, 0],
              "end_pos": [0, 0, 1.5],
              "radius": 0.25,
              "surface": {
                "outer_color": "wood_brown",
                "inner_color": "wood_brown"},
              "children": []
            }
          ]
        }
      ]
    }
    )"_json;

    ml::json::Object o(jscenery);
    ml::Scenery s;
    ml::json::append_to_frame_in_scenery(&s.root, &s, o);

    CHECK("root" == s.root.get_name());
    CHECK(s.root.has_children());

    CHECK(s.functions.has("leaf_reflection"));
    CHECK(s.functions.has("pole_dull"));

    CHECK(s.colors.has("grass_green"));
    CHECK(s.colors.has("leaf_green"));
    CHECK(s.colors.has("orange"));

    const std::vector<ml::Frame*>* children = s.root.get_children();
    REQUIRE(1u == children->size());
    CHECK("tree" == children->at(0)->get_name());
}

TEST_CASE("JsonTest: linear_interpolation_function", "[merlict]") {
    auto j = R"(
    [
      {
        "name":"foo",
        "argument_versus_value":[
          [0, 5],
          [1, 4],
          [2, 3],
          [3, 2],
          [4, 1],
          [5, 0]
        ]
      }
    ]
    )"_json;

    ml::json::Object o(j);
    CHECK(o.size() == 1);
    const ml::json::Object &fo = o.obj(0);
    CHECK(fo.key("name"));
    CHECK(fo.key("argument_versus_value"));

    ml::FunctionMap functions;
    ml::json::add_functions(&functions, o);
    CHECK(functions.has("foo"));
    CHECK(functions.get("foo")->limits.upper == 5);
    CHECK(functions.get("foo")->limits.lower == 0);
    CHECK(functions.get("foo")->evaluate(0.0) == Approx(5.0).margin(1e-9));
    CHECK(functions.get("foo")->evaluate(0.5) == Approx(4.5).margin(1e-9));
    CHECK(functions.get("foo")->evaluate(1.0) == Approx(4.0).margin(1e-9));
    CHECK(functions.get("foo")->evaluate(4.999) == Approx(0.0).margin(2e-3));
}

TEST_CASE("JsonTest: Annulus", "[merlict]") {
    auto j = R"(
    {
      "type": "Annulus",
      "name": "ring",
      "pos": [0, 0, 3],
      "rot": [0, 1, 0],
      "outer_radius": 1.5,
      "inner_radius": 1.0,
      "surface": {},
      "children": []
    }
    )"_json;
    ml::json::Object o(j);
    ml::Scenery s;
    ml::Frame* a = ml::json::add_Annulus(&s.root, &s, o);
    CHECK("ring" == a->get_name());
    CHECK(ml::Vec3(0, 0, 3) == a->position_in_mother());
    CHECK(ml::Rot3(0, 1, 0) == a->rotation_in_mother());
    CHECK(0u == a->get_children()->size());
}

TEST_CASE("JsonTest: Cylinder_with_rot_and_pos", "[merlict]") {
    auto j = R"(
    {
      "type": "Cylinder",
      "name": "cyl",
      "pos": [0, 0, 3],
      "rot": [0, 1, 0],
      "radius": 1.5,
      "length": 1.0,
      "surface": {},
      "children": []
    }
    )"_json;
    ml::json::Object o(j);
    ml::Scenery s;
    ml::Frame* a = ml::json::add_Cylinder(&s.root, &s, o);
    CHECK("cyl" == a->get_name());
    CHECK(ml::Vec3(0, 0, 3) == a->position_in_mother());
    CHECK(ml::Rot3(0, 1, 0) == a->rotation_in_mother());
    CHECK(0u == a->get_children()->size());
}

TEST_CASE("JsonTest: Cylinder_with_start_pos_and_end_pos", "[merlict]") {
    auto j = R"(
    {
      "type": "Cylinder",
      "name": "cyl",
      "start_pos": [0, 0, 0],
      "end_pos": [0, 0, 1],
      "radius": 1.5,
      "surface": {},
      "children": []
    }
    )"_json;
    ml::json::Object o(j);
    ml::Scenery s;
    ml::Frame* a = ml::json::add_Cylinder(&s.root, &s, o);
    CHECK("cyl" == a->get_name());
    CHECK(0u == a->get_children()->size());
}

TEST_CASE("JsonTest: Triangle", "[merlict]") {
    auto j = R"(
    {
      "type": "Triangle",
      "name": "tri",
      "pos": [0, 0, 1.5],
      "rot": [-1.57, 0.0, -1.57],
      "Ax": -1, "Ay": 0, "Bx": 1, "By": 0, "Cx": 0, "Cy": 1,
      "surface": {},
      "children": []
    }
    )"_json;
    ml::json::Object o(j);
    ml::Scenery s;
    ml::Frame* a = ml::json::add_Triangle(&s.root, &s, o);
    CHECK("tri" == a->get_name());
    CHECK(0u == a->get_children()->size());
}

TEST_CASE("JsonTest: Disc", "[merlict]") {
    auto j = R"(
    {
      "type": "Disc",
      "name": "didi",
      "pos": [0, 0, 1.5],
      "rot": [-1.57, 0.0, -1.57],
      "radius": 1,
      "surface": {},
      "children": []
    }
    )"_json;
    ml::Scenery s;
    ml::json::Object o(j);
    ml::Frame* a = ml::json::add_Disc(&s.root, &s, o);
    CHECK("didi" == a->get_name());
    CHECK(0u == a->get_children()->size());
}

TEST_CASE("JsonTest: What_is_key", "[merlict]") {
    auto j = R"(
    {
      "key": {
        "val1": 1,
        "val2": 2
      }
    }
    )"_json;
    j["key"]["val1"].get<double>();
    CHECK(1 == j["key"]["val1"]);
}

TEST_CASE("JsonTest: PropagationConfig", "[merlict]") {
    auto j = R"(
    {
      "max_num_interactions_per_photon": 1337
    }
    )"_json;
    ml::json::Object o(j);
    ml::PropagationConfig cfg = ml::json::to_PropagationConfig(o);
    CHECK(1337u == cfg.max_num_interactions_per_photon);
}

TEST_CASE("JsonTest: PointSource", "[merlict]") {
  auto j = R"(
  {
    "point_source": {
      "opening_angle": 0.01,
      "num_photons": 137
    },
    "pos": [0, 2, 0],
    "rot": [0, 0, 0]
  }
  )"_json;
  ml::json::Object o(j);
  std::vector<ml::Photon> phs = ml::json::to_photons(o);
  CHECK(137u == phs.size());
  for (const ml::Photon &ph: phs) {
    CHECK(0.0 == ph.support().x);
    CHECK(2.0 == ph.support().y);
    CHECK(0.0 == ph.support().z);

    CHECK(ph.direction().x == Approx(0.0).margin(0.1));
    CHECK(ph.direction().y == Approx(0.0).margin(0.1));
    CHECK(ph.direction().z == Approx(1.0).margin(0.1));
  }
}

TEST_CASE("JsonTest: PointSource_rotated", "[merlict]") {
  auto j = R"(
  {
    "point_source": {
      "opening_angle": 0.01,
      "num_photons": 13
    },
    "pos": [0, 2, 0],
    "rot": [0, 1.5705, 0]
  }
  )"_json;
  ml::json::Object o(j);
  std::vector<ml::Photon> phs = ml::json::to_photons(o);
  CHECK(13u == phs.size());
  for (const ml::Photon &ph: phs) {
    CHECK(0.0 == ph.support().x);
    CHECK(2.0 == ph.support().y);
    CHECK(0.0 == ph.support().z);

    CHECK(ph.direction().x == Approx(-1.0).margin(0.1));
    CHECK(ph.direction().y == Approx(0.0).margin(0.1));
    CHECK(ph.direction().z == Approx(0.0).margin(0.1));
  }
}

TEST_CASE("JsonTest: ParallelDisc_rotated", "[merlict]") {
  auto j = R"(
  {
    "parallel_disc": {
      "disc_radius": 2.5,
      "num_photons": 13
    },
    "pos": [0, 0, 0],
    "rot": [0, 1.5705, 0]
  }
  )"_json;
  ml::json::Object o(j);
  std::vector<ml::Photon> phs = ml::json::to_photons(o);
  CHECK(13u == phs.size());
  for (const ml::Photon &ph: phs) {
    CHECK(ph.direction().x == Approx(-1.0).margin(0.1));
    CHECK(ph.direction().y == Approx(0.0).margin(0.1));
    CHECK(ph.direction().z == Approx(0.0).margin(0.1));
  }
}

TEST_CASE("JsonTest: visual_config", "[merlict]") {
  auto j = R"(
  {
    "max_interaction_depth": 41,
    "preview": {
      "rows": 256,
      "cols": 144,
      "scale": 5
    },
    "snapshot": {
      "cols": 1280,
      "rows":  720,
      "noise_level": 25,
      "focal_length_over_aperture_diameter": 0.95,
      "image_sensor_size_along_a_row": 0.07
    },
    "global_illumination": {
      "on": true,
      "incoming_direction": [0.2, 0.3, 1.0]
    },
    "sky_dome": {
      "path": "",
      "color": [0, 0, 0]
    },
    "photon_trajectories": {
      "radius": 0.15
    }
  }
  )"_json;
  ml::json::Object o(j);
  ml::visual::Config cfg = ml::json::to_visual_config(o, "./");
  CHECK(cfg.max_interaction_depth == 41u);
  CHECK(cfg.preview.rows == 256u);
  CHECK(cfg.preview.cols == 144u);
  CHECK(cfg.preview.scale == 5u);
  CHECK(cfg.snapshot.cols == 1280u);
  CHECK(cfg.snapshot.rows == 720u);
  CHECK(cfg.snapshot.noise_level == 25u);
  CHECK(cfg.snapshot.focal_length_over_aperture_diameter == 0.95);
  CHECK(cfg.snapshot.image_sensor_size_along_a_row == 0.07);
  CHECK(cfg.global_illumination.on);
  CHECK(cfg.global_illumination.incoming_direction == ml::Vec3(.2, .3, 1.));
  CHECK(cfg.photon_trajectories.radius == 0.15);
}

TEST_CASE("JsonTest: linear_interpolation_function2", "[merlict]") {
  auto j = R"(
  {
    "argument_versus_value": [
      [266e-9, 0.03],
      [277e-9, 0.1],
      [283e-9, 0.2],
      [300e-9, 0.254]
    ]
  }
  )"_json;
  ml::json::Object o(j);
  std::vector<std::vector<double>> f = ml::json::json_to_vec_of_vecs(
    o.obj("argument_versus_value"));
  REQUIRE(4u == f.size());
  REQUIRE(2u == f.at(0).size());
  CHECK(266e-9 == f.at(0).at(0));
  CHECK(0.03 == f.at(0).at(1));
  REQUIRE(2u == f.at(1).size());
  CHECK(277e-9 == f.at(1).at(0));
  CHECK(0.1 == f.at(1).at(1));
  REQUIRE(2u == f.at(2).size());
  CHECK(283e-9 == f.at(2).at(0));
  CHECK(0.2 == f.at(2).at(1));
  REQUIRE(2u == f.at(3).size());
  CHECK(300e-9 == f.at(3).at(0));
  CHECK(0.254 == f.at(3).at(1));
}
