// Copyright 2014 Sebastian A. Mueller
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <cmath>

#include "Core/Histogram1D.h"
#include "Tools/StringTools.h"
using std::string;
using std::vector;


Histogram1D::Histogram1D(
    const vector<double> &samples,
    const vector<double> &_bin_edges
):bin_edges(_bin_edges) {
    assert_at_least_two_bin_edges();
    std::sort(bin_edges.begin(), bin_edges.end());
    init_bins_to_zero();
    for (double sample : samples)
        fill_in(sample);
    number_of_samples = samples.size();
}

void Histogram1D::init_bins_to_zero() {
    unsigned int bin_count = bin_edges.size() - 1;
    bins.reserve(bin_count);
    for (unsigned int i = 0; i < bin_count; i++)
        bins.push_back(0);
}

void Histogram1D::fill_in(const double sample) {
    vector<double>::const_iterator up_bin_edge = std::upper_bound(
        bin_edges.begin(),
        bin_edges.end(),
        sample);

    if (up_bin_edge == bin_edges.begin()) {
        underflow_bin++;
        return;
    }

    if (up_bin_edge == bin_edges.end()) {
        overflow_bin++;
        return;
    }

    unsigned int bin_index = (up_bin_edge - bin_edges.begin()) - 1;
    bins.at(bin_index)++;
}

double Histogram1D::mode()const {
    unsigned int idx_max_bin = arg_max();
    return 0.5*(bin_edges.at(idx_max_bin+1) + bin_edges.at(idx_max_bin));
}

unsigned int Histogram1D::arg_max()const {
    unsigned int idx_max_bin = 0;
    unsigned int max_bin_count = 0;

    for (unsigned int i = 0; i < bins.size(); i++)
        if (bins.at(i) > max_bin_count) {
            idx_max_bin = i;
            max_bin_count = bins.at(i);
        }

    return idx_max_bin;
}

string Histogram1D::str()const {
    std::stringstream out;
    out.precision(2);

    unsigned int max_cols = 40;
    unsigned int max_bin_count = 0;
    for (unsigned int bin : bins) if (bin > max_bin_count) max_bin_count = bin;

    double scaling = static_cast<double>(max_cols)/
        static_cast<double>(max_bin_count);

    unsigned int bin_number_cols = ceil(log10(bins.size()));
    if (bin_number_cols < 2) bin_number_cols = 2;

    out << "underflow " << underflow_bin << "\n";
    for (unsigned int i = 0; i < bins.size(); i++) {
        out << std::setw(bin_number_cols) << i << " ";
        out << "[" << std::setw(8) << bin_edges.at(i) << ", ";
        out << std::setw(8) << bin_edges.at(i+1) << "] ";
        out << StringTools::repeat_multiple_times(
            "|",
            (unsigned int)(scaling*bins.at(i)) );
        out << " " << bins.at(i) << "\n";
    }
    out << "overflow  " << overflow_bin << "\n";

    return out.str();
}

void Histogram1D::assert_at_least_two_bin_edges()const {
    if (bin_edges.size() < 2) {
        std::stringstream info;
        info << __FILE__ << ", " << __LINE__ << ", " << __func__ << "\n";
        info << "Expected at least two bin edges, but actual there is only ";
        info << bin_edges.size() << ".\n";
        throw std::invalid_argument(info.str());
    }
}
