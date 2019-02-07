// Copyright 2014 Sebastian A. Mueller
#include "gtest/gtest.h"
#include "nlohmann_json.hpp"
#include "json.h"
#include "Scenery/Scenery.h"
#include "Scenery/Primitive/Primitive.h"
//#include "Visual/Config.h"
//#include "Visual/FlyingCamera.h"
namespace nl = nlohmann;
using std::string;
using StringTools::is_equal;

class JsonTest : public ::testing::Test {};

TEST_F(JsonTest, nlohmann_getter) {
    nl::json j = R"({"f8": -0.898})"_json;
    EXPECT_EQ(j["f8"].get<double>(), -0.898);
    EXPECT_EQ(j["f8"].get<int64_t>(), 0);
    EXPECT_THROW(j["f8"].get<std::string>(), std::exception);
    EXPECT_EQ(j.size(), 1u);
}

TEST_F(JsonTest, object_wrapper_simple) {
    nl::json j = R"({"f8": -0.898})"_json;
    mct::json::Object obj(j);
    EXPECT_EQ(obj.f8("f8"), -0.898);
}

TEST_F(JsonTest, object_wrapper_multiple_objects) {
    nl::json j = R"({"f8": -0.898, "hans": {"A":1, "B":2}})"_json;
    mct::json::Object obj(j);
    EXPECT_EQ(obj.f8("f8"), -0.898);
    EXPECT_EQ(obj.obj("hans").i8("A"), 1);
    EXPECT_EQ(obj.obj("hans").i8("B"), 2);
}

TEST_F(JsonTest, object_wrapper_lists) {
    nl::json j = R"({"f8": -0.898, "peter": [1, 2, 3]})"_json;
    mct::json::Object obj(j);
    EXPECT_EQ(obj.f8("f8"), -0.898);
    EXPECT_THROW(obj.obj("peter").i8("A"), mct::json::MissingKey);
    EXPECT_EQ(obj.obj("peter").size(), 3u);
    EXPECT_EQ(obj.obj("peter").i8(0), 1);
    EXPECT_EQ(obj.obj("peter").i8(1), 2);
    EXPECT_EQ(obj.obj("peter").i8(2), 3);
    EXPECT_THROW(obj.obj("peter").i8(3), mct::json::ListTooShort);
}

TEST_F(JsonTest, object_wrapper_vec3) {
    nl::json j = R"({"f8": -0.898, "peter": [1, 2, 3]})"_json;
    mct::json::Object obj(j);
    EXPECT_EQ(obj.f8("f8"), -0.898);
    EXPECT_EQ(obj.vec3("peter"), Vec3(1, 2, 3));
}

TEST_F(JsonTest, object_wrapper_bad_vec3) {
    nl::json j = R"({"peter": [1, 2]})"_json;
    mct::json::Object obj(j);
    EXPECT_THROW(obj.vec3("peter"), mct::json::BadTriple);
}

TEST_F(JsonTest, empty_path) {
    const std::string path = "";
    Scenery s;
    EXPECT_THROW(
        mct::json::append_to_frame_in_scenery(&s.root, &s, path),
        std::runtime_error);
}

TEST_F(JsonTest, mini_scenery_with_stl) {
    const std::string path =  "json/mini_scenery.json";
    Scenery s;
    mct::json::append_to_frame_in_scenery(&s.root, &s, path);
    s.root.init_tree_based_on_mother_child_relations();
    //Visual::Config cfg;
    //Visual::FlyingCamera(&s.root, &cfg);
}

TEST_F(JsonTest, valid_color) {
  auto j = R"({"red": [255, 0, 0]})"_json;
  mct::json::Object o(j);
  Color c = o.color("red");
  EXPECT_EQ(c.r, 255);
  EXPECT_EQ(c.g, 0);
  EXPECT_EQ(c.b, 0);
}

TEST_F(JsonTest, color_rg_no_b) {
  auto j = R"({"red": [255, 0]})"_json;
  mct::json::Object o(j);
  EXPECT_THROW(o.color("red"), mct::json::BadTriple);
}

TEST_F(JsonTest, color_no_array_but_string) {
  auto j = R"({"red": "woot"})"_json;
  mct::json::Object o(j);
  EXPECT_THROW(o.color("red"), mct::json::BadTriple);
}

