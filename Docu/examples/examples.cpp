#include "set_up_scenery.cpp"

int main(int argc, char **argv) {
	UserInteraction::print_welcome_screen();
  	::testing::InitGoogleTest(&argc, argv);
  	return RUN_ALL_TESTS();
}
