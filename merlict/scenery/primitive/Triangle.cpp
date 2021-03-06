// Copyright 2014 Sebastian A. Mueller
#include "merlict/scenery/primitive/Triangle.h"
#include <algorithm>
#include <sstream>


namespace merlict {

void Triangle::set_corners_in_xy_plane(
    const double Ax, const double Ay,
    const double Bx, const double By,
    const double Cx, const double Cy
) {
    A = Vec2(Ax, Ay);
    B = Vec2(Bx, By);
    C = Vec2(Cx, Cy);
    assert_edge_length_is_non_zero((A-B).norm(), "AB");
    assert_edge_length_is_non_zero((C-A).norm(), "AC");
    assert_edge_length_is_non_zero((C-B).norm(), "CB");

    post_initialize_radius_of_enclosing_sphere();
}

void Triangle::set_normal_and_3_vertecies(
    const Vec3 normal,
    Vec3 a,
    Vec3 b,
    Vec3 c
) {
    // correct for offset
    Vec3 pos = (a + b + c)/3.0;
    a = a - pos;
    b = b - pos;
    c = c - pos;
    Rot3 rot;

    if (normal != VEC3_UNIT_Z) {
        // transformation to make surface normal match z-axis
        Vec3 rot_axis = VEC3_UNIT_Z.cross(normal);
        double rotation_angle = VEC3_UNIT_Z.angle_in_between(normal);
        rot = Rot3(rot_axis, rotation_angle);
        HomTra3 trafo;
        trafo.set_transformation(rot, VEC3_ORIGIN);
        HomTra3 trafo_inv = trafo.inverse();
        // transform a b c
        a = trafo_inv.orientation(a);
        b = trafo_inv.orientation(b);
        c = trafo_inv.orientation(c);
    } else {
        rot = ROT3_UNITY;
    }
    set_name_pos_rot(name, pos, rot);
    set_corners_in_xy_plane(
        a.x, a.y,
        b.x, b.y,
        c.x, c.y);
}

void Triangle::assert_edge_length_is_non_zero(
    const double edge_length,
    const std::string edge
)const {
    if (edge_length <= 0.0) {
        std::stringstream out;
        out << "Triangle::" << __func__ <<"():\n";
        out << "Expected edge length of " << edge << " > 0.0, but actual: ";
        out << "edge length of " << edge << " = " << edge_length << "m.\n";
        throw std::invalid_argument(out.str());
    }
}

void Triangle::post_initialize_radius_of_enclosing_sphere() {
    std::vector<double> dist_corner_to_base;
    dist_corner_to_base.push_back(A.norm());
    dist_corner_to_base.push_back(B.norm());
    dist_corner_to_base.push_back(C.norm());
    bounding_sphere_radius = *max_element(
        dist_corner_to_base.begin(),
        dist_corner_to_base.end());
}

std::string Triangle::str()const {
    std::stringstream out;
    out << SurfaceEntity::str();
    out << "triangle:\n";
    out << "| A: " << A.str() << "\n";
    out << "| B: " << B.str() << "\n";
    out << "| C: " << C.str() << "\n";
    return out.str();
}

double Triangle::sign(const Vec3 &P1, const Vec2 &P2, const Vec2 &P3)const {
    return (P1.x-P3.x) * (P2.y-P3.y)-(P2.x-P3.x) * (P1.y-P3.y);
}

bool Triangle::is_inside_triangle(const Vec3 &intersec_vec)const {
    bool bA, bB, bC;
    bA = sign(intersec_vec, A, B) < 0.0;
    bB = sign(intersec_vec, B, C) < 0.0;
    bC = sign(intersec_vec, C, A) < 0.0;
    return ((bA == bB) && (bB == bC));
}

#include "merlict/Ray.h"
#include "merlict/Intersection.h"

void Triangle::calculate_intersection_with(
    const Ray* ray,
    std::vector<Intersection> *intersections
)const {
    XyPlaneRayIntersectionEquation xyPlaneRayEquation(ray);
    if (xyPlaneRayEquation.has_causal_solution()) {
        double v = xyPlaneRayEquation.get_ray_parameter_for_intersection();
        Vec3 intersection_vector = ray->position_at(v);
        if (is_inside_triangle(intersection_vector)) {
            if (ray->support() != intersection_vector) {
                intersections->emplace_back(
                    this,
                    intersection_vector,
                    xyPlaneRayEquation.get_plane_normal_vector(),
                    v,
                    ray->direction());
            }
        }
    }
}

}  // namespace merlict
