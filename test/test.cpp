#include "gtest/gtest.h"

#include "Vector2DTest.cpp"
#include "Vector3DTest.cpp"
#include "RayTest.cpp"
#include "RayAndBoundingSphereOfFrameInteractionTest.cpp"
#include "Rotation3DTest.cpp"
#include "PhotonTest.cpp"
#include "WorldFactoryTest.cpp"
#include "FrameTest.cpp"
#include "ToolTest.cpp"
#include "SphereTest.cpp"
#include "DiscTest.cpp"
#include "SphereIntersectionTest.cpp"
#include "QuadraticEquationTest.cpp"
#include "CameraManTest.cpp"
#include "ApertureCameraTest.cpp"
#include "MmcsCorsikaEventGetterTest.cpp"
#include "FiniteStateMashineTEST.cpp"
#include "RayAndFrameInteractionTest.cpp"
#include "XyPlaneRayEquationTest.cpp"
#include "DualSphericalPrismZTest.cpp"
#include "PlaneIntersectionTest.cpp"
#include "ZaxisCylinderRayIntersectionEquationTest.cpp"
#include "HexagonalPrismZTest.cpp"
#include "BiConvexLensTest.cpp"
#include "HomoTrafo3DTest.cpp"
#include "RayForPropagationTest.cpp"
#include "PropagationEnvironmentTest.cpp"
#include "FresnelRefractionAndReflectionTest.cpp"
#include "TrajectoryFactoryTest.cpp"
#include "HexGridAnnulusTest.cpp"
#include "AsciiIoTest.cpp"
#include "KeyValueMapTest.cpp"
#include "FunctionLinInterpolTest.cpp"
#include "PhotonsTest.cpp"
#include "PostInitFrameSpeed.cpp"
#include "EventIoTest.cpp"
#include "CorsikaPhotonFactoryTest.cpp"
#include "FunctionTest.cpp"
#include "LensMakerTest.cpp"
#include "../Geometry/StereoLitographyIoTest.cpp"
#include "RandomGeneratorTest.cpp"
#include "FitsIoTest.cpp"
#include "SensorStorageTest.cpp"
#include "Histogram1DTest.cpp"

int main(int argc, char **argv) {
	UserInteraction::print_welcome_screen();
  	::testing::InitGoogleTest(&argc, argv);
  	return RUN_ALL_TESTS();
}

