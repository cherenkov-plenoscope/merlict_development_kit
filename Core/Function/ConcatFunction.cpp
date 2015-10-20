#include "ConcatFunction.h"
#include <sstream>
#include "TracerException.h"
#include <algorithm>    // std::lower_bound, std::upper_bound, std::sort

namespace Function {

	Concat::Concat(const std::vector<Func1D*> _conc) {
		conc = _conc;
		assert_limits_do_fit();
		adopt_new_limits();
	}
	//--------------------------------------------------------------------------
	void Concat::assert_limits_do_fit()const {

		for(uint i=0; i<conc.size()-1; i++) {
			if(	conc.at(i)->get_limits().get_upper() != 
				conc.at(i+1)->get_limits().get_lower()
			){
				std::stringstream info;
				info << "Concat of Function 1D in file " << __FILE__ << "\n";
				info << "Expected limits to fit. ";
				info << "function[" << i << "].upper_limit == ";
				info << "function[" << i+1 << "].lower_limit, but actual\n";
				info << "f[" << i << "].upper_limit = ";
				info << conc.at(i)->get_limits().get_upper() << "\n";
				info << "f[" << i+1 << "].lower_limit = ";
				info << conc.at(i+1)->get_limits().get_lower() << "\n";
				throw TracerException(info.str());			
			}
		}
	}
	//--------------------------------------------------------------------------
	void Concat::adopt_new_limits() {
		limits = Limits(
			conc.front()->get_limits().get_lower(),
			conc.back()->get_limits().get_upper()
		);
	}
	//--------------------------------------------------------------------------
	double Concat::operator()(const double x)const {
		limits.assert_contains(x);
		std::vector<Func1D*>::const_iterator sub_func = get_sub_function_for(x);
		return (*(*(sub_func)))(x);
	}
	//--------------------------------------------------------------------------
	std::vector<Func1D*>::const_iterator Concat::get_sub_function_for(
		const double x
	)const {
		return std::upper_bound(
			conc.begin(),
			conc.end(), 
			x, 
			compare_upper_limit
		);
	}
	//--------------------------------------------------------------------------
	bool Concat::compare_upper_limit(const double x, const Func1D *f) {
		return f->get_limits().get_upper() > x;
	}
} // namespace Function