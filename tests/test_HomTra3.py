import pytest
from math import pi
import merlict as ml
from merlict import HomTra3, Vec3


def test_default_is_unit_trafo():
    trafo = HomTra3()
    unit_frafo = HomTra3()
    unit_frafo.set_transformation(
        # rotation
        Vec3(1, 0, 0),
        Vec3(0, 1, 0),
        Vec3(0, 0, 1),
        # position
        Vec3(0, 0, 0)
    )
    assert trafo == unit_frafo


def test_unit_trafo_has_no_translation():
    trafo = HomTra3()
    # translation should be zero
    assert trafo.translation() == ml.VEC3_ORIGIN


def test_unit_trafo_has_no_rotation():
    trafo = HomTra3()
    # and rotation should be unity as well
    unit_x = Vec3(1.0, 0.0, 0.0)
    assert trafo.orientation(unit_x) == unit_x
    unit_y = Vec3(0.0, 1.0, 0.0)
    assert trafo.orientation(unit_y) == unit_y
    unit_z = Vec3(0.0, 0.0, 1.0)
    assert trafo.orientation(unit_z) == unit_z


def test_unit_trafo_is_its_own_inverse():
    trafo = HomTra3()
    inverse_trafo = trafo
    inverse_trafo.inverse()
    assert trafo == inverse_trafo


def test_translation_without_rotation():
    translation = Vec3(0.0, 0.0, 133.7)
    rotation = ml.Rot3(0.0, 0.0, 0.0)
    frame1_to_frame2 = HomTra3()
    frame1_to_frame2.set_transformation(rotation, translation)
    my_vector_in_frame1 = Vec3(3.141, 4.2, 0.0)
    my_vector_in_frame2 = Vec3(3.141, 4.2, 133.7)
    assert (
        frame1_to_frame2.position(my_vector_in_frame1) == my_vector_in_frame2
    )
    # and also the other/inverse way round
    frame2_to_frame1 = frame1_to_frame2.inverse()
    assert (
        frame2_to_frame1.position(my_vector_in_frame2) == my_vector_in_frame1
    )


def test_trans_pos_forth_and_back_only_translation_component_set():
    t = Vec3(0.0, 0.0, 133.7)
    r = ml.Rot3(0.0, 0.0, 0.0)
    homtra = HomTra3()
    homtra.set_transformation(r, t)
    ux_original = ml.VEC3_UNIT_X
    ux_forth = homtra.position(ux_original)
    assert 1.0 == ux_forth.x
    assert 0.0 == ux_forth.y
    assert 133.7 == ux_forth.z
    ux_back = homtra.position_inverse(ux_forth)
    assert ux_original.x == ux_back.x
    assert ux_original.y == ux_back.y
    assert ux_original.z == ux_back.z


def test_transform_position_forth_and_back_full_set():
    t = Vec3(0.0, 0.0, 133.7)
    r = ml.Rot3(0.0, pi, 0.0)
    homtra = HomTra3()
    homtra.set_transformation(r, t)
    ux_original = ml.VEC3_UNIT_X
    ux_forth = homtra.position(ux_original)
    assert -1.0 == pytest.approx(ux_forth.x, abs=1e-12)
    assert 0.0 == pytest.approx(ux_forth.y, abs=1e-12)
    assert 133.7 == pytest.approx(ux_forth.z, abs=1e-12)
    ux_back = homtra.position_inverse(ux_forth)
    assert ux_original.x == ux_back.x
    assert ux_original.y == ux_back.y
    assert ux_original.z == ux_back.z


def test_trans_orientation_forth_and_back_only_rot_component_set():
    t = Vec3(0.0, 0.0, 0.0)
    r = ml.Rot3(0.0, pi, 0.0)
    homtra = HomTra3()
    homtra.set_transformation(r, t)
    ux_original = ml.VEC3_UNIT_X
    ux_forth = homtra.orientation(ux_original)
    assert -1.0 == pytest.approx(ux_forth.x, abs=1e-12)
    assert 0.0 == pytest.approx(ux_forth.y, abs=1e-12)
    assert 0.0 == pytest.approx(ux_forth.z, abs=1e-12)
    ux_back = homtra.orientation_inverse(ux_forth)
    assert ux_original.x == ux_back.x
    assert ux_original.y == ux_back.y
    assert ux_original.z == ux_back.z
