#include "ConstantFunction.h"
#include <sstream>
#include "Core/TracerException.h"

namespace Function {
	const Constant Constant::void_function = Constant(0.0, Limits::void_limits);
	//--------------------------------------------------------------------------
	Constant::Constant(): value(0.0) {}
	//--------------------------------------------------------------------------
	Constant::Constant(const double value, const Limits &limits)
	{
		init(value, limits);
	}
	//--------------------------------------------------------------------------
	void Constant::init(const double value, const Limits &limits) {
		this->value = value;
		this->limits = limits;
	}
	//--------------------------------------------------------------------------
	double Constant::operator()(const double x)const {
		limits.assert_contains(x);
		return value;
	}
	//--------------------------------------------------------------------------
} // namespace Function