// Copyright 2014 Sebastian A. Mueller
#include "gtest/gtest.h"
#include "Scenery/FunctionMap.h"
using namespace relleums;

class FunctionMapTest : public ::testing::Test {};

TEST_F(FunctionMapTest, init_empty) {
    FunctionMap my_funcs;
    EXPECT_FALSE(my_funcs.has("mirror_reflectivity_vs_wavelength"));
}

TEST_F(FunctionMapTest, get_not_existing_color) {
    FunctionMap my_funcs;
    EXPECT_THROW(my_funcs.get("f(x)"), FunctionMap::NoSuchKey);
}

TEST_F(FunctionMapTest, add_func_and_check_it_is_in) {
    FunctionMap my_funcs;
    Function::Func1D* function = my_funcs.add<Function::Func1D>("red");
    function->init({{0, 0}, {1, 0}});
    EXPECT_TRUE(my_funcs.has("red"));
}

TEST_F(FunctionMapTest, add_func_and_add_it_again) {
    FunctionMap my_funcs;
    Function::Func1D* function = my_funcs.add<Function::Func1D>("f_vs_x");
    function->init({{0, 0}, {1, 0}});
    EXPECT_THROW(
        my_funcs.add<Function::Func1D>("f_vs_x"),
        FunctionMap::KeyAlreadyInUse);
}

TEST_F(FunctionMapTest, add_func_and_get_it_again) {
    FunctionMap my_funcs;
    Function::Func1D* function = my_funcs.add<Function::Func1D>("f_vs_x");
    function->init({{0, 0}, {1, 0}});
    EXPECT_EQ(function->limits.lower, 0);
    EXPECT_EQ(function->limits.upper, 1);
}

