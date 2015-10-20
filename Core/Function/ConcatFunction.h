//==============================================================================
// include guard
#ifndef __FUNCTIONCONCATENATE_H_INCLUDED__
#define __FUNCTIONCONCATENATE_H_INCLUDED__

//==============================================================================
// forward declared dependencies

//==============================================================================
// included dependencies
#include "Func1DFunction.h"

namespace Function {

	class Concat : public Func1D {

		std::vector<Func1D*> conc;
	public:
		Concat(const std::vector<Func1D*> _conc);
		double operator()(const double x)const;
	private:
		void assert_limits_do_fit()const;
		void adopt_new_limits();
		std::vector<Func1D*>::const_iterator get_sub_function_for(
			const double x
		)const;
		static bool compare_upper_limit(const double x, const Func1D *f);
	};
} // namespace Function
#endif // __FUNCTIONCONCATENATE_H_INCLUDED__