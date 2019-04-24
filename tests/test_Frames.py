import pytest
import merlict as ml
import math


def test_too_close_together_no_frame():
    vf = ml.VectorOfFramePointers()
    assert not ml.positions_in_mother_too_close(vf)


def test_too_close_together_one_frame():
    root = ml.Frame()
    root.set_name_pos_rot("root", ml.VEC3_ORIGIN, ml.ROT3_UNITY)
    vf = ml.VectorOfFramePointers()
    vf.push_back(root)

    assert not ml.positions_in_mother_too_close(vf)


def test_too_close_together_true():
    f1 = ml.Frame()
    f1.set_name_pos_rot("f1", ml.VEC3_ORIGIN, ml.ROT3_UNITY)

    f2 = ml.Frame()
    f2.set_name_pos_rot("f2", ml.VEC3_ORIGIN, ml.ROT3_UNITY)

    vf = ml.VectorOfFramePointers([f1, f2])
    assert ml.positions_in_mother_too_close(vf)


def test_too_close_together_false():
    f1 = ml.Frame()
    f1.set_name_pos_rot("f1", ml.VEC3_ORIGIN, ml.ROT3_UNITY)

    f2 = ml.Frame()
    f2.set_name_pos_rot(
        "f2",
        ml.Vec3(0, 0, 10.0 * ml.FRAME_MIN_STRUCTURE_SIZE),
        ml.ROT3_UNITY)

    vf = ml.VectorOfFramePointers([f1, f2])
    assert not ml.positions_in_mother_too_close(vf)


@pytest.mark.skip
def test_mean_no_frame():
    vf = ml.VectorOfFramePointers()
    # CHECK_THROWS_AS(ml.bound::position_mean_in_mother(vf), std::invalid_argument)

    # this call results in a core dump
    ml.position_mean_in_mother(vf)


@pytest.mark.skip
def test_optimal_bounding_sphere_pos_no_frame():
    vf = ml.VectorOfFramePointers()
    # CHECK_THROWS_AS(ml.bound::bounding_sphere_center_alternative(vf), std::invalid_argument)

    # this call results in a core dump
    ml.bounding_sphere_center_alternative(vf)


def test_optimal_bounding_sphere_pos_one_frame():
    root = ml.Frame()
    root.set_name_pos_rot("root", ml.VEC3_UNIT_X, ml.ROT3_UNITY)
    vf = ml.VectorOfFramePointers()
    vf.push_back(root)

    v = ml.bounding_sphere_center_alternative(vf)

    # ml.Vec3 equals operator does not work in python
    # assert ml.VEC3_UNIT_X == v
    assert  v.x == 1.
    assert  v.y == 0.
    assert  v.z == 0.


def test_optimal_bounding_sphere_pos_many_frames_symetric():
    vf = ml.VectorOfFramePointers()

    # Python helper list to avoid `f` being garbage collected
    avoid_garbage_collection_list = []
    for i in range(5):

        f = ml.Frame()
        f.set_name_pos_rot(
            "f{}".format(i),
            ml.Vec3(i, 0, 0),
            ml.ROT3_UNITY
        )
        avoid_garbage_collection_list.append(f)
        vf.push_back(f)

    v = ml.bounding_sphere_center_alternative(vf)
    # ml.Vec3 equals operator does not work in python
    # assert v == ml.Vec3(2, 0, 0)
    assert v.x == 2.
    assert v.y == 0.
    assert v.z == 0.


def test_optimal_bounding_sphere_pos_many_spheres_symetric():
    vf = ml.VectorOfFramePointers()
    avoid_garbage_collection_list = []
    for i in range(5):
        s = ml.Sphere()
        s.set_name_pos_rot(
            "s{}".format(i),
            ml.Vec3(i, 0, 0),
            ml.ROT3_UNITY
        )
        avoid_garbage_collection_list.append(s)
        vf.push_back(s)

    v = ml.bounding_sphere_center_alternative(vf)
    # ml.Vec3 equals operator does not work in python
    # assert v == ml.Vec3(2, 0, 0)
    assert v.x == 2.
    assert v.y == 0.
    assert v.z == 0.


def test_optimal_bounding_sphere_pos_many_spheres_asymetric():
    vf = ml.VectorOfFramePointers()
    avoid_garbage_collection_list = []
    radii = [1., 1., 1., 1., 5.]
    for i, radius in enumerate(radii):
        s = ml.Sphere()
        s.set_name_pos_rot(
            "s{}".format(i),
            ml.Vec3(i, 0, 0),
            ml.ROT3_UNITY
        )
        s.set_radius(radius)
        avoid_garbage_collection_list.append(s)
        vf.push_back(s)

    v = ml.bounding_sphere_center_alternative(vf)
    # ml.Vec3 equals operator does not work in python
    # assert v == ml.Vec3(4, 0, 0)
    assert v.x == 4.
    assert v.y == 0.
    assert v.z == 0.


def test_not_optimal_in_symetric_case():
    vf = ml.VectorOfFramePointers()

    angles = [0., 120., 240.]
    avoid_garbage_collection_list = []
    for i, angle in enumerate(angles):

        s = ml.Sphere()
        s.set_name_pos_rot(
            "s{}".format(i),
            ml.Vec3(
                math.cos(ml.deg2rad(angle)),
                math.sin(ml.deg2rad(angle)),
                0
            ),
            ml.ROT3_UNITY
        )
        s.set_radius(0.5)
        avoid_garbage_collection_list.append(s)
        vf.push_back(s)

    center = ml.bounding_sphere_center_alternative(vf)
    radius = ml.bounding_sphere_radius(vf, center)

    expected_enclosing_radius_for_three_balls = 1.5
    assert radius > expected_enclosing_radius_for_three_balls
