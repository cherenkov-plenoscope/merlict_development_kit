#include "testVector3D.cpp"
#include "testRotation3D.cpp"
#include "testPhoton.cpp"
#include "testWorldFactory.cpp"
#include "gtest/gtest.h"

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
