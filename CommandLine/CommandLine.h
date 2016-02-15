//=================================
// include guard
#ifndef __CommandLine_H_INCLUDED__
#define __CommandLine_H_INCLUDED__

//=================================
// forward declared dependencies

//=================================
// included dependencies
#include "cmdline/cmdline.h"
#include <string>
// A command line wrapper

using std::string;
//------------------------------------------------------------------------------
class CommandLine {
 	
	cmdline::parser cp;	
public:

	void add_value(const string name, const char short_name, const string desc);
	void add_flag(const string name, const string desc);
	void parse(int argc, char *argv[]);
	string get(const string key)const;
	bool exist(const string key)const;
};
#endif // __CommandLine_H_INCLUDED__ 