#include "MmcsCorsikaRunHeader.h"
#include <cmath>
//------------------------------------------------------------------------------
MmcsCorsikaRunHeader::MmcsCorsikaRunHeader()
{
	// nothing;
}
//------------------------------------------------------------------------------
MmcsCorsikaRunHeader::MmcsCorsikaRunHeader(const std::vector<float>& block){
	MmcsCorsikaSubBlock dummy;
	for (size_t i=0; i<block.size(); i++)
		dummy[i] = block[i];

	create_from_mmcs_sub_block(dummy);
}
//------------------------------------------------------------------------------
void MmcsCorsikaRunHeader::create_from_mmcs_sub_block(
	const MmcsCorsikaSubBlock &mmcs_sub_block
) {
	this->mmcs_sub_block = mmcs_sub_block;
	assert_sub_block_has_Run_Header();
	
	run_number = mmcs_sub_block[1];
	date_of_begin_run = int(std::round(mmcs_sub_block[2]));
	version_of_program = mmcs_sub_block[3];	
	fill_observation_levels();
	fill_from(slope_of_energy_spektrum, 15);
    fill_from_until(energy_range, 16, 18);

	flag_for_EGS4_treatment_of_em_component = 
		mmcs_sub_block.get_bool_representation_at(18);
    flag_for_NKG_treatment_of_em_component =
    	mmcs_sub_block.get_bool_representation_at(19);

    fill_from(kin_energy_cutoff_for_hadrons_in_GeV, 20);
    fill_from(kin_energy_cutoff_for_muons_in_GeV, 21);
	fill_from(kin_energy_cutoff_for_electrons_in_GeV, 22);
	fill_from(energy_cutoff_for_photons_in_GeV, 23);

	fill_from_until(phyiscal_constants, 24, 74);

	fill_from(X_displacement_of_inclined_observation_plane, 74);
	fill_from(Y_displacement_of_inclined_observation_plane, 75);
	fill_from(Z_displacement_of_inclined_observation_plane, 76);

    fill_from(theta_angle_of_normal_vector_of_inclined_observation_plane, 77);
    fill_from(phi_angle_of_normal_vector_of_inclined_observation_plane, 78);

    // now some constants, I don't understand
    fill_from_until(CKA, 94, 134);
   	fill_from_until(CETA, 134, 139);
    fill_from_until(CSTRBA, 139, 150);
    fill_from(scatter_range_in_x_direction_for_Cherenkov, 247);
    fill_from(scatter_range_in_y_direction_for_Cherenkov, 248);
    fill_from_until(HLAY, 249, 254);
    fill_from_until(AATM, 254, 259);
    fill_from_until(BATM, 259, 264);
    fill_from_until(CATM, 264, 269);
    fill_from(NFLAIN, 269);
    fill_from(NFLDIF, 270);
    fill_from(NFLPI0_plus_100_times_NFLPIF, 271);
    fill_from(NFLCHE_plus_100_times_NFRAGM, 272);
}
//------------------------------------------------------------------------------
void MmcsCorsikaRunHeader::assert_sub_block_has_Run_Header()const {

	if(!mmcs_sub_block.is_run_header()) {
		std::stringstream info;
		info << "MmcsCorsikaRunHeader:\n";
		info << "A sub_block of MMCS file has no valid Run Header.\n";
		info << "Expected first word of block to be: 'RUNH', but actual: '";
		info << MmcsCorsikaTools::float2str_4byte_bin_map(mmcs_sub_block[0]);
		info << "'\n";
		throw std::runtime_error(info.str());		
	}
}
//------------------------------------------------------------------------------
void MmcsCorsikaRunHeader::fill_observation_levels() {

	number_of_observation_levels = (unsigned int)(std::round(mmcs_sub_block[4]));
	assert_number_of_observation_levels_is_valid();

	for(unsigned int obs_lvl=0; obs_lvl<number_of_observation_levels; obs_lvl++) {
		observation_levels.push_back(mmcs_sub_block[5+obs_lvl]);		
	}	
}
//------------------------------------------------------------------------------
void MmcsCorsikaRunHeader::assert_number_of_observation_levels_is_valid()const {

	if(number_of_observation_levels < 1 || number_of_observation_levels > 10) {
		std::stringstream info;
		info << "MmcsCorsikaRunHeader:\n";
		info << "Expected  0 < number_of_observation_levels < 10, ";
		info << "but actual: number_of_observation_levels = ";
		info << number_of_observation_levels << "\n";
		throw std::runtime_error(info.str());		
	}
}
//------------------------------------------------------------------------------
void MmcsCorsikaRunHeader::print()const {
	std::cout << str();
}
//------------------------------------------------------------------------------
std::string MmcsCorsikaRunHeader::str()const {
	std::stringstream out;
	out << "MMCS_Run_Header_________________________________________________\n";
	out << "| run_number " << run_number << "\n";
	out << "| date_of_begin_run " << date_of_begin_run << "\n";
	out << "| version_of_program " << version_of_program << "\n";
	out << "| number_of_observation_levels " << number_of_observation_levels << "\n";
	out << "| observation_levels " << MmcsCorsikaTools::float_vec_2_str_using_delimiter(observation_levels," ") << "\n";
	out << "| slope_of_energy_spektrum " << slope_of_energy_spektrum << "\n";
	out << "| energy_range " << MmcsCorsikaTools::float_vec_2_str_using_delimiter(energy_range," ") << "GeV\n";
	out << "| flag_for_EGS4_treatment_of_em_component " << flag_for_EGS4_treatment_of_em_component << "\n";
	out << "| flag_for_NKG_treatment_of_em_component " << flag_for_NKG_treatment_of_em_component << "\n";
	out << "| kin_energy_cutoff_for_hadrons_in_GeV " << kin_energy_cutoff_for_hadrons_in_GeV << "\n";
	out << "| kin_energy_cutoff_for_muons_in_GeV " << kin_energy_cutoff_for_muons_in_GeV << "\n";
	out << "| kin_energy_cutoff_for_electrons_in_GeV " << kin_energy_cutoff_for_electrons_in_GeV << "\n";
	out << "| energy_cutoff_for_photons_in_GeV " << energy_cutoff_for_photons_in_GeV << "\n";
	out << "| phyiscal_constants " << MmcsCorsikaTools::float_vec_2_str_using_delimiter(phyiscal_constants," ") << "\n";
	out << "| X_displacement_of_inclined_observation_plane " << X_displacement_of_inclined_observation_plane << "\n";
	out << "| Y_displacement_of_inclined_observation_plane " << Y_displacement_of_inclined_observation_plane << "\n";
	out << "| Z_displacement_of_inclined_observation_plane " << Z_displacement_of_inclined_observation_plane << "\n";
	out << "| theta_angle_of_normal_vector_of_inclined_observation_plane " << theta_angle_of_normal_vector_of_inclined_observation_plane << "\n";
	out << "| phi_angle_of_normal_vector_of_inclined_observation_plane " << phi_angle_of_normal_vector_of_inclined_observation_plane << "\n";
	out << "| CKA " << MmcsCorsikaTools::float_vec_2_str_using_delimiter(CKA," ") << "\n";
	out << "| CETA " << MmcsCorsikaTools::float_vec_2_str_using_delimiter(CETA," ") << "\n";
	out << "| CSTRBA " << MmcsCorsikaTools::float_vec_2_str_using_delimiter(CSTRBA," ") << "\n";	
	out << "| scatter_range_in_x_direction_for_Cherenkov " << run_number << "\n";
	out << "| scatter_range_in_y_direction_for_Cherenkov " << run_number << "\n";
	out << "| HLAY " << MmcsCorsikaTools::float_vec_2_str_using_delimiter(HLAY," ")<< "\n";
	out << "| AATM " << MmcsCorsikaTools::float_vec_2_str_using_delimiter(AATM," ") << "\n";
	out << "| BATM " << MmcsCorsikaTools::float_vec_2_str_using_delimiter(BATM," ") << "\n";
	out << "| CATM " << MmcsCorsikaTools::float_vec_2_str_using_delimiter(CATM," ") << "\n";
	out << "| NFLAIN " << NFLAIN << "\n";
	out << "| NFLDIF " << NFLDIF << "\n";
	out << "| NFLPI0_plus_100_times_NFLPIF " << NFLPI0_plus_100_times_NFLPIF << "\n";
	out << "| NFLCHE_plus_100_times_NFRAGM " << NFLCHE_plus_100_times_NFRAGM << "\n";
	out << "|_______________________________________________________________\n";
 	return out.str();	
}
//------------------------------------------------------------------------------
void MmcsCorsikaRunHeader::fill_from_until(
	std::vector<float> &vec, const unsigned int begin, const unsigned int end
) {
	for(unsigned int j=begin; j<end; j++)
		vec.push_back( mmcs_sub_block[j] );		
}
//------------------------------------------------------------------------------
void MmcsCorsikaRunHeader::fill_from(float &num_to_fill, const unsigned int at) {
	num_to_fill = mmcs_sub_block[at];	
}