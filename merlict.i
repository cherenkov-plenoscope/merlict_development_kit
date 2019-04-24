%module merlict

%{
    #include "merlict.h"
%}

%include "stdint.i"
%include "std_vector.i"
%include "std_string.i"

%rename(function_mean) merlict::function::mean;
%rename(numeric_mean) merlict::numeric::mean;

%rename(segmented_imaging_reflector_Config) merlict::segmented_imaging_reflector::Config;
%rename(lens_maker_Config) merlict::lens_maker::Config;

%include "merlict.h"

namespace std
{
  %template(VectorOfInts) vector<int>;
  %template(VectorOfDoubles) vector<double>;
  %template(VectorOfVectorOfDoubles) vector< vector<double>>;
  %template(VectorOfSensors) vector< merlict::sensor::Sensor* >;
  %template(VectorOfPhotons) vector< merlict::Photon >;
  %template(VectorOfPhotonArrivals) vector< merlict::sensor::PhotonArrival >;
  %template(VectorOfColors) vector< merlict::Color >;
  %template(VectorOfFramePointers) vector< merlict::Frame* >;

  // templating
}

namespace merlict
{
    %template(add_frame) Frame::add<Frame>;
    %template(add_plane) Frame::add<Plane>;
    %template(add_rectangular_box) Frame::add<RectangularBox>;
    %template(add_surface_entity) Frame::add<SurfaceEntity>;
    %template(add_sphere) Frame::add<Sphere>;
    %template(add_sphere_cap_with_hexagonal_bound) Frame::add<SphereCapWithHexagonalBound>;
    %template(add_sphere_cap_with_cylinder_bound) Frame::add<SphereCapWithCylinderBound>;
    %template(add_cylinder) Frame::add<Cylinder>;
    %template(add_disc) Frame::add<Disc>;

    %template(add_bi_convex_lens) Frame::add<BiConvexLens>;
    %template(add_bi_convex_lens_hex_bound) Frame::add<BiConvexLensHexBound>;
    %template(add_triangle) Frame::add<Triangle>;
    %template(add_plane_dual_spherical_bound) Frame::add<PlaneDualSphericalBound>;
}
