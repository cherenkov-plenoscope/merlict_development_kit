// Copyright 2014 Sebastian A. Mueller
#ifndef MERLICT_FRAME_H_
#define MERLICT_FRAME_H_

namespace merlict {
class Ray;
class Intersection;
}  // namespace merlict

#include <string>
#include <vector>
#include "merlict/Vec3.h"
#include "merlict/Rot3.h"
#include "merlict/HomTra3.h"

namespace merlict {

const char FRAME_PATH_DELIMITER = '/';
const unsigned int FRAME_MAX_NUMBER_CHILDREN = 16;
const double FRAME_MIN_STRUCTURE_SIZE = 1e-6;

class Frame {
    // A frame defines the geometric relation to its mother frame and its
    // children frames. This way a tree structure of the scenery is created.
    // The root of this tree is often called 'world' or 'world frame' here.
 protected:
    std::string name;
    Vec3 pos_in_mother;
    Rot3 rot_in_mother;
    double bounding_sphere_radius;
    HomTra3 T_frame2mother;
    HomTra3 T_frame2world;
    std::vector<Frame*> children;
    Frame *mother;
    const Frame *root_frame;

 public:
    // SET
    Frame();
    virtual ~Frame();
    void set_name_pos_rot(
        const std::string name,
        const Vec3 pos,
        const Rot3 rot);
    // GET
    std::string get_name()const;
    std::string path_in_tree()const;
    Vec3 position_in_mother()const;
    Rot3 rotation_in_mother()const;
    Vec3 position_in_world()const;
    double get_bounding_sphere_radius()const;
    const HomTra3* frame2mother()const;
    const HomTra3* frame2world()const;
    const std::vector<Frame*>* get_children()const;
    const Frame* root()const;
    bool has_mother()const;
    bool has_children()const;
    void assert_no_children_duplicate_names()const;
    virtual std::string str()const;
    std::string tree_str()const;
    // DO
    template<class ProtoFrame>
    ProtoFrame* add() {
        ProtoFrame* child = new ProtoFrame;
        children.push_back(child);
        child->mother = this;
        return child;
    }
    void erase(const Frame* child);
    void init_tree_based_on_mother_child_relations();
    virtual void calculate_intersection_with(
        const Ray* ray,
        std::vector<Intersection> *intersections)const;

 private:
    HomTra3 calculate_frame2world()const;
    void init_frame2world();
    void init_root();
    void set_name(const std::string name);
    void warn_small_child(const Frame* frame)const;
    void warn_about_close_frames()const;
    void cluster_children();
    void assert_name_is_valid(const std::string name_to_check)const;
    void update_bounding_sphere();
};

const Frame VOID_FRAME;

}  // namespace merlict

#endif  // MERLICT_FRAME_H_
