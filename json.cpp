// Copyright 2018 Sebastian A. Mueller
#include "./json.h"
#include <fstream>
#include <sstream>
#include "nlohmann_json.hpp"
#include "Tools/StringTools.h"
#include "Scenery/Primitive/Primitive.h"
#include "Scenery/StereoLitography/StereoLitography.h"
#include "Tools/PathTools.h"
#include "Scenery/SegmentedReflector/SegmentedReflector.h"
namespace nl = nlohmann;
using std::string;
using StringTools::is_equal;

namespace mct {
namespace json {

void assert_key(const nl::json &j, const string &key) {
    if (!has(j, key)) {
        std::stringstream info;
        info << "Expected key: '" << key << "' in " << j << "\n";
        throw MissingKey(info.str());
    }
}

void assert_idx(const nl::json &j, const uint64_t idx) {
    if (j.size() <= idx) {
        std::stringstream info;
        info << "Expected at least size " << idx + 1 << " for object ";
        info << j << " \n";
        throw ListTooShort(info.str());
    }
}

std::array<double, 3> to_triple(const nlohmann::json &j) {
    try {
        std::array<double, 3> a = {
            j.at(0).get<float>(),
            j.at(1).get<float>(),
            j.at(2).get<float>()};
        return a;
    } catch (...) {
        std::stringstream info;
        info << "Expected triple to be an array of three floats: ";
        info << "[x, y, z]', but actually it is" << j << "'.\n";
        throw BadTriple(info.str());
    }
}

bool has(const nlohmann::json &j, const std::string &key) {
    return j.find(key) != j.end();
}

Object::Object(const nlohmann::json _j): j(_j) {}

bool Object::key(const std::string &key)const {
    return has(j, key);}

double Object::f8(const std::string &key)const {
    assert_key(j, key);
    return j[key].get<double>();}
double Object::f8(const uint64_t idx)const {
    assert_idx(j, idx);
    return j.at(idx).get<double>();}

int64_t Object::i8(const std::string &key)const {
    assert_key(j, key);
    return j[key].get<int64_t>();}
int64_t Object::i8(const uint64_t idx)const {
    assert_idx(j, idx);
    return j.at(idx).get<int64_t>();}

uint64_t Object::u8(const std::string &key)const {
    assert_key(j, key);
    return j[key].get<uint64_t>();}
uint64_t Object::u8(const uint64_t idx)const {
    assert_idx(j, idx);
    return j.at(idx).get<int64_t>();}

std::string Object::st(const std::string &key)const {
    assert_key(j, key);
    return j[key].get<std::string>();}
std::string Object::st(const uint64_t idx)const {
    assert_idx(j, idx);
    return j.at(idx).get<std::string>();}

Object Object::obj(const std::string &key)const {
    assert_key(j, key);
    return Object(j[key]);}
Object Object::obj(const uint64_t idx)const {
    assert_idx(j, idx);
    return j.at(idx);}

Vec3 Object::vec3(const std::string &key)const {
    assert_key(j, key);
    std::array<double, 3> a = to_triple(j[key]);
    return Vec3(a.at(0), a.at(1), a.at(2));}
Vec3 Object::vec3(const uint64_t idx)const {
    assert_idx(j, idx);
    std::array<double, 3> a = to_triple(j.at(idx));
    return Vec3(a.at(0), a.at(1), a.at(2));}

Rot3 Object::rot3(const std::string &key)const {
    assert_key(j, key);
    std::array<double, 3> a = to_triple(j[key]);
    return Rot3(a.at(0), a.at(1), a.at(2));}
Rot3 Object::rot3(const uint64_t idx)const {
    assert_idx(j, idx);
    std::array<double, 3> a = to_triple(j.at(idx));
    return Rot3(a.at(0), a.at(1), a.at(2));}

Color Object::color(const std::string &key)const {
    assert_key(j, key);
    std::array<double, 3> a = to_triple(j[key]);
    return Color(a.at(0), a.at(1), a.at(2));}
Color Object::color(const uint64_t idx)const {
    assert_idx(j, idx);
    std::array<double, 3> a = to_triple(j.at(idx));
    return Color(a.at(0), a.at(1), a.at(2));}

bool Object::b1(const std::string &key)const {
    assert_key(j, key);
    return j[key].get<bool>();}
bool Object::b1(const uint64_t idx)const {
    assert_idx(j, idx);
    return j.at(idx).get<bool>();}

uint64_t Object::size()const {
    return j.size();}

std::string Object::str()const {
    std::stringstream out;
    out << j;
    return out.str();}

Object load(const string &path) {
    std::ifstream fin(path.c_str());
    if (fin.is_open()) {
        nl::json content;
        fin >> content;
        fin.close();
        return content;
    } else {
        std::stringstream info;
        info << "json_to_scenery: ";
        info << "Can not open '" << path << "'.";
        throw std::runtime_error(info.str());
    }
}

std::map<string, json_to_frame> basic_scenery() {
    std::map<string, json_to_frame> mymap;
    mymap["Frame"] = add_Frame;
    mymap["Sphere"] = add_Sphere;
    mymap["Annulus"] = add_Annulus;
    mymap["Cylinder"] = add_Cylinder;
    mymap["Triangle"] = add_Triangle;
    mymap["Disc"] = add_Disc;
    mymap["Plane"] = add_Plane;
    mymap["HexPlane"] = add_HexPlane;
    mymap["StereoLitography"] = add_StereoLitography;
    mymap["BiConvexLensHex"] = add_BiConvexLensHex;
    mymap["SphereCapWithHexagonalBound"] = add_SphereCapWithHexagonalBound;
    mymap["SphereCapWithRectangularBound"] = add_SphereCapWithRectangularBound;
    mymap["SegmentedReflector"] = add_SegmentedReflector;
    return mymap;
}

bool has_key(std::map<string, json_to_frame> m, const string &key) {
    return m.find(key) != m.end();
}

void assert_has_key(std::map<string, json_to_frame> m, const string &key) {
    if (!has_key(m, key)) {
        std::stringstream info;
        info << "There is no json-to-frame called '" << key << "'. \n";
        throw std::invalid_argument(info.str());
    }
}

json_to_frame get(std::map<string, json_to_frame> m, const string &key) {
    assert_has_key(m, key);
    return m.find(key)->second;
}

void set_frame(Frame *f, const Object &o) {
    string name = o.st("name");
    Vec3 pos = o.vec3("pos");
    Rot3 rot;
    if (o.key("rot")) {
        rot = o.rot3("rot");
    } else if (o.key("rot_axis")) {
        const Vec3 rot_axis = o.vec3("rot_axis");
        const double angle = o.f8("rot_angle");
        rot = Rot3(rot_axis, angle);}
    f->set_name_pos_rot(name, pos, rot);
}

void set_surface(SurfaceEntity *s, Scenery *scenery, const Object &o) {
    const Object &su = o.obj("surface");
    if (su.key("inner_color"))
        s->set_inner_color(
            scenery->colors.get(su.st("inner_color")));
    if (su.key("outer_color"))
        s->set_outer_color(
            scenery->colors.get(su.st("outer_color")));
    if (su.key("inner_reflection"))
        s->set_inner_reflection(
            scenery->functions.get(su.st("inner_reflection")));
    if (su.key("outer_reflection"))
        s->set_outer_reflection(
            scenery->functions.get(su.st("outer_reflection")));
    if (su.key("inner_refraction"))
        s->set_inner_refraction(
            scenery->functions.get(su.st("inner_refraction")));
    if (su.key("outer_refraction"))
        s->set_outer_refraction(
            scenery->functions.get(su.st("outer_refraction")));
}

void make_children(Frame* mother, Scenery* scenery, const Object &o) {
    std::map<string, json_to_frame> j2s = basic_scenery();
    const uint64_t num_children = o.size();
    for (uint64_t cidx = 0; cidx < num_children; cidx++) {
        const Object &jchild = o.obj(cidx);
        string type = jchild.st("type");
        if (has_key(j2s, type)) {
            json_to_frame us = get(j2s, type);
            make_children(
                us(mother, scenery, jchild),
                scenery,
                jchild.obj("children"));
        }
    }
}

void add_colors(ColorMap* colors, const Object &o) {
    const uint64_t num_colors = o.size();
    for (uint64_t cidx = 0; cidx < num_colors; cidx++) {
        const Object &jcolor = o.obj(cidx);
        colors->add(jcolor.st("name"), jcolor.color("rgb"));
    }
}

void add_functions(FunctionMap* functions, const Object &o) {
    const uint64_t num_functions = o.size();
    for (uint64_t fidx = 0; fidx < num_functions; fidx++) {
        const Object &jfun = o.obj(fidx);
        string name = jfun.st("name");
        const Object &avsv = jfun.obj("argument_versus_value");
        const uint64_t num_points = avsv.size();
        std::vector<std::vector<double>> argument_versus_value;
        for (uint64_t i = 0; i < num_points; i++) {
            const Object &point = avsv.obj(i);
            if (point.size() != 2) {
                std::stringstream info;
                info << "Expected argument_versus_value in ";
                info << "linear_interpolation to be a list ";
                info << "of length-two-lists.\n";
                throw std::invalid_argument(info.str());}
            argument_versus_value.push_back({point.f8(0), point.f8(1)});
        }
        Function::LinInterpol* f = functions->add<Function::LinInterpol>(name);
        f->init(argument_versus_value);
    }
}

void append_to_frame_in_scenery(
    Frame* mother,
    Scenery* scenery,
    const string &path
) {
    PathTools::Path json_path(path);
    scenery->current_working_directory = json_path.dirname;
    append_to_frame_in_scenery(mother, scenery, load(path));
}

void append_to_frame_in_scenery(
    Frame* mother,
    Scenery* scenery,
    const Object &o
) {
    add_functions(&scenery->functions, o.obj("functions"));
    add_colors(&scenery->colors, o.obj("colors"));
    make_children(mother, scenery, o.obj("children"));
}

Frame* add_Frame(Frame* mother, Scenery *scenery, const Object &o) {
    (void)scenery;
    Frame* frame = mother->append<Frame>();
    set_frame(frame, o);
    return frame;
}

Frame* add_Sphere(Frame* mother, Scenery *scenery, const Object &o) {
    Sphere* sphere = mother->append<Sphere>();
    set_frame(sphere, o);
    sphere->set_radius(o.f8("radius"));
    set_surface(sphere, scenery, o);
    return sphere;
}

Frame* add_StereoLitography(
    Frame* mother,
    Scenery *scenery,
    const Object &o
) {
    SurfaceEntity* object = mother->append<SurfaceEntity>();
    set_frame(object, o);
    set_surface(object, scenery, o);
    const double scale = o.f8("scale");
    const string stl_path = PathTools::join(
        scenery->current_working_directory, o.st("path"));
    StereoLitography::add_stl_to_and_inherit_surface_from_surfac_entity(
        stl_path, object, scale);
    return object;
}

Frame* add_Annulus(Frame* mother, Scenery *scenery, const Object &o) {
    Annulus* annulus = mother->append<Annulus>();
    set_frame(annulus, o);
    annulus->set_outer_inner_radius(
        o.f8("outer_radius"),
        o.f8("inner_radius"));
    set_surface(annulus, scenery, o);
    return annulus;
}

Frame* add_Cylinder(Frame* mother, Scenery *scenery, const Object &o) {
    Cylinder* c = mother->append<Cylinder>();
    if (o.key("rot")) {
        set_frame(c, o);
        c->set_radius_and_length(
            o.f8("radius"),
            o.f8("length"));
    } else if (o.key("start_pos")) {
        c->set_name_pos_rot(o.st("name"), Vec3(0, 0, 0), Rot3(0, 0, 0));
        c->set_cylinder(
            o.f8("radius"),
            o.vec3("start_pos"),
            o.vec3("end_pos"));
    } else {
        std::stringstream info;
        info << "Expected Cylinder to have either 'pos', 'rot', 'radius', ";
        info << "and 'length', or to have ";
        info << "'start_pos', 'end_pos', and 'radius'. ";
        info << "But actually it has: " << o.str() << "\n";
        throw std::invalid_argument(info.str());
    }
    set_surface(c, scenery, o);
    return c;
}

Frame* add_Triangle(Frame* mother, Scenery *scenery, const Object &o) {
    Triangle* tri = mother->append<Triangle>();
    set_frame(tri, o);
    tri->set_corners_in_xy_plane(
        o.f8("Ax"), o.f8("Ay"),
        o.f8("Bx"), o.f8("By"),
        o.f8("Cx"), o.f8("Cy"));
    set_surface(tri, scenery, o);
    return tri;
}

Frame* add_Disc(Frame* mother, Scenery *scenery, const Object &o) {
    Disc* disc = mother->append<Disc>();
    set_frame(disc, o);
    disc->set_radius(o.f8("radius"));
    set_surface(disc, scenery, o);
    return disc;
}

Frame* add_Plane(Frame* mother, Scenery *scenery, const Object &o) {
    Plane* plane = mother->append<Plane>();
    set_frame(plane, o);
    plane->set_x_y_width(o.f8("x_width"), o.f8("y_width"));
    set_surface(plane, scenery, o);
    return plane;
}

Frame* add_HexPlane(Frame* mother, Scenery *scenery, const Object &o) {
    HexPlane* plane = mother->append<HexPlane>();
    set_frame(plane, o);
    plane->set_outer_hex_radius(o.f8("outer_radius"));
    set_surface(plane, scenery, o);
    return plane;
}

Frame* add_BiConvexLensHex(
    Frame* mother,
    Scenery *scenery,
    const Object &o
) {
    BiConvexLensHexBound* lens = mother->append<BiConvexLensHexBound>();
    set_frame(lens, o);
    lens->set_curvature_radius_and_outer_hex_radius(
        o.f8("curvature_radius"),
        o.f8("outer_radius"));
    set_surface(lens, scenery, o);
    return lens;
}

Frame* add_SphereCapWithHexagonalBound(
    Frame* mother,
    Scenery *scenery,
    const Object &o
) {
    SphereCapWithHexagonalBound* cap =
        mother->append<SphereCapWithHexagonalBound>();
    set_frame(cap, o);
    cap->set_curvature_radius_and_outer_hex_radius(
        o.f8("curvature_radius"),
        o.f8("outer_radius"));
    set_surface(cap, scenery, o);
    return cap;
}

Frame* add_SphereCapWithRectangularBound(
    Frame* mother,
    Scenery *scenery,
    const Object &o
) {
    SphereCapWithRectangularBound* cap =
        mother->append<SphereCapWithRectangularBound>();
    set_frame(cap, o);
    cap->set_curvature_radius_and_x_y_width(
        o.f8("curvature_radius"),
        o.f8("x_width"),
        o.f8("y_width"));
    set_surface(cap, scenery, o);
    return cap;
}

Frame* add_SegmentedReflector(
    Frame* mother,
    Scenery *scenery,
    const Object &o
) {
    SegmentedReflector::Config cfg;
    cfg.focal_length = o.f8("focal_length");
    cfg.DaviesCotton_over_parabolic_mixing_factor =
        o.f8("DaviesCotton_over_parabolic_mixing_factor");
    cfg.max_outer_aperture_radius = o.f8("max_outer_aperture_radius");
    cfg.min_inner_aperture_radius = o.f8("min_inner_aperture_radius");
    cfg.facet_inner_hex_radius = o.f8("facet_inner_hex_radius");
    cfg.gap_between_facets = o.f8("gap_between_facets");
    cfg.reflectivity = scenery->functions.get(
        o.obj("surface").st("outer_reflection"));

    SegmentedReflector::Factory refl_fab(cfg);
    Frame* reflector = mother->append<Frame>();
    set_frame(reflector, o);
    refl_fab.add_reflector_mirror_facets_to_frame(reflector);
    return reflector;
}

Visual::Config load_visual_config(const std::string &path) {
    return to_visual_config(load(path), path);
}

Visual::Config to_visual_config(
    const Object &o,
    const std::string &path
) {
    Visual::Config cfg;
    cfg.max_interaction_depth = o.u8("max_interaction_depth");
    cfg.preview.cols = o.obj("preview").u8("cols");
    cfg.preview.rows = o.obj("preview").u8("rows");
    cfg.preview.scale = o.obj("preview").u8("scale");
    cfg.snapshot.cols = o.obj("snapshot").u8("cols");
    cfg.snapshot.rows = o.obj("snapshot").u8("rows");
    cfg.snapshot.noise_level = o.obj("snapshot").u8("noise_level");
    cfg.snapshot.focal_length_over_aperture_diameter =
        o.obj("snapshot").f8("focal_length_over_aperture_diameter");
    cfg.snapshot.image_sensor_size_along_a_row =
        o.obj("snapshot").f8("image_sensor_size_along_a_row");
    cfg.global_illumination.on =
        o.obj("global_illumination").b1("on");
    cfg.global_illumination.incoming_direction =
        o.obj("global_illumination").vec3("incoming_direction");
    cfg.photon_trajectories.radius = o.obj("photon_trajectories").f8("radius");

    const Object &skyj = o.obj("sky_dome");
    string image_path = skyj.st("path");

    if (image_path.empty()) {
        cfg.sky_dome = Visual::SkyDome(skyj.color("color"));
    } else {
        PathTools::Path jsonpath = PathTools::Path(path);
        cfg.sky_dome = Visual::SkyDome(
            PathTools::join(jsonpath.dirname, image_path));
        cfg.sky_dome.set_background_color(skyj.color("color"));
    }
    return cfg;
}

PropagationConfig load_PropagationConfig(const std::string &path) {
    return to_PropagationConfig(load(path));
}

PropagationConfig to_PropagationConfig(const Object &o) {
    PropagationConfig cfg;
    cfg.use_multithread_when_possible =
        o.b1("use_multithread_when_possible");
    cfg.max_number_of_interactions_per_photon =
        o.u8("max_number_of_interactions_per_photon");
    return cfg;
}

void transform_photons(const Object &o, std::vector<Photon> *photons) {
    Vec3 pos = o.vec3("pos");
    Rot3 rot = o.rot3("rot");
    HomTra3 Trafo;
    Trafo.set_transformation(rot, pos);
    Photons::transform_all_photons_multi_thread(Trafo, photons);
}

std::vector<Photon> to_parallel_disc(const Object &o) {
    std::vector<Photon> photons =
        Photons::Source::parallel_towards_z_from_xy_disc(
            o.f8("disc_radius"),
            o.u8("num_photons"));
    return photons;
}

std::vector<Photon> to_pointsource(const Object &o) {
    std::vector<Photon> photons =
        Photons::Source::point_like_towards_z_opening_angle_num_photons(
            o.f8("opening_angle"),
            o.u8("num_photons"));
    return photons;
}

std::vector<Photon> load_photons(const std::string &path) {
    return to_photons(load(path));
}

std::vector<Photon> to_photons(const Object &o) {
    std::vector<Photon> photons;
    if (o.key("point_source"))  {
        photons = to_pointsource(o.obj("point_source"));
    } else if (o.key("parallel_disc")) {
        photons = to_parallel_disc(o.obj("parallel_disc"));
    } else {
        std::stringstream info;
        info << "Expected InternalPhotonSource from json ";
        info << "to be either 'point_source', or 'parallel_disc'.\n";
        throw UnkownTypeOfLightSource(info.str());
    }
    transform_photons(o, &photons);
    return photons;
}

}  // namespace json
}  // namespace mct
