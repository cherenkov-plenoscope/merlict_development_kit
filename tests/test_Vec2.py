import numpy as np
from math import sqrt
import pytest

import merlict as ml
from merlict import Vec2


def test_NullVector():
    a = ml.VEC2_ORIGIN
    assert a.x == 0.
    assert a.y == 0.
    assert a.norm() == 0.


def test_UnitVectors():
    a = Vec2()
    a = ml.VEC2_UNIT_X
    assert a.x == 1.
    assert a.y == 0.
    assert a.norm() == 1.
    a = ml.VEC2_UNIT_Y
    assert a.x == 0.
    assert a.y == 1.
    assert a.norm() == 1.


def test_ConstructorAndGetter():
    for x in np.arange(-2, 2, .25):
        for y in np.arange(-2, 2, .25):
            v = Vec2(x, y)
            assert v.x == x
            assert v.y == y


def test_SetterAndGetter():
    for x in np.arange(-2, 2, .25):
        for y in np.arange(-2, 2, .25):
            v = Vec2()
            v.set(x, y)
            assert v.x == x
            assert v.y == y


def test_EuclideanNorm():
    for x in np.arange(-2, 2, .25):
        for y in np.arange(-2, 2, .25):
            v = Vec2(x, y)
            assert v.norm() == sqrt(x * x + y * y)


def test_Scalar_Product_unit_vectors():
    x = ml.VEC2_UNIT_X
    y = ml.VEC2_UNIT_Y
    assert x * y == 0.
    assert x * x == 1.


def test_Scalar_Product():
    for x1 in np.arange(-2, 2, .25):
        for y1 in np.arange(-2, 2, .25):
            for x2 in np.arange(-2, 2, .25):
                for y2 in np.arange(-2, 2, .25):
                    v1 = Vec2(x1, y1)
                    v2 = Vec2(x2, y2)
                    assert v1 * v2 == x1 * x2 + y1 * y2


def test_ScalarMultiplication_and_Division():
    for x in np.arange(-2, 2, .25):
        for y in np.arange(-2, 2, .25):
            a = Vec2(x, y)
            for f in np.arange(-2, 2, .25):
                assert (a * f).x == x * f
                assert (a * f).y == y * f


# the test below fails at x or y == 0
# E  AssertionError: assert nan == (0.0 / 0.0)
# E  +  where nan = (<merlict.Vec2; proxy of <Swig Object of type 'merlict::Vec2 *' at 0x7f60955bf2a0> > / 0.0).y
@pytest.mark.xfail
def test_ScalarDivision():
    for x in np.arange(-2, 2, .25):
        for y in np.arange(-2, 2, .25):
            a = Vec2(x, y)
            for f in np.arange(-2, 2, .25):
                assert (a / f).x == x / f
                assert (a / f).y == y / f


def test_distance_unit_x_to_unit_y():
    a = ml.VEC2_UNIT_X
    b = ml.VEC2_UNIT_Y
    assert a.distance_to(b) == sqrt(2.)


def test_distance_to_itself():
    a = Vec2(1.3, 3.7)
    assert a.distance_to(a) == 0.


def test_Operator_equals_expect_true():
    a = Vec2(1.3, 4.2)
    b = Vec2(1.3, 4.2 + 1e-20)
    assert b == a
    assert a == b
    assert not (a != b)


def test_Operator_equals_expect_false():
    a = Vec2(1.3, 4.2)
    b = Vec2(1.3, 4.2 + 2e-7)
    assert b != a
    assert a != b
    assert not (a == b)


def test_normalize():
    a = Vec2(1., 2.)
    assert a.norm() != 1.
    a.normalize()
    assert 1. == pytest.approx(a.norm(), abs=1e-9)

    a *= 2.
    assert a.norm() != 1.

    a.normalize()
    assert a.norm() == 1.

    a = ml.VEC2_ORIGIN
    assert a.norm() != 1.

    a.normalize()
    assert np.isnan(a.norm())


def test_angle_in_between():
    a = ml.VEC2_UNIT_X
    b = ml.VEC2_UNIT_X

    assert a.angle_in_between(b) == 0.
    assert a.angle_in_between(b) == b.angle_in_between(a)

    c = ml.VEC2_UNIT_X * 5.
    assert c.norm() != 1.

    d = ml.VEC2_UNIT_X * 5.
    assert d.norm() != 1.

    assert d.angle_in_between(c) == 0.
    assert d.angle_in_between(c) == c.angle_in_between(d)

    foo = ml.VEC2_UNIT_X * 5. + ml.VEC2_UNIT_Y * 5.
    assert c.norm() != 1.

    bar = ml.VEC2_UNIT_X * 5.
    assert d.norm() != 1.

    assert np.deg2rad(45.) == pytest.approx(
        foo.angle_in_between(bar), abs=1e-5
    )
