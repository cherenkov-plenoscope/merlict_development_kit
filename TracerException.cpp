#include "TracerException.h"
//------------------------------------------------------------------------------
const std::string TracerException::compile_time = 
	TracerException::get_compile_time();
//------------------------------------------------------------------------------
TracerException::TracerException(){
	message = "Empty";
}
//------------------------------------------------------------------------------
TracerException::TracerException(std::string message){
	this->message = message;
}
//------------------------------------------------------------------------------
TracerException::TracerException(std::string message, ExceptionType Type){
	this->message = message;
	this->Type = Type;
}
//------------------------------------------------------------------------------
ExceptionType TracerException::type()const{
	return Type;
}
//------------------------------------------------------------------------------
const char* TracerException::what()const noexcept {
	return str2chararray(get_full_message_print());
}
//------------------------------------------------------------------------------
std::string TracerException::get_full_message_print()const {
	std::stringstream out;
	out << " ___TracerException___\n";
	out << "| compiled on: " << compile_time << "\n";
	out << 	StringTools::place_first_infront_of_each_new_line_of_second(
		   		"| ",
				message
			);
	out << "|_____________________\n";
	return out.str();
}
//------------------------------------------------------------------------------
std::string TracerException::get_compile_time() {
	std::stringstream timestamp;
	timestamp << __DATE__ << ", " << __TIME__;
	return timestamp.str();
}
//------------------------------------------------------------------------------
const char* TracerException::str2chararray(const std::string text)const {
	char* charrout = new char[text.length() + 1];
	std::strcpy(charrout, text.c_str());
	return charrout;
}