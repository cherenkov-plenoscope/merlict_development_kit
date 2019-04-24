import pytest
import numpy as np
import merlict as ml


def make_environment_with_lens_for_test():
    # env and sensor_list are being returned
    env = ml.PropagationEnvironment()
    sensor_list = ml.Sensors()

    prng = ml.Mt19937()
    prng.set_seed(0)   # this is the default and should be a ctor parameter
    env.prng = prng

    settings = ml.PropagationConfig()
    settings.max_num_interactions_per_photon = 5
    env.config = settings

    scenery = ml.Scenery()

    test_bench = scenery.root.add_frame()
    test_bench.set_name_pos_rot(
        "BiConvexLens_test_world",
        ml.VEC3_ORIGIN,
        ml.ROT3_UNITY
    )

    # this lens_maker_Config, is just a one-off class, used to
    # figure out numbers like, the radius of curvature from the focal length
    # it can be garbage collected after this function is done
    cfg = ml.lens_maker_Config()
    cfg.focal_length = 1.0
    cfg.aperture_radius = 0.125
    cfg.refractive_index = 1.49

    lens = test_bench.add_bi_convex_lens()
    lens.set_name_pos_rot(
        "little_lens",
        ml.VEC3_ORIGIN,
        ml.ROT3_UNITY)

    scenery.functions.add(
        "refraction_vs_wavelength",
        ml.Func1(ml.VectorOfVectorOfDoubles([
            [200e-9, cfg.refractive_index],
            [1200e-9, cfg.refractive_index],
        ]))
    )

    lens.inner_refraction = scenery.functions.get("refraction_vs_wavelength")
    lens.set_curvature_radius_and_aperture_radius(
        ml.get_curvature_radius(cfg),
        cfg.aperture_radius
    )

    image_sensor = test_bench.add_disc()
    image_sensor.set_name_pos_rot(
        "sensor_disc",
        ml.Vec3(0.0, 0.0, -1.0),
        ml.ROT3_UNITY)

    image_sensor.set_radius(cfg.aperture_radius)

    scenery.sensors.add(0, image_sensor)
    sensor_list.init(scenery.sensors.sensors)

    scenery.root.init_tree_based_on_mother_child_relations()

    # We hand over the scenery to the environment,
    # so we have to give up ownership, to avoid the garbage collector
    # from killing it.
    scenery.this.disown()
    env.root_frame = scenery.root

    return env, sensor_list


def test_send_photon_frontal_into_lens():
    total_propagations = int(1e4)
    num_photons_reaching_sensor_disc = 0

    env, _ = make_environment_with_lens_for_test()

    for _ in range(total_propagations):

        blue_photon = ml.Photon(
            ml.Vec3(0.0, 0.0, 1.0),
            ml.Vec3(0.0, 0.0, -1.0),
            433e-9)

        ml.Propagator(blue_photon, env)

        if np.isclose(blue_photon.accumulated_distance(), 2.0):
            num_photons_reaching_sensor_disc += 1

    assert (
        num_photons_reaching_sensor_disc / total_propagations ==
        pytest.approx(0.97, abs=5e-2)
    )


def test_send_photons_frontal_into_lens_with_offset():

    env, sensor_list = make_environment_with_lens_for_test()

    num_photons_emitted = int(1e4)
    prng = ml.Mt19937(0)

    photons = ml.parallel_towards_z_from_xy_disc(
        0.125,
        num_photons_emitted,
        prng
    )

    trafo = ml.HomTra3()
    trafo.set_transformation(
        ml.Rot3(0.0, -ml.deg2rad(180.0), 0.0),
        ml.Vec3(0.0, 0.0, 1.0)
    )

    for i in range(len(photons)):
        photons[i].transform(trafo)

    # Danger! this does not work, it does not transform the photons inside
    # the Vector of photons ... it seems to only operate of copies
    #
    # for photon in photons:
    #     photon.transform(trafo)

    ml.propagate_photons_in_frame_with_config(
        photons,
        env.root_frame,
        env.config,
        prng)

    sensor_list.clear_history()
    sensor_list.assign_photons(photons)

    assert len(sensor_list.by_frame) == 1

    first_sensor = sensor_list.at(0)

    assert (
        ml.point_spread_std_dev(first_sensor.photon_arrival_history) ==
        pytest.approx(1.5e-3, abs=1e-3)
    )

    assert (
        len(first_sensor.photon_arrival_history) / num_photons_emitted ==
        pytest.approx(1, abs=1e-1)
    )
