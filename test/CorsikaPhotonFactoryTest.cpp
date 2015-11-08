#include "gtest/gtest.h"
#include "MmcsCorsikaFileIO/CorsikaPhotonFactory.h"

class CorsikaPhotonFactoryTest : public ::testing::Test {};
//------------------------------------------------------------------------------
TEST_F(CorsikaPhotonFactoryTest, intersection_point_on_ground) {

    // compare the input ground intersection point with the actual intersection
    // point of the mctracer photons when they are absorbed on the ground.

    for(float x=-1e4; x<1e4; x=x+1495.0) {
        for(float y=-1e4; y<1e4; y=y+1495.0) {
            for(float cx=-0.5; cx<0.5; cx=cx+0.11) {
                for(float cy=-0.5; cy<0.5; cy=cy+0.11) {

                    const std::vector<float> corsika_photon = 
                    {    x,   y,   cx,  cy,  0.0,  1e5, 1.0,   433};
                    //   x    y    xcos ycos time  zem  weight lambda
                    //   cm   cm   1    1    ns    cm   1      nm   
                    const uint id = 1337;
                    Random::FakeConstant prng(0.0);

                    CorsikaPhotonFactory cpf(corsika_photon, id, &prng);

                    vector<Photon*> photons;
                    photons.push_back(cpf.get_photon());

                    // propagate mctracer photons down to ground
                    Frame world("world", Vector3D::null, Rotation3D::null);

                    Disc ground;
                    ground.set_name_pos_rot("ground", Vector3D::null, Rotation3D::null);
                    const Color* ground_color = &Color::gray;
                    const uint ground_sensor_id = 0;
                    ground.set_outer_color(ground_color);
                    ground.set_inner_color(ground_color);
                    ground.set_disc_radius(1e3);

                    PhotonSensor::PerfectSensor sensor(ground_sensor_id, &ground);
                    std::vector<PhotonSensor::Sensor*> sensor_list = {&sensor};

                    world.set_mother_and_child(&ground);
                    world.init_tree_based_on_mother_child_relations();

                    // propagation settings
                    TracerSettings settings;    
                    settings.SetMultiThread(false);
                    settings.store_only_last_intersection(false);

                    // photon propagation down to the ground
                    Photons::propagate_photons_in_world_with_settings(
                        &photons, &world, &settings
                    );

                    // detect photons in ground sensor
                    PhotonSensors::reset_all_sesnors(&sensor_list);
                    PhotonSensors::assign_photons_to_sensors(&photons, &sensor_list);

                    std::vector<std::vector<double>> xyt = sensor.get_arrival_table();

                    ASSERT_EQ(1.0, xyt.size());

                    EXPECT_NEAR(x*1e-2, xyt.at(0).at(0), 1e-6);
                    EXPECT_NEAR(y*1e-2, xyt.at(0).at(1), 1e-6);            
                }
            }
        }
    }
}
//------------------------------------------------------------------------------
TEST_F(CorsikaPhotonFactoryTest, convert_photons) {
      
    const std::vector<float> corsika_photon = 
    	{1.2, 3.4, 0.0, 0.0, 1e-9, 1e5, 0.999, 433};
    //   x    y    xcos ycos time  zem  weight lambda
    //   cm   cm   1    1    ns    cm   1    nm
    
    const uint id = 1337;
    Random::FakeConstant prng(0.0);

    CorsikaPhotonFactory cpf(corsika_photon, id, &prng);
    
    Photon* ph;

    ASSERT_TRUE(cpf.passed_atmosphere());
    	
    ph = cpf.get_photon();

    EXPECT_EQ(id, ph->get_id());
    EXPECT_NEAR(433e-9, ph->get_wavelength(), 1e-9);

    // since the x,y angles are zero, the support vector can be tested
    EXPECT_NEAR(0.012, ph->Support().x(), 1e-9);
    EXPECT_NEAR(0.034, ph->Support().y(), 1e-9);
    EXPECT_NEAR(1e3, ph->Support().z(), 1e-9);
    ph->delete_history();
}
//------------------------------------------------------------------------------
TEST_F(CorsikaPhotonFactoryTest, execute_atmospheric_absorption) {
     
	// In Corsika photons can have a 'weight' to represent a bunch of photons.
	// This was introduced in Corsika to save computations and disk space.
	// Corsika does not recomend to have weights above 5.
	// Mctracer has no concet of weighted photon bunches. 
	// We force Corsika to produce single photons, i.e. the maximum weight
	// in the Corsika output is 1.0.
	// A weight below 1.0 means, that the photon passed Corsika's atmosphere
	// simulation with a probability of this weigth.
	// In mctracer we collapse this photons probability when the photon
	// is created in mctracer. This is why the CorsikaPhotonFactory is 
	// given a PseudoRandomNumberGenerator.

    double absorbed = 0;
    double passed = 0;
    double total = 1e5;

    Random::Mt19937 prng(Random::zero_seed);

    for(double i=0; i<total-1; i++) {

    	float weight = i/total; // from 0.0 to 1.0
	    const std::vector<float> corsika_photon = 
	    	{1.2, 3.4, 0.0, 0.0, 1e-9, 1e3, weight, 433};
	    //   x    y    xcos ycos time  zem  weight lambda
	    //   cm   cm   1    1    ns    cm   1    nm
	    
	    CorsikaPhotonFactory cpf(corsika_photon, 1337, &prng);

	    if(cpf.passed_atmosphere())
	    	passed++;
	    else
	    	absorbed++;
    }

    double passed_ratio = passed/total;
    EXPECT_NEAR(0.5, passed_ratio, 1e-2);
}
//------------------------------------------------------------------------------
TEST_F(CorsikaPhotonFactoryTest, mctracer_rejects_photon_weight_below_0) {
 
    const uint id = 1337;
    Random::Mt19937 prng(Random::zero_seed);
    const std::vector<float> corsika_photon = 
    	{1.2, 3.4, 0.0, 0.0, 1e-9, 1e5, -0.1, 433};
    
    EXPECT_THROW(
    	CorsikaPhotonFactory cpf(corsika_photon, id, &prng), 
    	CorsikaPhotonFactory::BadPhotonWeight
    ); 
}
//------------------------------------------------------------------------------
TEST_F(CorsikaPhotonFactoryTest, mctracer_accepts_photon_weight_equal_1) {
 
    const uint id = 1337;
    Random::Mt19937 prng(Random::zero_seed);
    const std::vector<float> corsika_photon = 
    	{1.2, 3.4, 0.0, 0.0, 1e-9, 1e5, 1.0, 433};
    
    EXPECT_NO_THROW(
    	CorsikaPhotonFactory cpf(corsika_photon, id, &prng)
    ); 
}
//------------------------------------------------------------------------------
TEST_F(CorsikaPhotonFactoryTest, mctracer_rejects_photon_weight_above_1) {
 
    const uint id = 1337;
    Random::Mt19937 prng(Random::zero_seed);
    const std::vector<float> corsika_photon = 
    	{1.2, 3.4, 0.0, 0.0, 1e-9, 1e5, 1.1, 433};
    
    EXPECT_THROW(
    	CorsikaPhotonFactory cpf(corsika_photon, id, &prng), 
    	CorsikaPhotonFactory::BadPhotonWeight
    ); 
}
//------------------------------------------------------------------------------
TEST_F(CorsikaPhotonFactoryTest, mctracer_accepts_photon_weight_equal_0) {
 
    const uint id = 1337;
    Random::Mt19937 prng(Random::zero_seed);
    const std::vector<float> corsika_photon = 
    	{1.2, 3.4, 0.0, 0.0, 1e-9, 1e5, 0.0, 433};
    
    EXPECT_NO_THROW(
    	CorsikaPhotonFactory cpf(corsika_photon, id, &prng)
    ); 
}
//------------------------------------------------------------------------------
TEST_F(CorsikaPhotonFactoryTest, mctracer_accepts_photon_weight_btw_0_and_1) {
 
    const uint id = 1337;
    Random::Mt19937 prng(Random::zero_seed);
    const std::vector<float> corsika_photon = 
    	{1.2, 3.4, 0.0, 0.0, 1e-9, 1e5, 0.4455, 433};
    
    EXPECT_NO_THROW(
    	CorsikaPhotonFactory cpf(corsika_photon, id, &prng)
    ); 
}
//------------------------------------------------------------------------------
TEST_F(CorsikaPhotonFactoryTest, zero_weight_is_passed_on_zero_from_prng) {
 
    const std::vector<float> corsika_photon = 
    {1.2, 3.4, 0.0, 0.0, 1e-9, 1e5, 0.0, 433};
    
    const uint id = 1337;
    Random::FakeConstant prng(0.0);

   	CorsikaPhotonFactory cpf(corsika_photon, id, &prng);

   	EXPECT_TRUE(cpf.passed_atmosphere());
}
//------------------------------------------------------------------------------
TEST_F(CorsikaPhotonFactoryTest, bad_shape) {
 
    const std::vector<float> corsika_photon = {1.2, 3.4, 0.0};
    
    const uint id = 1337;
    Random::Mt19937 prng(Random::zero_seed);

    EXPECT_THROW(
    	CorsikaPhotonFactory cpf(corsika_photon, id, &prng), 
    	CorsikaPhotonFactory::BadCorsikaPhotonShape
    );   
}
//------------------------------------------------------------------------------
TEST_F(CorsikaPhotonFactoryTest, relative_arrival_time_on_ground) {
    
    float arrival_time_on_dround_in_ns = 1.0;
    const std::vector<float> corsika_photon = 
    {1.2, 3.4, 0.0, 0.0, arrival_time_on_dround_in_ns, 1e5, 0.4455, 433};
    
    const uint id = 1337;
    Random::FakeConstant prng(0.0);
    
    CorsikaPhotonFactory cpf(corsika_photon, id, &prng);

    EXPECT_EQ(
        arrival_time_on_dround_in_ns*1e-9, 
        cpf.relative_arrival_time_on_ground()
    );
}
#include <numeric>
//------------------------------------------------------------------------------
TEST_F(CorsikaPhotonFactoryTest, correct_relative_time_when_intersecting_ground) {
   
    EventIo::EventIoFile corsika_file("telescope.dat");

    while (corsika_file.has_still_events_left()) {

        /*
        read in a corsika eventio event. we remember the relative arrival
        times in the eventio file 'relative_arrival_times_in_corsika_file' and
        compare these to the actual arrival times of the mctracer 
        photons on ground.
        */
        EventIo::Event event = corsika_file.next_event();
        
        vector<float> relative_arrival_times_in_corsika_file;

        vector<Photon*> photons;

        Random::Mt19937 prng(Random::zero_seed);

        for (uint id=0; id<event.photons.size(); id++) {

            CorsikaPhotonFactory factory(
                event.photons.at(id),
                id,
                &prng
            );
            
            relative_arrival_times_in_corsika_file.push_back(
                factory.relative_arrival_time_on_ground()
            );

            photons.push_back(factory.get_photon());
        }

        // propagate mctracer photons down to ground
        Frame world("world", Vector3D::null, Rotation3D::null);
        
        Disc ground;
        ground.set_name_pos_rot("ground", Vector3D::null, Rotation3D::null);
        const Color* ground_color = &Color::gray;
        const uint ground_sensor_id = 0;
        ground.set_outer_color(ground_color);
        ground.set_inner_color(ground_color);
        ground.set_disc_radius(1e7);

        PhotonSensor::PerfectSensor sensor(ground_sensor_id, &ground);
        std::vector<PhotonSensor::Sensor*> sensor_list = {&sensor};

        world.set_mother_and_child(&ground);
        world.init_tree_based_on_mother_child_relations();

        // propagation settings
        TracerSettings settings;    
        settings.SetMultiThread(false);
        settings.store_only_last_intersection(false);

        // photon propagation
        Photons::propagate_photons_in_world_with_settings(
            &photons, &world, &settings
        );

        // detect photons in sensors
        PhotonSensors::reset_all_sesnors(&sensor_list);
        PhotonSensors::assign_photons_to_sensors(&photons, &sensor_list);

        std::vector<std::vector<double>> id_time = sensor.get_arrival_table();
        double mean_arrival_time = get_mean_along_column(id_time,4);

        for(uint row=0; row<id_time.size(); row++)
            id_time.at(row).at(4) = id_time.at(row).at(4) - mean_arrival_time;

        double mean_time_of_corsika_photons = std::accumulate(
            relative_arrival_times_in_corsika_file.begin(),
            relative_arrival_times_in_corsika_file.end(),
            0.0
        )/double(relative_arrival_times_in_corsika_file.size());

        for(uint row=0; row<id_time.size(); row++)
            id_time.at(row).at(4) = id_time.at(row).at(4) + mean_time_of_corsika_photons;

        // for each photon we compare the relative arrival
        // time written in the in the eventio file with the actual arrival time
        // of the mctracer photon which ran down to the ground.  
        for(uint i=0;i<id_time.size();i++) {

            uint id = id_time.at(i).at(5);

            EXPECT_NEAR(
                relative_arrival_times_in_corsika_file.at(id),
                id_time.at(i).at(4),
                1e-11
            );
        }

        /*
        // relative_arrival_times_in_corsika_file
        std::cout << "corsika times: " << relative_arrival_times_in_corsika_file.size() << ", avg " << mean_time_of_corsika_photons << "s \n";
        for(int i=0;i<10;i++) {
            std::cout << "id " << i << ", " << relative_arrival_times_in_corsika_file.at(i) << "\n";
        }
        std::cout << "mctracer times: " << id_time.size() << ", avg " << get_mean_along_column(id_time,4) << "s\n";
        for(int i=0;i<10;i++) {
            std::cout << "id " << id_time.at(i).at(5) << ", " << float(id_time.at(i).at(4)) << "\n";
        }*/
    }
}
//------------------------------------------------------------------------------