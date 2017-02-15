#include "gtest/gtest.h"
#include "SignalProcessing/PhotoElectricConverter.h"
#include "Core/Histogram1D.h"

class PhotoElectricConverterTest : public ::testing::Test {};

vector<SignalProcessing::PipelinePhoton> equi_distant_photons(const uint n) {
    vector<SignalProcessing::PipelinePhoton> photon_pipeline;
    for(uint i=0; i<n; i++) {
        const double arrival_time = double(i)*1e-9;
        const double wavelength = 433e-9;
        const int simulation_truth_id = i;

        photon_pipeline.push_back(
            SignalProcessing::PipelinePhoton(
                arrival_time,
                wavelength,
                simulation_truth_id));
    }
    return photon_pipeline;
}

//----------------------------------------------------------------------
TEST_F(PhotoElectricConverterTest, config_defaults) {

    SignalProcessing::PhotoElectricConverter::Config config;
    EXPECT_EQ(0.0, config.dark_rate);
    EXPECT_EQ(0.0, config.probability_for_second_puls);

    for(double wavelength=200e-9; wavelength<1200e-9; wavelength+=10e-9)
        EXPECT_EQ(0.0, (*config.quantum_efficiency_vs_wavelength)(wavelength));

    EXPECT_THROW(
        (*config.quantum_efficiency_vs_wavelength)(1200e-9), 
        Function::Limits::OutOfRange);
    EXPECT_THROW(
        (*config.quantum_efficiency_vs_wavelength)(199e-9), 
        Function::Limits::OutOfRange);
}
//----------------------------------------------------------------------
TEST_F(PhotoElectricConverterTest, empty_input_yields_empty_output) {

    SignalProcessing::PhotoElectricConverter::Config config;
    SignalProcessing::PhotoElectricConverter::Converter conv(&config);

    vector<SignalProcessing::PipelinePhoton> photon_pipeline;
    double exposure_time = 1.0;
    Random::Mt19937 prng(0);
    vector<SignalProcessing::ElectricPulse> result = conv.
        get_pulse_pipeline_for_photon_pipeline(
            photon_pipeline,
            exposure_time,
            &prng
        );

    EXPECT_EQ(0u, result.size());
}
//----------------------------------------------------------------------
TEST_F(PhotoElectricConverterTest, input_pulses_absorbed_zero_qunatum_eff) {

    SignalProcessing::PhotoElectricConverter::Config config;
    SignalProcessing::PhotoElectricConverter::Converter conv(&config);

    vector<SignalProcessing::PipelinePhoton> photon_pipeline = 
        equi_distant_photons(1337u);

    double exposure_time = 1337e-9;
    Random::Mt19937 prng(0);
    vector<SignalProcessing::ElectricPulse> result = conv.
        get_pulse_pipeline_for_photon_pipeline(
            photon_pipeline,
            exposure_time,
            &prng
        );

    EXPECT_EQ(0u, result.size());
}
//----------------------------------------------------------------------
TEST_F(PhotoElectricConverterTest, input_pulses_pass_qunatum_eff_is_one) {

    SignalProcessing::PhotoElectricConverter::Config config;
    Function::Constant qeff(1.0, Function::Limits(200e-9, 1200e-9));
    config.quantum_efficiency_vs_wavelength = &qeff;
    SignalProcessing::PhotoElectricConverter::Converter conv(&config);

    vector<SignalProcessing::PipelinePhoton> photon_pipeline = 
        equi_distant_photons(1337u);

    double exposure_time = 1337e-9;
    Random::Mt19937 prng(0);
    vector<SignalProcessing::ElectricPulse> result = conv.
        get_pulse_pipeline_for_photon_pipeline(
            photon_pipeline,
            exposure_time,
            &prng
        );

    ASSERT_EQ(1337u, result.size());
    for(uint i=0; i<result.size(); i++) {
        EXPECT_EQ(
            photon_pipeline.at(i).arrival_time, 
            result.at(i).arrival_time);
    }
}
//----------------------------------------------------------------------
TEST_F(PhotoElectricConverterTest, dark_rate_on_empty_photon_pipe) {

    SignalProcessing::PhotoElectricConverter::Config config;
    config.dark_rate = 100e6;
    SignalProcessing::PhotoElectricConverter::Converter conv(&config);

    vector<SignalProcessing::PipelinePhoton> photon_pipeline;

    double exposure_time = 1e-6;
    Random::Mt19937 prng(0);
    vector<SignalProcessing::ElectricPulse> result = conv.
        get_pulse_pipeline_for_photon_pipeline(
            photon_pipeline,
            exposure_time,
            &prng
        );

    EXPECT_NEAR(100u, result.size(), 3);
}
//----------------------------------------------------------------------
TEST_F(PhotoElectricConverterTest, triangle_qeff) {

                                                            
    // Qeff [1]                                            //    
    //                                                     // 
    // _1.0_|                                              //  
    //      |            /\                                //                
    //      |           /  \                               //                 
    //      |          /    \                              //                  
    //      |         /      \                             //                   
    //      |        /        \                            //                    
    //      |       /          \                           //                     
    //      |      /            \                          //                      
    //      |     /              \                         //                       
    //      |    /                \                        //                        
    //      |   /                  \                       //
    //      |  /                    \                      //
    // _0.0_|--|----------|----------|----                 //
    //        200e-9    700e-9     1200e-9 [nm] wavelength //

    SignalProcessing::PhotoElectricConverter::Config config;
    vector<vector<double>> raw_qeff {{  200e-9, 0.0},
                                     {  700e-9, 1.0},
                                     { 1200e-9, 0.0}};
    Function::LinInterpol qeff(raw_qeff);
    config.quantum_efficiency_vs_wavelength = &qeff;
    SignalProcessing::PhotoElectricConverter::Converter conv(&config);

    vector<SignalProcessing::PipelinePhoton> photon_pipeline;
    const uint n = 1000*1000;
    for(uint i=0; i<n; i++) {
        const double arrival_time = double(i);
        const double wavelength = 200e-9 + double(i)/double(n)*1000e-9;
        const int simulation_truth_id = i;
        photon_pipeline.push_back(
            SignalProcessing::PipelinePhoton(
                arrival_time,
                wavelength,
                simulation_truth_id));
    }

    double exposure_time = 1.0;
    Random::Mt19937 prng(0);
    vector<SignalProcessing::ElectricPulse> result = conv.
        get_pulse_pipeline_for_photon_pipeline(
            photon_pipeline,
            exposure_time,
            &prng
        );

    EXPECT_NEAR(n/2, result.size(), double(n));

    vector<double> survived_arrival_times; 
    // We created a correlation between wavelength and arrival 
    // time in this case on purpose.
    for(const SignalProcessing::ElectricPulse pulse: result)
        survived_arrival_times.push_back(pulse.arrival_time);

    Histogram1D hist(
        survived_arrival_times, 
        {   
            0,
            100*1000,
            200*1000,
            300*1000,
            400*1000,
            500*1000,
            600*1000,
            700*1000,
            800*1000,
            900*1000,
            1000*1000
        }
    );

    EXPECT_NEAR(10*1000u, hist.bins.at(0), 58);
    EXPECT_NEAR(30*1000u, hist.bins.at(1), 253);
    EXPECT_NEAR(50*1000u, hist.bins.at(2), 119);
    EXPECT_NEAR(70*1000u, hist.bins.at(3), 39);
    EXPECT_NEAR(90*1000u, hist.bins.at(4), 54);
    EXPECT_NEAR(90*1000u, hist.bins.at(5), 228);
    EXPECT_NEAR(70*1000u, hist.bins.at(6), 71);
    EXPECT_NEAR(50*1000u, hist.bins.at(7), 194);
    EXPECT_NEAR(30*1000u, hist.bins.at(8), 37);
    EXPECT_NEAR(10*1000u, hist.bins.at(9), 25);
}