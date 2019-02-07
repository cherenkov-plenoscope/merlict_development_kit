// Copyright 2018 Sebastian A. Mueller
#ifndef MCT_JSON_H_
#define MCT_JSON_H
#include <string>
#include <sstream>
#include "Scenery/Scenery.h"
#include "Core/Frame.h"
#include "Core/Photons.h"
#include "Core/PropagationConfig.h"
#include "Visual/Config.h"
#include "Scenery/Primitive/Primitive.h"
#include "nlohmann_json.hpp"

namespace mct {
namespace json {

void assert_key(const nlohmann::json &j, const std::string &key);
void assert_idx(const nlohmann::json &j, const uint64_t idx);
bool has(const nlohmann::json &j, const std::string &key);

class BadTriple : public std::invalid_argument {
    using invalid_argument::invalid_argument;};

std::array<double, 3> to_triple(const nlohmann::json &j);

class MissingKey : public std::invalid_argument {
    using invalid_argument::invalid_argument;};
class ListTooShort : public std::invalid_argument {
    using invalid_argument::invalid_argument;};

struct Object {
	const nlohmann::json j;
	Object(const nlohmann::json _j);
	bool key(const std::string &key)const;
	double f8(const std::string &key)const;
	double f8(const uint64_t idx)const;
	int64_t i8(const std::string &key)const;
	int64_t i8(const uint64_t idx)const;
	uint64_t u8(const std::string &key)const;
	uint64_t u8(const uint64_t idx)const;
	std::string st(const std::string &key)const;
	std::string st(const uint64_t idx)const;
	Object obj(const std::string &key)const;
	Object obj(const uint64_t idx)const;
	Vec3 vec3(const std::string &key)const;
	Vec3 vec3(const uint64_t idx)const;
	Rot3 rot3(const std::string &key)const;
	Rot3 rot3(const uint64_t idx)const;
	Color color(const std::string &key)const;
	Color color(const uint64_t idx)const;
	bool b1(const std::string &key)const;
	bool b1(const uint64_t idx)const;

	uint64_t size()const;
	std::string str()const;
};

Object load(const std::string &path);

void set_frame(Frame *f, const Object &o);
void set_surface(SurfaceEntity *s, Scenery *scenery, const Object &o);
void make_children(Frame* mother, Scenery* scenery, const Object &o);
void add_colors(ColorMap* colors, const Object &o);
void add_functions(FunctionMap* functions, const Object &o);

void append_to_frame_in_scenery(
    Frame* frame,
    Scenery* scenery,
    const std::string &path);

void append_to_frame_in_scenery(
    Frame* mother,
    Scenery* scenery,
    const Object &o);

Frame* add_Frame(
	Frame* mother,
	Scenery *scenery,
	const Object &o);

Sphere* add_Sphere(
	Frame* mother,
	Scenery *scenery,
	const Object &o);

Annulus* add_Annulus(
	Frame* mother,
	Scenery *scenery,
	const Object &o);

Cylinder* add_Cylinder(
	Frame* mother,
	Scenery *scenery,
	const Object &o);

Triangle* add_Triangle(
	Frame* mother,
	Scenery *scenery,
	const Object &o);

Disc* add_Disc(
	Frame* mother,
	Scenery *scenery,
	const Object &o);

Plane* add_Plane(
	Frame* mother,
	Scenery *scenery,
	const Object &o);

HexPlane* add_HexPlane(
	Frame* mother,
	Scenery *scenery,
	const Object &o);

Frame* add_StereoLitography(
	Frame* mother,
	Scenery *scenery,
	const Object &o);

BiConvexLensHexBound* add_BiConvexLensHex(
    Frame* mother,
    Scenery *scenery,
    const Object &o);

SphereCapWithHexagonalBound* add_SphereCapWithHexagonalBound(
    Frame* mother,
    Scenery *scenery,
    const Object &o);

SphereCapWithRectangularBound* add_SphereCapWithRectangularBound(
    Frame* mother,
    Scenery *scenery,
    const Object &o);

Frame* add_SegmentedReflector(
    Frame* mother,
    Scenery *scenery,
    const Object &o);

Visual::Config to_visual_config(const Object &o, const std::string &path);

Visual::Config load_visual_config(const std::string &path);

PropagationConfig to_PropagationConfig(const Object &o);

PropagationConfig load_PropagationConfig(const std::string &path);

void transform(const Object &o, std::vector<Photon> *photons);

std::vector<Photon> to_parallel_disc(const Object &o);

std::vector<Photon> to_pointsource(const Object &o);

class UnkownTypeOfLightSource : public std::invalid_argument {
    using invalid_argument::invalid_argument;};

std::vector<Photon> load_photons(const std::string &path);

std::vector<Photon> to_photons(const Object &o);

}  // namespace json
}  // namespace mct

#endif  // MCT_JSON_H_