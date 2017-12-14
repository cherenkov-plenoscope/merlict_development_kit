#include "gtest/gtest.h"
#include "Core/Function/ConstantFunction.h"
#include "Core/PropagationConfig.h"
#include "Core/Rot3.h"
#include "Core/Frame.h"
#include "Core/RayAndFrame.h"
#include "Core/Random/Random.h"
#include "Scenery/Primitive/Plane.h"
#include "Core/Vec3.h"

class PlaneIntersectionTest : public ::testing::Test {
 protected:

	PropagationConfig setup;
	Vec3    pos;
	Rot3  rot;
	Function::Constant* refl_vs_wavl;
	Color*      colo;
	double x_width = 2.5;
	double y_width = 1.3;
	Plane* plane;
	Frame world;
	Random::Mt19937 dice;
	double wavelength = 433e-9;

  PlaneIntersectionTest() {
    // You can do set-up work for each test here.
  }

  virtual ~PlaneIntersectionTest() {
    // You can do clean-up work that doesn't throw exceptions here.
  }
  virtual void SetUp() {

  	dice.set_seed(Random::ZERO_SEED);
  	
  	pos.set(0.0,0.0,0.0);
  	rot.set(0.0,0.0,0.0);

	world.set_name_pos_rot("world",pos,rot);

	refl_vs_wavl = new Function::Constant(1.0, Function::Limits(200e-9, 1200e-9));
	colo = new Color(200,128,128);;

	//------------Plane----------------
	plane = world.append<Plane>();
	plane->set_name_pos_rot("My_Plane", pos, rot);
	plane->set_outer_color(colo);
	plane->set_inner_color(colo);
	plane->set_outer_reflection(refl_vs_wavl);
	plane->set_inner_reflection(refl_vs_wavl);
	plane->set_x_y_width(x_width, y_width);

	//---post initialize the world to calculate all bounding spheres---
	world.init_tree_based_on_mother_child_relations();
  }
};
//------------------------------------------------------------------------------
TEST_F(PlaneIntersectionTest, frontal) {

	Ray ray(Vec3(0.0, 0.0, -1.0), Vec3(0.0, 0.0, 1.0));
	const Intersection intersec = RayAndFrame::first_intersection(&ray, &world);

	ASSERT_TRUE(intersec.does_intersect());
	EXPECT_EQ(plane, intersec.get_object());
	EXPECT_EQ(Vec3::ORIGIN, intersec.position_in_object_frame());
	EXPECT_EQ(Vec3(0.0, 0.0, 1.0), intersec.surface_normal_in_object_frame());
}
//------------------------------------------------------------------------------
TEST_F(PlaneIntersectionTest, frontal_lateral_offset_alwas_intersection) {

	for(int x_offset=-10; x_offset<10; x_offset++) {
		for(int y_offset=-10; y_offset<10; y_offset++) {
			double x_support = double(x_offset)*0.01;
			double y_support = double(y_offset)*0.01;

			Ray ray(Vec3(x_support, y_support, -1.0), Vec3(0.0, 0.0, 1.0));
			const Intersection intersec = RayAndFrame::first_intersection(&ray, &world);

			ASSERT_TRUE(intersec.does_intersect());
			EXPECT_EQ(plane, intersec.get_object());
			EXPECT_EQ(Vec3(x_support, y_support, 0.0), intersec.position_in_object_frame());
			EXPECT_EQ(Vec3(0.0, 0.0, 1.0), intersec.surface_normal_in_object_frame());
		}
	}
}
//------------------------------------------------------------------------------
TEST_F(PlaneIntersectionTest, close_miss_x) {

	Ray ray(Vec3(x_width/2.0+0.01, 0.0, -1.0), Vec3(0.0, 0.0, 1.0));
	const Intersection intersec = RayAndFrame::first_intersection(&ray, &world);
	EXPECT_FALSE(intersec.does_intersect());
}
//------------------------------------------------------------------------------
TEST_F(PlaneIntersectionTest, close_miss_y) {

	Ray ray(Vec3(0.0, y_width/2.0+0.01, -1.0), Vec3(0.0, 0.0, 1.0));
	const Intersection intersec = RayAndFrame::first_intersection(&ray, &world);
	EXPECT_FALSE(intersec.does_intersect());
}
//------------------------------------------------------------------------------
TEST_F(PlaneIntersectionTest, move_plane_up) {

 	pos.set(0.0,0.0,0.0);
  	rot.set(0.0,0.0,0.0);

	world.set_name_pos_rot("world",pos,rot);

	colo = new Color(200,128,128);

	//------------sphere----------------
	pos.set(0.0,0.0,1.0);
	Plane* plane2 = world.append<Plane>();
	plane2->set_name_pos_rot("My_second_plane", pos, rot);
	plane2->set_outer_color(colo);
	plane2->set_inner_color(colo);
	plane2->set_outer_reflection(refl_vs_wavl);
	plane2->set_inner_reflection(refl_vs_wavl);
	plane2->set_x_y_width(x_width, y_width);

	//---post initialize the world to calculate all bounding spheres---
	world.init_tree_based_on_mother_child_relations();

	Ray ray(Vec3(0.0, 0.0, -1.0), Vec3(0.0, 0.0, 1.0));
	const Intersection intersec = RayAndFrame::first_intersection(&ray, &world);
	EXPECT_TRUE(intersec.does_intersect());
}