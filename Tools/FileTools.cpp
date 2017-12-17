#include "FileTools.h"
#include <sstream>
#include <fstream>
//------------------------------------------------------------------------------
void FileTools::write_text_to_file(
	const std::string &text,
	const std::string &path
) {
	std::ofstream text_file (path.c_str());
	if(text_file.is_open()) {
		text_file << text;
		text_file.close();
	}else{
		std::stringstream info;
		info << "FileTools: ";
		info << "Unable to write text to file: '" << path << "'.";
		throw std::runtime_error(info.str());	  	
	}	
}
//------------------------------------------------------------------------------