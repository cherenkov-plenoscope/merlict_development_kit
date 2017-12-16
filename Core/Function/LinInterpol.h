// Copyright 2014 Sebastian A. Mueller
#ifndef CORE_FUNCTION_LININTERPOL_H_
#define CORE_FUNCTION_LININTERPOL_H_

#include "Func1D.h"
#include <vector>
#include <string>

namespace Function {

class LinInterpol : public Func1D {
    struct Point {
        double x, y, slope;
        bool operator()(const Point &l, const Point &r) {
            return l.x < r.x;
        }
    };
    std::vector<Point> func;

 public:
    LinInterpol();
    explicit LinInterpol(const std::vector<std::vector<double>>& two_column_xy);
    void init(const std::vector<std::vector<double>>& two_column_xy);
    double evaluate(const double x)const;
    double max()const;
    double min()const;

 private:
    void assert_table_two_columns(
        const std::vector<std::vector<double> > &xy_table,
        const unsigned int row)const;
    void assert_upper_bound_and_argument_in_range(
        const std::vector<Point>::const_iterator it,
        const double arg)const;
    double slope_in_table_in_row(
        const std::vector<std::vector<double> > &xy,
        const unsigned int row)const;
    std::string exception_header()const;
    void sort_function_arguments();
    void assert_no_duplicate_argument_on_sorted_arguments()const;
    static bool comp_upp(const double x, const Point P);
    double interpolate_linear(const Point p0, const double x)const;
    std::vector<Point>::const_iterator get_upper_bound(double arg)const;
    void assert_func_is_not_empty()const;
};

}  // namespace Function

#endif  // CORE_FUNCTION_LININTERPOL_H_