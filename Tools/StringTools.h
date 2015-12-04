//=================================
// include guard
#ifndef __STRINGTOOLS_H_INCLUDE__
#define __STRINGTOOLS_H_INCLUDE__

//=================================
// forward declared dependencies

//=================================
// included dependencies 
#include <iostream>
#include <string>
#include <string.h>
#include <sstream>
#include <vector>
#include "TracerException.h"

namespace StringTools {
	bool is_equal(const std::string text_A,const std::string text_B);

	std::string cut_leading_token_infront_of_delimiter( 
		std::string &text_of_tokens, const char delimiter 
	);

	std::string repeat_multiple_times( 
		const std::string text_to_repeat, 
		const unsigned int times 
	);

	bool is_ending(std::string text, std::string ending);

	bool string_contains_char(const std::string &text, const char ch);

	std::string place_first_infront_of_each_new_line_of_second(
		const std::string front,
		std::string text
	);

	std::vector<std::string> tokenize_text_given_delimiter_char(
		const std::string &text, 
		const char delimiter
	);

	std::string strip_whitespaces(std::string text);

	std::string fill_up_text_with_whitespaces_until_column(
		const std::string &text, const uint column
	);
}

#endif // __STRINGTOOLS_H_INCLUDE__