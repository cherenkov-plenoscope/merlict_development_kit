// Copyright 2014 Sebastian A. Mueller
#ifndef MERLICT_FUNCTION_LIMITS_H_
#define MERLICT_FUNCTION_LIMITS_H_

#include <string>

namespace merlict {
namespace function {

struct Limits{
    double lower;
    double upper;
    void assert_causal()const;
    Limits();
    Limits(const double _lower, const double _upper);
    double range()const;
    void assert_contains(const double x)const;
    std::string str()const;
};

}  // namespace function
}  // namespace merlict

#endif  // MERLICT_FUNCTION_LIMITS_H_
