#include "gtest/gtest.h"
#include "Core/Vec3.h"
#include "Core/PropagationConfig.h"
#include "Scenery/Primitive/Sphere.h"
#include "Core/Rot3.h"
#include "Core/Random/Random.h"
#include "Core/PropagationEnvironment.h"
#include "Core/Photon.h"
#include "Core/PhotonAndFrame.h"
#include "Core/Photons.h"
using std::vector;

class SphereIntersectionTest : public ::testing::Test {
protected:

	PropagationConfig setup;
	Vec3    pos;
	Rot3  rot; 
	Color*      colo;
	double radius;
	Sphere* MySphere;
	Frame world;
	Random::Mt19937 dice;
	double wavelength = 433e-9;
	PropagationEnvironment sphere_test_environment;

  SphereIntersectionTest() {
    // You can do set-up work for each test here.
  }

  virtual ~SphereIntersectionTest() {
    // You can do clean-up work that doesn't throw exceptions here.
  }
  virtual void SetUp() {

  	dice.set_seed(Random::ZERO_SEED);
  	pos.set(0.0,0.0,0.0);
  	rot.set(0.0,0.0,0.0);

	world.set_name_pos_rot("world",pos,rot);

	colo = new Color(200,128,128);

	//------------sphere----------------
	radius = 1.0;
	MySphere = world.append<Sphere>();
	MySphere->set_name_pos_rot("MySphere", pos, rot);
	MySphere->set_inner_color(colo);
	MySphere->set_outer_color(colo);
	MySphere->set_radius(radius);

	//---post initialize the world to calculate all bounding spheres---
	world.init_tree_based_on_mother_child_relations();

	sphere_test_environment.world_geometry = &world;
	sphere_test_environment.propagation_options = &setup;
	sphere_test_environment.random_engine = &dice;
  }
};
//------------------------------------------------------------------------------
TEST_F(SphereIntersectionTest, frontal) {
 
	double x_pos = -5.0;

	Vec3 Support(x_pos,0.0,0.0);
	Vec3 direction(1.0,0.0,0.0);

	Photon P(Support,direction,wavelength);
	PhotonAndFrame::Propagator(&P, sphere_test_environment);
	//P.propagate_in(sphere_test_environment);

	ASSERT_EQ(2u, P.get_number_of_interactions_so_far() ) << "There should be 2 "
	"interaction stored in the history, 1 for creation of the photon and 1 for"
	"the intersection with the sphere";

	EXPECT_EQ(-radius-x_pos, P.get_accumulative_distance() ) << 
	"The photon was shot from 5m away from the spheres center. The sphere has "
	"radius 1m. So there are 5-1=4m to traverse. ";

	EXPECT_EQ(
		Vec3::UNIT_X*(-1),
		P.get_intersection_at(1).surface_normal_in_object_frame()
	);
}
//------------------------------------------------------------------------------
TEST_F(SphereIntersectionTest, emmitting_close_above_surface_tangential) {

	Vec3 Support(0.0,0.0,1.0+1e-9);
	Vec3 direction(1.0,0.0,0.0);

	Photon P(Support,direction,wavelength);
	PhotonAndFrame::Propagator(&P, sphere_test_environment);
	//P.propagate_in(sphere_test_environment);

	ASSERT_EQ(absorption_in_void, P.get_final_interaction_type() );
}
//------------------------------------------------------------------------------
TEST_F(SphereIntersectionTest, emmitting_close_above_surface_straigtht_away) {

	Vec3 Support(0.0,0.0,1.0+1e-9);
	Vec3 direction(0.0,0.0,1.0);

	Photon P(Support,direction,wavelength);
	PhotonAndFrame::Propagator(&P, sphere_test_environment);
	//P.propagate_in(sphere_test_environment);

	ASSERT_EQ(absorption_in_void, P.get_final_interaction_type() );
}
//------------------------------------------------------------------------------
TEST_F(SphereIntersectionTest, tangential_intersection) {

	Vec3 Support(-5.0,0.0,1.0);
	Vec3 direction(1.0,0.0,0.0);

	Photon P(Support,direction,wavelength);
	PhotonAndFrame::Propagator(&P, sphere_test_environment);
	//P.propagate_in(sphere_test_environment);

	ASSERT_EQ(2u, P.get_number_of_interactions_so_far() );

	Vec3 normal = Vec3::UNIT_Z;

	EXPECT_NEAR(
		0.0,
		normal.distance_to(
			P.get_intersection_at(1).surface_normal_in_object_frame()
		),
		1e-12
	);
}
//------------------------------------------------------------------------------
// NEW INTERSECTION METHOD
//------------------------------------------------------------------------------
TEST_F(SphereIntersectionTest, ray_frontal_intersection) {

  Ray ray_with_intersection(Vec3(0.0,0.0,-2.0), Vec3(0.0,0.0,1.0));

  vector<Intersection> intersections;
  MySphere->calculate_intersection_with(&ray_with_intersection, &intersections);

  ASSERT_FALSE(intersections.empty());
  EXPECT_EQ(intersections.front().get_object(), MySphere);
  EXPECT_EQ(
  	Vec3(0.0,0.0,-1.0),
  	intersections.front().position_in_object_frame()
  );
  EXPECT_EQ(
  	Vec3(0.0,0.0,-1.0),
  	intersections.front().surface_normal_in_object_frame()
  );
}
//------------------------------------------------------------------------------
TEST_F(SphereIntersectionTest, ray_intersection_but_no_causal_intersection) {

  Ray ray_without_intersection(Vec3(0.0,0.0,+2.0), Vec3(0.0,0.0,1.0));

  vector<Intersection> intersections;
  MySphere->calculate_intersection_with(&ray_without_intersection, &intersections);

  EXPECT_TRUE(intersections.empty());
}
//------------------------------------------------------------------------------
TEST_F(SphereIntersectionTest, ray_completely_outside_of_sphere) {

  Ray ray_outside(Vec3(5.0,0.0,0.0), Vec3(0.0,0.0,1.0));

  vector<Intersection> intersections;
  MySphere->calculate_intersection_with(&ray_outside, &intersections);

  EXPECT_TRUE(intersections.empty());
}
//------------------------------------------------------------------------------
TEST_F(SphereIntersectionTest, ray_starts_inside_sphere) {

  Ray ray_inside(Vec3::ORIGIN, Vec3(0.0,0.0,1.0));

  vector<Intersection> intersections;
  MySphere->calculate_intersection_with(&ray_inside, &intersections);

  ASSERT_FALSE(intersections.empty());
  EXPECT_EQ(intersections.front().get_object(), MySphere);
  EXPECT_EQ(
  	Vec3(0.0,0.0,+1.0),
  	intersections.front().position_in_object_frame()
  );
  EXPECT_EQ(
  	Vec3(0.0,0.0,+1.0),
  	intersections.front().surface_normal_in_object_frame()
  );
}
//------------------------------------------------------------------------------
TEST_F(SphereIntersectionTest, ray_tangents_sphere) {

  Ray ray_inside(Vec3(1.0, 0.0, -2.0), Vec3(0.0, 0.0, 1.0));

  vector<Intersection> intersections;
  MySphere->calculate_intersection_with(&ray_inside, &intersections);

  ASSERT_FALSE(intersections.empty());
  EXPECT_EQ(intersections.front().get_object(), MySphere);
  EXPECT_EQ(
  	Vec3(1.0, 0.0, 0.0),
  	intersections.front().position_in_object_frame())
  ;
  EXPECT_EQ(
  	Vec3(1.0, 0.0, 0.0),
  	intersections.front().surface_normal_in_object_frame()
  );
}
//------------------------------------------------------------------------------