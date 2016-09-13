#include "gtest/gtest.h"
#include "Scenery/FunctionMap.h"

class FunctionMapTest : public ::testing::Test {};
//------------------------------------------------------------------------------
TEST_F(FunctionMapTest, init_empty) {

    FunctionMap my_funcs;
    EXPECT_FALSE(my_funcs.has("mirror_reflectivity_vs_wavelength"));
}
//------------------------------------------------------------------------------
TEST_F(FunctionMapTest, get_not_existing_color) {

    FunctionMap my_funcs;
    EXPECT_THROW(my_funcs.get("f(x)"), FunctionMap::NoSuchKey);
}
//------------------------------------------------------------------------------
TEST_F(FunctionMapTest, add_func_and_check_it_is_in) {

    FunctionMap my_funcs;
    Function::Polynom3* function = my_funcs.add<Function::Polynom3>("red");
    function->set_limits(Function::Limits(0.0, 1.0));
    function->set_coefficients(0.0, 1.0, 0.0, 1.0);
    EXPECT_TRUE(my_funcs.has("red"));
}
//------------------------------------------------------------------------------
TEST_F(FunctionMapTest, add_func_and_add_it_again) {

    FunctionMap my_funcs;
    Function::Polynom3* function = my_funcs.add<Function::Polynom3>("f_vs_x");
    function->set_limits(Function::Limits(0.0, 1.0));
    function->set_coefficients(0.0, 1.0, 0.0, 1.0);
    EXPECT_THROW(
        my_funcs.add<Function::Polynom3>("f_vs_x"),
        FunctionMap::KeyAlreadyInUse
    );
}
//------------------------------------------------------------------------------
TEST_F(FunctionMapTest, add_func_and_get_it_again) {

    FunctionMap my_funcs;
    Function::Polynom3* function = my_funcs.add<Function::Polynom3>("f_vs_x");
    Function::Limits my_limits(0.0, 1.0);
    function->set_limits(my_limits);
    function->set_coefficients(0.0, 1.0, 0.0, 1.0);

    EXPECT_EQ(function->get_limits().get_lower(), my_limits.get_lower());
    EXPECT_EQ(function->get_limits().get_upper(), my_limits.get_upper());
}
//------------------------------------------------------------------------------