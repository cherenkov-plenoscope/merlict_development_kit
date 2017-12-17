// Copyright 2014 Sebastian A. Mueller
#ifndef PHOTONSENSOR_PHOTONARRIVAL_H_
#define PHOTONSENSOR_PHOTONARRIVAL_H_

namespace PhotonSensor {

struct PhotonArrival {
    int simulation_truth_id;
    double wavelength;
    double arrival_time;
    double x_intersect;
    double y_intersect;
    double theta_x;
    double theta_y;

    PhotonArrival();
    PhotonArrival(
        int id,
        double w,
        double t,
        double x,
        double y,
        double tx,
        double ty);
};

}  // namespace PhotonSensor

#endif  // PHOTONSENSOR_PHOTONARRIVAL_H_