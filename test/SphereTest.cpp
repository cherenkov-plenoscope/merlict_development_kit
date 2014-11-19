#include <iostream> 
#include <string>
#include <math.h>

#include "gtest/gtest.h"
#include "../Vector3D.h"

using namespace std;

// The fixture for testing class Foo.
class SphereTest : public ::testing::Test {
 protected:
  // You can remove any or all of the following functions if its body
  // is empty.

  SphereTest() {
    // You can do set-up work for each test here.
  }

  virtual ~SphereTest() {
    // You can do clean-up work that doesn't throw exceptions here.
  }

  // If the constructor and destructor are not enough for setting up
  // and cleaning up each test, you can define the following methods:

  virtual void SetUp() {
    // Code here will be called immediately after the constructor (right
    // before each test).
  }

  virtual void TearDown() {
    // Code here will be called immediately after each test (right
    // before the destructor).
  }

  // Objects declared here can be used by all tests in the test case for Foo.
};
//------------------------------------------------------------------------------
TEST_F(SphereTest, set_positiv_radius) {

  bool error_detected = false;
  try{
    Sphere S;
    S.set_sphere(1.0);
  }catch(...){
    error_detected = true;
  }
  EXPECT_FALSE(error_detected);

}
//------------------------------------------------------------------------------
TEST_F(SphereTest, set_negativ_radius) {

  bool error_detected = false;
  try{
    Sphere S;
    S.set_sphere(-1.0);
  }catch(std::exception &err){
    error_detected = true;
  }
  EXPECT_TRUE(error_detected) << "Negativ radius of a sphere is not physicaly";
}
//------------------------------------------------------------------------------
TEST_F(SphereTest, set_zero_radius) {
  bool error_detected = false;
  try{
    Sphere S;
    S.set_sphere(0.0);
  }catch(std::exception &err){
    error_detected = true;
  }
  EXPECT_TRUE(error_detected) << "Spheres with zero radius are not allowed! "
  "Imagine setting up such a sphere and wondering why there aro never "
  "intersections althoug no other errors occured.";
}
//------------------------------------------------------------------------------