TEST_F(JsonTest, fine_colors) {
    auto j = R"(
    {
      "colors": [
        {"name":"red", "rgb":[255, 0, 0]},
        {"name":"green", "rgb":[0, 255, 0]},
        {"name":"blue", "rgb":[0, 0, 255]}
      ]
    }
    )"_json;
    ColorMap cmap;
    mct::json::assert_key(j, "colors");
    mct::json::add_colors(&cmap, j["colors"]);
    EXPECT_TRUE(cmap.has("red"));
    EXPECT_EQ(*cmap.get("red"), Color(255, 0, 0));
    EXPECT_TRUE(cmap.has("green"));
    EXPECT_EQ(*cmap.get("green"), Color(0, 255, 0));
    EXPECT_TRUE(cmap.has("blue"));
    EXPECT_EQ(*cmap.get("blue"), Color(0, 0, 255));
}

TEST_F(JsonTest, empty_colors) {
    nl::json j = R"({"colors": {}})"_json;
    ColorMap cmap;
    mct::json::assert_key(j, "colors");
    mct::json::add_colors(&cmap, j["colors"]);
    EXPECT_EQ(cmap.colors.size(), 0u);
}

TEST_F(JsonTest, parse_mini_scenery) {
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

    mct::json::Object o(jscenery);
    Scenery s;
    mct::json::append_to_frame_in_scenery(&s.root, &s, o);

    EXPECT_EQ(s.root.get_name(), "root");
    EXPECT_TRUE(s.root.has_children());

    EXPECT_TRUE(s.functions.has("leaf_reflection"));
    EXPECT_TRUE(s.functions.has("pole_dull"));

    EXPECT_TRUE(s.colors.has("grass_green"));
    EXPECT_TRUE(s.colors.has("leaf_green"));
    EXPECT_TRUE(s.colors.has("orange"));

    const std::vector<Frame*>* children = s.root.get_children();
    ASSERT_EQ(children->size(), 1u);
    EXPECT_EQ(children->at(0)->get_name(), "tree");
}

TEST_F(JsonTest, linear_interpolation_function) {
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

    mct::json::Object o(j);
    EXPECT_TRUE(o.size() == 1);
    const mct::json::Object &fo = o.obj(0);
    EXPECT_TRUE(fo.key("name"));
    EXPECT_TRUE(fo.key("argument_versus_value"));

    FunctionMap functions;
    mct::json::add_functions(&functions, o);
    EXPECT_TRUE(functions.has("foo"));
    EXPECT_TRUE(functions.get("foo")->limits().upper() == 5);
    EXPECT_TRUE(functions.get("foo")->limits().lower() == 0);
    EXPECT_NEAR(functions.get("foo")->evaluate(0.0), 5.0, 1e-9);
    EXPECT_NEAR(functions.get("foo")->evaluate(0.5), 4.5, 1e-9);
    EXPECT_NEAR(functions.get("foo")->evaluate(1.0), 4.0, 1e-9);
    EXPECT_NEAR(functions.get("foo")->evaluate(4.999), 0.0, 2e-3);
}

TEST_F(JsonTest, Annulus) {
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
    mct::json::Object o(j);
    Scenery s;
    Annulus* a = mct::json::add_Annulus(&s.root, &s, o);
    EXPECT_EQ(a->get_name(), "ring");
    EXPECT_EQ(a->get_position_in_mother(), Vec3(0, 0, 3));
    EXPECT_EQ(a->get_rotation_in_mother(), Rot3(0, 1, 0));
    EXPECT_EQ(a->get_children()->size(), 0u);
}

TEST_F(JsonTest, Cylinder_with_rot_and_pos) {
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
    mct::json::Object o(j);
    Scenery s;
    Cylinder* a = mct::json::add_Cylinder(&s.root, &s, o);
    EXPECT_EQ(a->get_name(), "cyl");
    EXPECT_EQ(a->get_position_in_mother(), Vec3(0, 0, 3));
    EXPECT_EQ(a->get_rotation_in_mother(), Rot3(0, 1, 0));
    EXPECT_EQ(a->get_children()->size(), 0u);
}

TEST_F(JsonTest, Cylinder_with_start_pos_and_end_pos) {
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
    mct::json::Object o(j);
    Scenery s;
    Cylinder* a = mct::json::add_Cylinder(&s.root, &s, o);
    EXPECT_EQ(a->get_name(), "cyl");
    EXPECT_EQ(a->get_children()->size(), 0u);
}

