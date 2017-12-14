#include "FlyingCamera.h"
#include "Core/RayAndFrame.h"
#include "Core/Intersection.h"
#include <sstream>
using std::cout;
using std::stringstream;
using std::string;

namespace Visual {

//------------------------------------------------------------------------------
FlyingCamera::FlyingCamera(
	const Frame *world, 
	const Config *visual_config
) {	
	this->world = world;
	this->visual_config = visual_config;

	flying_camera = new PinHoleCamera("Cam", 
		visual_config->preview.cols, 
		visual_config->preview.rows
	);
	
	flying_camera_full_resolution = new PinHoleCamera("Cam", 
		visual_config->preview.cols*visual_config->preview.scale, 
		visual_config->preview.rows*visual_config->preview.scale
	);

	create_CameraMen_to_safely_operate_the_flying_camera();
	reset_camera();
	time_stamp.update_now();
	
	create_display();
	enter_interactive_display();
}
//------------------------------------------------------------------------------
FlyingCamera::~FlyingCamera() {
	destroy_display();
}
//------------------------------------------------------------------------------
void FlyingCamera::create_CameraMen_to_safely_operate_the_flying_camera() {
	fov_operator = new CameraMan::FieldOfView(flying_camera_full_resolution);
	fov_operator->set_verbosity(true);

	translation_operator = new CameraMan::Translation(flying_camera_full_resolution);
	translation_operator->set_verbosity(true);

	rotation_operator = new CameraMan::Rotation(flying_camera_full_resolution);
	rotation_operator->set_verbosity(true);

	stereo_operator = new CameraMan::Stereo3D(flying_camera_full_resolution);
	stereo_operator->set_verbosity(true);
}
//------------------------------------------------------------------------------
void FlyingCamera::reset_camera() {
	translation_operator->set_default_position(Vec3(0.0, 0.0, 0.0));
	rotation_operator->set_default_rotation(Rot3(0.0,Deg2Rad(-90.0),0.0));
	fov_operator->set_default_FoV();
}
//------------------------------------------------------------------------------
void FlyingCamera::enter_interactive_display() {
	UserInteraction::print_welcome_screen();
	print_display_help_text();

	int user_input_key = 0;
	bool key_stroke_requires_image_update = true;

	while(!UserInteraction::is_Escape_key(user_input_key)) {

		user_input_counter++;

		if(it_is_time_again_to_show_the_help())
			print_display_help_text();

		if(key_stroke_requires_image_update)
			update_display();
		
		key_stroke_requires_image_update = true;
		user_input_key = wait_for_user_key_stroke();

		switch(user_input_key){
			case 't': toggle_stereo3D();
			break;
			case 'w': translation_operator->move_forward();
			break;
			case 's': translation_operator->move_back();
			break;
			case 'a': translation_operator->move_left();
			break;
			case 'd': translation_operator->move_right();
			break;
			case 'q': translation_operator->move_up();
			break;
			case 'e': translation_operator->move_down();
			break;
			case 'n': fov_operator->increase_when_possible();
			break;
			case 'm': fov_operator->decrease_when_possible();
			break;
			case 'i': rotation_operator->look_further_up_when_possible();
			break;
			case 'k': rotation_operator->look_further_down_when_possible();
			break;
			case 'j': rotation_operator->look_left();
			break;
			case 'l': rotation_operator->look_right();
			break;
			case 'x': stereo_operator->increase_stereo_offset();
			break;
			case 'y': stereo_operator->decrease_stereo_offset();
			break;
			case 'g': translation_operator->move_to(UserInteraction::get_Vec3());
			break;
			case UserInteraction::space_key: {
				update_display_full_resolution();
				key_stroke_requires_image_update = false;
			}
			break;
			case 'p': {
				cout << world->get_tree_print() << "\n";
				key_stroke_requires_image_update = false;
			}
			break;
			case 'h': {
				print_display_help_text();
				key_stroke_requires_image_update = false;
			};
			break;
			default: key_stroke_requires_image_update = false;
		}
	}
}
//------------------------------------------------------------------------------
int FlyingCamera::wait_for_user_key_stroke() {
	return cvWaitKey(0);
}
//------------------------------------------------------------------------------
bool FlyingCamera::it_is_time_again_to_show_the_help() {
	if(user_input_counter > 15) {
		user_input_counter = 0;
		return true;
	}else{
		return false;
	}
}
//------------------------------------------------------------------------------
void FlyingCamera::create_display() {
	cv::namedWindow( display_name, CV_WINDOW_AUTOSIZE );
	FlyingCamera* p = this;
	cv::setMouseCallback( 
		display_name.c_str(), 
		mouse_button_event, 
		(void *)p 
	);
}
//------------------------------------------------------------------------------
void FlyingCamera::destroy_display() {
	cv::destroyWindow(display_name);	
}
//------------------------------------------------------------------------------
void FlyingCamera::update_display_full_resolution() {
	cout << "Full resolution image "
	<< flying_camera_full_resolution->get_number_of_sensor_cols() <<"x"
	<< flying_camera_full_resolution->get_number_of_sensor_rows() <<", "
	<< flying_camera_full_resolution->get_number_of_sensor_cols()*
	flying_camera_full_resolution->get_number_of_sensor_rows()/1e6 
	<< " MPixel\n";

	const Image* img = acquire_scaled_image_with_camera(
		false,
		flying_camera_full_resolution
	);

	cv::imshow(display_name, img->raw_image); 
}
//------------------------------------------------------------------------------
void FlyingCamera::update_display() {
	flying_camera->set_FoV_in_rad(
		flying_camera_full_resolution->get_FoV_in_rad()
	);

	flying_camera->update_position_and_orientation(
		flying_camera_full_resolution->get_position_in_world(),
		flying_camera_full_resolution->get_rotation_in_world()
	);

	const Image* img = acquire_scaled_image_with_camera(true, flying_camera);
	cv::imshow(display_name, img->raw_image); 
}
//------------------------------------------------------------------------------
void FlyingCamera::mouse_button_event(
	int event, int col, int row, int flags, void *param
) {
	(void)flags;
	FlyingCamera* p = (FlyingCamera*)param;
	
	if( event == cv::EVENT_LBUTTONDOWN )
		p->print_info_of_probing_ray_for_pixel_col_row( col, row );
	else if( event == cv::EVENT_RBUTTONDOWN )
		p->take_snapshot_manual_focus_on_pixel_col_row( col, row );
	else
		return;
}
//------------------------------------------------------------------------------
void FlyingCamera::toggle_stereo3D() {
	stereo3D = !stereo3D;
	cout << "Stereo 3D : " << (stereo3D ? "On" : "Off") << "\n";
}
//------------------------------------------------------------------------------
string FlyingCamera::get_snapshot_filename() {
	snapshot_counter++;

	stringstream filename;  
	filename << time_stamp.yyyy() << time_stamp.mm() << time_stamp.dd() << "_";
	filename << time_stamp.HH() << time_stamp.MM() << time_stamp.SS();
	filename << "_" << snapshot_counter;
	return filename.str();
}
//------------------------------------------------------------------------------
ApertureCamera FlyingCamera::get_ApertureCamera_based_on_display_camera()const{

	ApertureCamera apcam("Imax70mm", 
		visual_config->snapshot.cols, 
		visual_config->snapshot.rows
	);

	apcam.set_fStop_sesnorWidth_rayPerPixel(
		visual_config->snapshot.focal_length_over_aperture_diameter, 
		visual_config->snapshot.image_sensor_size_along_a_row, 
		visual_config->snapshot.rays_per_pixel
	);

	apcam.set_FoV_in_rad(flying_camera_full_resolution->get_FoV_in_rad());
	apcam.update_position_and_orientation(
		flying_camera_full_resolution->get_position_in_world(),
		flying_camera_full_resolution->get_rotation_in_world()
	);
	return apcam;
}
//------------------------------------------------------------------------------
void FlyingCamera::take_snapshot_manual_focus_on_pixel_col_row(int col, int row) {

	Ray probing_ray = 
		flying_camera_full_resolution->get_ray_for_pixel_in_row_and_col(row, col);
	
	DistanceMeter dist_meter(&probing_ray, world);

	double object_distance_to_focus_on;

	if(dist_meter.faces_an_object)
		object_distance_to_focus_on = dist_meter.distance_to_closest_object;
	else
		object_distance_to_focus_on = 9e99; // a very large distance
	
	ApertureCamera apcam = get_ApertureCamera_based_on_display_camera();
	apcam.set_focus_to(object_distance_to_focus_on);
	cout << apcam.str();

	const Image* img = acquire_scaled_image_with_camera(false ,&apcam);
	img->save(get_snapshot_filename());
}
//------------------------------------------------------------------------------
const Image* FlyingCamera::acquire_scaled_image_with_camera(
	const bool scale, CameraDevice* cam
) {	
	if(scale){
		
		if(stereo3D) {

			CameraMan::Stereo3D op(cam);
			op.use_same_stereo_offset_as(stereo_operator);
			op.aquire_stereo_image(world, visual_config);
			image = *op.get_anaglyph_stereo3D_image();
			image.scale(visual_config->preview.scale);
			return &image;
		}else{

			cam->acquire_image(world, visual_config);
			image = *cam->get_image();
			image.scale(visual_config->preview.scale);
			return &image;
		}	
	}else{

		if(stereo3D) {

			CameraMan::Stereo3D op(cam);
			op.use_same_stereo_offset_as(stereo_operator);
			op.aquire_stereo_image(world, visual_config);
			return op.get_anaglyph_stereo3D_image();
		}else{

			cam->acquire_image(world, visual_config);
			return cam->get_image();
		}	
	}
}
//------------------------------------------------------------------------------
void FlyingCamera::print_info_of_probing_ray_for_pixel_col_row(int col, int row) {

	Ray probing_ray = flying_camera_full_resolution->
		get_ray_for_pixel_in_row_and_col(row, col);

	Intersection intersec = RayAndFrame::first_intersection(&probing_ray, world);

	UserInteraction::ClearScreen();
	stringstream out;

	//      0        1         2         3         4         5         6
	//      123456789012345678901234567890123456789012345678901234567890

	out << " _Info_for_Pixel_(_" << col << "_|_" << row << "_)__________________\n";
	out << "|\n";
	out << "| Ray emitted by camera:\n";
	out << "| " << probing_ray.str() << "\n";
	out << "|\n";
	if(intersec.does_intersect())
		out << get_intersection_info_print(intersec);
	else
		out << "| No intersection with any object.\n";	
	out << "|___________________________________________________________\n";
	cout << out.str();
}
//------------------------------------------------------------------------------
string FlyingCamera::get_intersection_info_print(const Intersection intersec)const {
	
	stringstream out;
	out << "| Object: " << intersec.get_object()->get_path_in_tree_of_frames() << "\n";
	out << "| Distance to first intersection: " << intersec.distance_to_ray_support() << "m\n";
	out << "|\n";
	out << "| In frame of intersecting object\n";
	out << "| | intesection point: " << intersec.position_in_object_frame().str() << "\n";
	out << "| | surface normal   : " << intersec.surface_normal_in_object_frame().str() << "\n";
	out << "| | facing surface   : " << (intersec.from_outside_to_inside() ? "outside" : "inside") << "\n";
	out << "|\n";
	out << "| In world frame\n";
	out << "| | intesection point: " << intersec.position_in_root_frame().str() << "\n";
	out << "| | surface normal   : " << intersec.surface_normal_in_root_frame().str() << "\n";
	out << "|\n";		
	out << StringTools::place_first_infront_of_each_new_line_of_second(
		"| ",
		intersec.get_object()->str()
	);
	return out.str();	
}
//------------------------------------------------------------------------------
void FlyingCamera::print_display_help_text()const {

	UserInteraction::ClearScreen();
	stringstream out;

//  0         1         2         3         4         5         6         7         8
//   12345678901234567890123456789012345678901234567890123456789012345678901234567890
	out << 
	"_Position______________________   _Orientation___________________\n"
	" move forward............[ w ]     look up.................[ i ]\n"
	" move backward...........[ s ]     look down...............[ k ]\n"
	" move left...............[ a ]     look left...............[ j ]\n"
	" move right..............[ d ]     look right..............[ l ]\n"
	" move up.................[ q ]\n"
	" move down...............[ e ]\n"
	" move to custom pos......[ g ]\n"
	"\n"
	"_Stereo3D_left:red_right:blue__   _Field_of_View_________________\n"
	" on/off..................[ t ]     increace................[ n ]\n"
	" increase................[ x ]     decreace................[ m ]\n"
	" decrease................[ y ]\n"
	"                                  _interaction__________________\n"
	"_Aperture_camera_______________    print help..............[ h ]\n"
	" manual focus..[ right mouse ]     print geometry tree.....[ p ]\n"
	"                                   exit/next photon........[ESC]\n"
	"\n"
	"[  space key  ] full resolution.\n"
	"[ left mouse  ] click image for additional information.\n"
	"[ right mouse ] click image for manual focus with snapshot.\n\n";
	cout << out.str();
}
//------------------------------------------------------------------------------
void FlyingCamera::continue_with_new_scenery_and_visual_config(
	const Frame *world, 
	const Config *visual_config
) {
	this->world = world;
	this->visual_config = visual_config;
	enter_interactive_display();
}
//------------------------------------------------------------------------------
}//Visual