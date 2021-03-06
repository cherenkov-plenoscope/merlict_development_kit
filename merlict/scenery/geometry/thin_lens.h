// Copyright 2014 Sebastian A. Mueller
#ifndef SCENERY_GEOMETRY_THIN_LENS_H_
#define SCENERY_GEOMETRY_THIN_LENS_H_

namespace merlict {
namespace thin_lens {

// The thin lens equation
//
//    1     1     1
//   --- = --- + ---
//    f     i     o
//
// f: focal-length of imaging-system
// i: image-distance
// o: object-distance

double image_dist_given_focal_and_object_dist(double f, double o);

double object_dist_given_focal_and_image_dist(double f, double i);

double focal_dist_given_object_and_image_dist(double o, double i);

}  // namespace thin_lens
}  // namespace merlict

#endif  // SCENERY_GEOMETRY_THIN_LENS_H_