TEST_F(JsonTest, Triangle) {
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
    mct::json::Object o(j);
    Scenery s;
    Triangle* a = mct::json::add_Triangle(&s.root, &s, o);
    EXPECT_EQ(a->get_name(), "tri");
    EXPECT_EQ(a->get_children()->size(), 0u);
}

TEST_F(JsonTest, Disc) {
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
    Scenery s;
    mct::json::Object o(j);
    Disc* a = mct::json::add_Disc(&s.root, &s, o);
    EXPECT_EQ(a->get_name(), "didi");
    EXPECT_EQ(a->get_children()->size(), 0u);
}

TEST_F(JsonTest, What_is_key) {
    auto j = R"(
    {
      "key": {
        "val1": 1,
        "val2": 2
      }
    }
    )"_json;
    j["key"]["val1"].get<double>();
    EXPECT_EQ(j["key"]["val1"], 1);
}

TEST_F(JsonTest, PropagationConfig) {
    auto j = R"(
    {
      "max_number_of_interactions_per_photon": 1337,
      "use_multithread_when_possible": true
    }
    )"_json;
    mct::json::Object o(j);
    PropagationConfig cfg = mct::json::to_PropagationConfig(o);
    EXPECT_EQ(cfg.max_number_of_interactions_per_photon, 1337u);
    EXPECT_TRUE(cfg.use_multithread_when_possible);
}

TEST_F(JsonTest, PointSource) {
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
  mct::json::Object o(j);
  std::vector<Photon> phs = mct::json::to_photons(o);
  EXPECT_EQ(phs.size(), 137u);
  for (const Photon &ph: phs) {
    EXPECT_EQ(ph.support().x, 0.0);
    EXPECT_EQ(ph.support().y, 2.0);
    EXPECT_EQ(ph.support().z, 0.0);

    EXPECT_NEAR(ph.direction().x, 0.0, 0.1);
    EXPECT_NEAR(ph.direction().y, 0.0, 0.1);
    EXPECT_NEAR(ph.direction().z, 1.0, 0.1);
  }
}

TEST_F(JsonTest, PointSource_rotated) {
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
  mct::json::Object o(j);
  std::vector<Photon> phs = mct::json::to_photons(o);
  EXPECT_EQ(phs.size(), 13u);
  for (const Photon &ph: phs) {
    EXPECT_EQ(ph.support().x, 0.0);
    EXPECT_EQ(ph.support().y, 2.0);
    EXPECT_EQ(ph.support().z, 0.0);

    EXPECT_NEAR(ph.direction().x, -1.0, 0.1);
    EXPECT_NEAR(ph.direction().y, 0.0, 0.1);
    EXPECT_NEAR(ph.direction().z, 0.0, 0.1);
  }
}

TEST_F(JsonTest, ParallelDisc_rotated) {
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
  mct::json::Object o(j);
  std::vector<Photon> phs = mct::json::to_photons(o);
  EXPECT_EQ(phs.size(), 13u);
  for (const Photon &ph: phs) {
    EXPECT_NEAR(ph.direction().x, -1.0, 0.1);
    EXPECT_NEAR(ph.direction().y, 0.0, 0.1);
    EXPECT_NEAR(ph.direction().z, 0.0, 0.1);
  }
}

TEST_F(JsonTest, visual_config) {
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
  mct::json::Object o(j);
  Visual::Config cfg = mct::json::to_visual_config(o, "./");
  EXPECT_TRUE(cfg.max_interaction_depth == 41u);
  EXPECT_TRUE(cfg.preview.rows == 256u);
  EXPECT_TRUE(cfg.preview.cols == 144u);
  EXPECT_TRUE(cfg.preview.scale == 5u);
  EXPECT_TRUE(cfg.snapshot.cols == 1280u);
  EXPECT_TRUE(cfg.snapshot.rows == 720u);
  EXPECT_TRUE(cfg.snapshot.noise_level == 25u);
  EXPECT_TRUE(cfg.snapshot.focal_length_over_aperture_diameter == 0.95);
  EXPECT_TRUE(cfg.snapshot.image_sensor_size_along_a_row == 0.07);
  EXPECT_TRUE(cfg.global_illumination.on);
  EXPECT_TRUE(cfg.global_illumination.incoming_direction == Vec3(.2, .3, 1.));
  EXPECT_TRUE(cfg.photon_trajectories.radius == 0.15);
}
