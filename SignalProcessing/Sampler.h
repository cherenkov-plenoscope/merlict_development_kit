//=================================
// include guard
#ifndef __PhotoElectricSampler_H_INCLUDED__
#define __PhotoElectricSampler_H_INCLUDED__

//=================================
// forward declared dependencies

//=================================
// included dependencies
#include <iostream>
#include "Core/Function/Function.h"
#include "Core/Random/Random.h"
#include "PipelinePhoton.h"
//#include <random>
//=================================

namespace PhotoElectricConverter {

	struct SamplerConfig {
		
		double exposure_time;
		double sampling_frequency;
		double std_dev_noise;
		const Function::Func1D* pulse_template;
		SamplerConfig();
	};

	static const Function::Constant box_pulse_5ns(
		1.0, 
		Function::Limits(0.0e-9, 5.0e-9)
	);

	class Sampler {

		uint number_of_slices;
		const SamplerConfig* config;
	public:

		Sampler(const SamplerConfig* config);
		vector<double> time_line(
			const vector<double> *electric_pipeline,
			const Function::DiscretSampling::LookUpTable* puls_look_up_table,
			Random::Generator* prng
		);

		uint get_number_of_slices()const;
	};

}// PhotoElectricConverter
#endif // __PhotoElectricSampler_H_INCLUDED__ 