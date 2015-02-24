#include "CartesianFrame.h"
//------------------------------------------------------------------------------
CartesianFrame::CartesianFrame(
    const std::string new_name,
    const Vector3D    new_pos,
    const Rotation3D  new_rot
) { 
    set_frame(new_name, new_pos, new_rot); 
}
//------------------------------------------------------------------------------
uint CartesianFrame::get_number_of_children()const { 
	return children.size(); 
}
//------------------------------------------------------------------------------
void CartesianFrame::post_initialize_transformations() {
	// in the set_frame() function the transformation frame2mother has been set
	// using the new relative position to the mother and the new relative
	// orientation/rotation with respect to the mother frame.
	// All other transformations are calculated using this first frame2mother.

	// The mother2frame is just the inverse of the given frame2mother
	T_mother2frame = T_frame2mother.inverse();

	// For the transformations with respect to the root/world frame we have to 
	// recalculate the current frame2mother transformation by concatination of 
	// all the frame2mother transformations of this frame up to the root frame
	T_frame2world = calculate_frame2world();

	// And again world2frame is just the inverse of the frame2world
	T_world2frame = T_frame2world.inverse();	
	
	// Now we set pos_in_world after all transformations took place
	// This is needed for the bounding spheres later on. Since we use spheres 
	// for the pre intersection tests we do not need the relative rotation of 
	// this frame relative to the root frame to be stored in this frame. 
	pos_in_world = T_frame2world.get_translation();
}
//------------------------------------------------------------------------------
HomoTrafo3D CartesianFrame::calculate_frame2world()const {
	// All parent frames of this frame do already know their frame2mother 
	// relation.
	// To calculate the frame2world relation of this frame we have to 
	// conacatenate all the frame2mother transformations from this frame to the
	// root frame.

	// The first step toward the root is the mother of this frame
	CartesianFrame* frame_on_our_way_towards_the_root = mother;
	
	// The starting point for the latter frame2world is this framse frame2motehr
	HomoTrafo3D Trafo_on_our_way_towards_the_root = T_frame2mother;	

	while(frame_on_our_way_towards_the_root != nullptr){

		Trafo_on_our_way_towards_the_root = 
		frame_on_our_way_towards_the_root -> 
		T_frame2mother*Trafo_on_our_way_towards_the_root;

		frame_on_our_way_towards_the_root = 
		frame_on_our_way_towards_the_root->mother; 
	}

	return Trafo_on_our_way_towards_the_root;
}
//------------------------------------------------------------------------------
void CartesianFrame::set_frame(
	const std::string name_of_frame,
	const Vector3D pos_in_mother,
	const Rotation3D rot_in_mother
){
	assert_name_is_valid(name_of_frame);
	this->name_of_frame = name_of_frame;
	
	this->pos_in_mother = pos_in_mother;
	this->rot_in_mother = rot_in_mother;

	// The only transformation set yet is the frame2mother. The others are set
	// when the construction of the world tree has finished and the post 
	// initialization is performed.
	T_frame2mother.set_transformation(rot_in_mother, pos_in_mother);

	reset_all_connections_to_children_and_mother();
}
//------------------------------------------------------------------------------
void CartesianFrame::reset_all_connections_to_children_and_mother() {
	radius_of_sphere_enclosing_all_children = 0.0;	 
	mother = nullptr;
	root_of_world = this;
	children.clear();
}
//------------------------------------------------------------------------------
void CartesianFrame::assert_name_is_valid(const std::string name_to_check)const{
	assert_name_is_not_empty(name_to_check);
	assert_name_has_no_whitespaces(name_to_check);
	assert_name_has_no_delimiter_symbol(name_to_check);
}
//------------------------------------------------------------------------------
void CartesianFrame::assert_name_is_not_empty(
	const std::string name_to_check)const
{
	// A name must not be empty!
	if(name_to_check.empty()){
		std::stringstream info;
		info << "CartesianFrame::" << __func__ << "()\n";
		info << "The name of a frame must not be empty!\n";
		throw TracerException(info.str(), EMPTY_NAME_OF_FRAME);
	}
}
//------------------------------------------------------------------------------
void CartesianFrame::assert_name_has_no_whitespaces(
	const std::string name_to_check)const
{
	// A name must not use whitespaces!
	for(auto single_character : name_to_check){
		if(isspace(single_character)){
			std::stringstream info;
			info << "CartesianFrame::" << __func__ << "()\n";
			info << "The name of a frame must not contain blank characters! ";
			info << "But the name is actual: '" << name_to_check << "'.\n";
			throw TracerException(info.str(), WHITE_SPACE_IN_NAME_OF_FRAME);			
		}
	}
}
//------------------------------------------------------------------------------
void CartesianFrame::assert_name_has_no_delimiter_symbol(
	const std::string name_to_check)const
{
	// A name must not contain the delimiter sign
	if(StringTools::string_contains_char(name_to_check, delimiter_for_frame_path)) {
		std::stringstream info;
		info << "CartesianFrame::" << __func__ << "()\n";
		info << "The name of a frame must not contain the delimiter symbol: ";
		info << "'" << delimiter_for_frame_path << "'. ";
		info << "But the name is actual: '" << name_to_check << "'.\n";
		throw TracerException(info.str(), DELIMITER_SYMBOL_IN_NAME_OF_FRAME);		
	}
}
//------------------------------------------------------------------------------
std::string CartesianFrame::get_print()const {
	return get_frame_print();
}
//------------------------------------------------------------------------------
std::string CartesianFrame::get_frame_print()const {
	std::stringstream out;
	out << "frame: " << name_of_frame << "\n";
	out << "| pos in mother: " << pos_in_mother << "\n";
	out << "| rot in mother: " << rot_in_mother << "\n";
	out << "| pos in world:  " << pos_in_world << "\n";
	out << "| enclosing boundary radius: " << radius_of_sphere_enclosing_all_children << "m\n";	
	return out.str();
}
//------------------------------------------------------------------------------
std::string CartesianFrame::get_tree_print()const {
	
	std::stringstream out;
	out << name_of_frame << ", " << get_number_of_children();
	out << " children, " << radius_of_sphere_enclosing_all_children << "m\n";

	for(CartesianFrame* child : children)
		out << "| " << child->get_tree_print();

	return out.str();
}
//------------------------------------------------------------------------------
void CartesianFrame::set_mother(CartesianFrame *const new_mother) {
	mother = new_mother;
}
//------------------------------------------------------------------------------
void CartesianFrame::add_child(CartesianFrame * const new_child) {
	children.push_back(new_child);
	update_sphere_enclosing_all_children(new_child);
}
//------------------------------------------------------------------------------
void CartesianFrame::update_sphere_enclosing_all_children(
	CartesianFrame *new_child
) {
	// When a child frame is added to a frame we have to check if the sphere 
	// enclosing all the frames previous children is also enclosing the new
	// child. In case the old sphere is to small we have to increase its radius
	// to enclose all the childrens again.
	// To do so we calculate the radius needed to enclose only the new child.
	
	// Case: The new child is not enclosed by the previous sphere of this frame
	//-------             _______________                                     //
	//               ____/               \____                                //
	//            __/     \                   \__                             //
	//           /         \  radius             \                            //
    //          |           \  enclosing          |                           //
    //         |             \  all                |                          //
    //         |              \  (previous)        |          new child       //
    //        |  ___           \  children          |            ___          //
	//        |/     \          \                   |          /     \        //
    //        |       |          x------------------|---------|---x   |       //
    //        |\ ___ /         frame                |          \ _._ /.       //
    //        | previous child   .                  |             .   . 
    //         | defining        .                 |              .   .
    //         |  the current    .                 |              .   .
	//          |  radius        .                |               .   .
	//           \__             .             __/                .   .
	//              \___         .        ____/                   .   .
	//                  \________._______/                        .   .
	//                           .                                .   .
	//                           \_________________  ____________/\_ _/
	//                                             \/               V
	//                                   new childs rel. pos    new childs
	//                                      to mother frame       radius
	//                                             \________  _______/
	//                                                      \/
	//                         new radius of sphere enclosing all children   
	// 
	// Here we have to update the radius of the sphere enclosing all children.
	//      
	// Case: The new child is not enclosed by the previous sphere of this frame
	//-------
	//                    _______________
	//               ____/               \____                                //
	//            __/     \ radius            \__                             // 
	//           /         \ enclosing           \                            //
    //          |           \  all                |                        
    //         |             \  (previous)         |
    //         |              \  children           |            
    //        |                \           ___      |            
	//        |                 \        /     \    |          
    //        |                  x------|---x   |   |
    //        |                frame     \ ___ /    |          
    //        |                  .      new child   |                 
    //         |                 .                 |                 
    //         |                 .                 |                  
	//          |                .                |                   
	//           \__             .             __/                    
	//              \___         .        ____/                       
	//                  \________________/                            
	//                                                              
	//                             
	// In this case the old radius remains because it is already enclosing the 
	// new child.

	double radius_needed_to_enclose_new_child = 
	new_child->pos_in_mother.norm2() + 
	new_child->radius_of_sphere_enclosing_all_children;

	if(	radius_needed_to_enclose_new_child > 
		radius_of_sphere_enclosing_all_children
	)
		radius_of_sphere_enclosing_all_children = 
		radius_needed_to_enclose_new_child;	
}
//------------------------------------------------------------------------------
void CartesianFrame::set_mother_and_child(CartesianFrame *new_child) {
	this->add_child(new_child);
	new_child->set_mother(this);
}
//------------------------------------------------------------------------------
void CartesianFrame::setup_tree_based_on_mother_child_relations() {
	post_initialize_me_and_all_my_children();
	update_enclosing_sphere_for_all_children();
	post_initialize_root_of_world();
}
//------------------------------------------------------------------------------
void CartesianFrame::post_initialize_me_and_all_my_children() {
	// post initialize all frames in world tree of frames.
	// This has to be done to ensure the relationships between each frame and
	// the root frame are declared in each frame.
	post_initialize_transformations();

	// and all children
	for(CartesianFrame* child : children)
		child->post_initialize_me_and_all_my_children();
}
//------------------------------------------------------------------------------
void CartesianFrame::update_enclosing_sphere_for_all_children() {
	// when there is already a treee structure of frames and one is adding a
	// new frame later on, makeing it the child of a previous frame, then one
	// has to update the spheres enclosing the children of all mother frames
	// of the frame where the new frame was added to.
	// Here we do this in a recursive way.

	for(CartesianFrame *child : children){
		child->update_enclosing_sphere_for_all_children();
		update_sphere_enclosing_all_children(child);
	}
}
//------------------------------------------------------------------------------
void CartesianFrame::post_initialize_root_of_world() {

	if(has_mother())
		root_of_world = mother->get_root_of_world();
	else
		root_of_world = this;

	for(CartesianFrame* child : children)
		child->post_initialize_root_of_world();
}
//------------------------------------------------------------------------------
const CartesianFrame* CartesianFrame::get_root_of_world()const {
	return root_of_world;
}
//------------------------------------------------------------------------------
void  CartesianFrame::take_children_from(
	CartesianFrame *frame_to_take_chidren_from
) {
	// take all children of the frame_to_take_chidren_from and 
	// put them to this frame

	for(CartesianFrame *child_to_take: frame_to_take_chidren_from->children)
		set_mother_and_child( child_to_take);
}
//------------------------------------------------------------------------------
const CartesianFrame* CartesianFrame::get_frame_in_tree_by_path( 
	std::string path_to_frame 
)const{
	/// Input is a string called path. It is string of concatenated sub strings 
	/// seperated by delimiter '/'. The sub strings are the names of the frames.
	/// The delimiter represents the tree structure of mother and children 
	/// frames.
	/// All frame tree structure is meant to start with a root frame called 
	/// world. This frame is not explicitly indicated in the paths. It is only
	/// indicated by a leading delimiter symbol.
	/// eg.      /house/roof/chimney/chimney_wall_2 insead of
	///     world/house/roof/chimney/chimney_wall_2

	StringTools::remove_char_from_text_if_leading(
		delimiter_for_frame_path, path_to_frame
	);

	std::string name_of_leading_frame = 
		StringTools::cut_leading_token_infront_of_delimiter(
			path_to_frame,
	 		delimiter_for_frame_path
		);

	if( has_child_with_name(name_of_leading_frame) ){

		if( path_to_frame.empty() ){
			// There is no path left, so this frame is actually the final child 
			// we are looking for
			return get_child_by_name(name_of_leading_frame);
		}else{
			// There is still path to check. We have not reached our final
			// Frame yet so lets proceed with the child
			return get_child_by_name(name_of_leading_frame)->
			get_frame_in_tree_by_path(path_to_frame);
		}
	}else{
		// there is no child on this frame which matches the name of the leading
		// frame. This is: There is no such Frame in this Tree of frames as 
		// described in path.
		return nullptr;
	}
}
//------------------------------------------------------------------------------
const CartesianFrame* CartesianFrame::get_child_by_name( 
	std::string specific_name 
)const{
	for( CartesianFrame* child : children )
		if( StringTools::is_equal(child->name_of_frame, specific_name) )
			return child;

	return nullptr;
}
//------------------------------------------------------------------------------
std::string CartesianFrame::get_path_in_tree_of_frames()const {
	/// The path of a frame is returned here. The root frame called world is not 
	/// included in the path. The delimiter sign is '/' as for directorys on 
	/// unix systems.
	/// eg. City/Street14/house18/roof/chimney/chimney_wall_2

	// chech if this frame has a mother frame 
	if( has_mother() ){
		// This frame has a mother. Therefore it is not the root frame. 
		// Here we add at least the delimiter to the path and ,at least there 
		// is one, the path of its mother
		return mother->get_path_in_tree_of_frames() + 
			delimiter_for_frame_path + name_of_frame;
	}else{
		// This frame has not a mother. So this is the root frame. Here is 
		// nothing added to the string
		return "";
	}
}
//------------------------------------------------------------------------------
bool CartesianFrame::has_child_with_name(const std::string name_of_child)const {
	return get_child_by_name(name_of_child) != nullptr;
}
//------------------------------------------------------------------------------
bool CartesianFrame::has_mother()const {
	return mother != nullptr;
}
//------------------------------------------------------------------------------
bool CartesianFrame::has_children()const {
	return get_number_of_children() > 0; 
}
//------------------------------------------------------------------------------
#include "Ray.h"
#include "Intersection.h"
//------------------------------------------------------------------------------
Intersection* CartesianFrame::calculate_intersection_with(const Ray* ray)const {
	return empty_intersection();
}
//------------------------------------------------------------------------------
Intersection* CartesianFrame::empty_intersection()const {
	Intersection* intersection;
	intersection = new Intersection();
	return intersection;
}
//------------------------------------------------------------------------------
void CartesianFrame::find_intersection_candidates_for_all_children_and_ray(
	const Ray* ray,
	std::vector<const CartesianFrame*> *candidate_frames
)const {

	for(CartesianFrame *child : children)
		ray->find_intersection_candidates_in_tree_of_frames(child, candidate_frames);
}