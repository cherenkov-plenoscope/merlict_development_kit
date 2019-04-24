from math import sqrt
import pytest
import numpy as np
import merlict as ml
from merlict import Vec3


def test_NullVector():
    a = ml.VEC3_ORIGIN

    assert a.x == 0.
    assert a.y == 0.
    assert a.z == 0.
    assert a.norm() == 0.


def test_UnitVectors():
    a = ml.VEC3_UNIT_X
    assert a.x == 1.
    assert a.y == 0.
    assert a.z == 0.
    assert a.norm() == 1.

    a = ml.VEC3_UNIT_Y
    assert a.x == 0.
    assert a.y == 1.
    assert a.z == 0.
    assert a.norm() == 1.

    a = ml.VEC3_UNIT_Z
    assert a.x == 0.
    assert a.y == 0.
    assert a.z == 1.
    assert a.norm() == 1.


def test_ConstructorAndGetter():
    x = np.random.rand() - .5
    y = np.random.rand() - .5
    z = np.random.rand() - .5
    v = Vec3(x, y, z)
    assert v.x == x
    assert v.y == y
    assert v.z == z


def test_SetterAndGetter():
    x = np.random.rand() - .5
    y = np.random.rand() - .5
    z = np.random.rand() - .5
    v = Vec3()
    v.set(x, y, z)
    assert v.x == x
    assert v.y == y
    assert v.z == z


def test_EuclideanNorm():
    x = np.random.rand() - .5
    y = np.random.rand() - .5
    z = np.random.rand() - .5
    v = Vec3(x, y, z)
    assert v.norm() == sqrt(x * x + y * y + z * z)
    v.set(1., 0., 0.)
    assert v.norm() == 1.
    v.set(0., 1., 0.)
    assert v.norm() == 1.
    v.set(0., 0., 1.)
    assert v.norm() == 1.


def test_crossUnitVectors():
    u = ml.VEC3_UNIT_X
    v = ml.VEC3_UNIT_Y
    w = u.cross(v)
    assert w.z == 1.
    assert w == ml.VEC3_UNIT_Z


def test_cross():
    for x1 in np.arange(-2, 2, .5):
        for y1 in np.arange(-2, 2, .5):
            for z1 in np.arange(-2, 2, .5):
                for x2 in np.arange(-2, 2, .5):
                    for y2 in np.arange(-2, 2, .5):
                        for z2 in np.arange(-2, 2, .5):
                            v1 = Vec3(x1, y1, z1)
                            v2 = Vec3(x2, y2, z2)
                            v3 = Vec3(
                                y1 * z2 - y2 * z1,
                                z1 * x2 - z2 * x1,
                                x1 * y2 - x2 * y1
                            )
                            assert v1.cross(v2) == v3


def test_Scalar_Product_unit_vectors():
    x = ml.VEC3_UNIT_X
    y = ml.VEC3_UNIT_Y
    assert x * y == 0.
    z = ml.VEC3_UNIT_Z
    assert y * z == 0.
    assert x * x == 1.


def test_Scalar_Product():
    for x1 in np.arange(-2, 2, .5):
        for y1 in np.arange(-2, 2, .5):
            for z1 in np.arange(-2, 2, .5):
                for x2 in np.arange(-2, 2, .5):
                    for y2 in np.arange(-2, 2, .5):
                        for z2 in np.arange(-2, 2, .5):
                            v1 = Vec3(x1, y1, z1)
                            v2 = Vec3(x2, y2, z2)
                            assert v1 * v2 == x1 * x2 + y1 * y2 + z1 * z2


@pytest.mark.xfail
def test_ScalarMultiplication():
    for x in np.arange(-2, 2, .25):
        for y in np.arange(-2, 2, .25):
            for z in np.arange(-2, 2, .25):
                a = (x, y, z)
                for f in np.arange(-2, 2, .25):
                    assert (a * f).x == x * f
                    assert (a * f).y == y * f
                    assert (a * f).z == z * f

@pytest.mark.xfail
def test_ScalarDivision():
    for x in np.arange(-2, 2, .25):
        for y in np.arange(-2, 2, .25):
            for z in np.arange(-2, 2, .25):
                a = (x, y, z)
                for f in np.arange(-2, 2, .25):
                    # fails with:
                    assert (a / f).x == x / f
                    # AttributeError: 'numpy.ndarray' object has no attribute 'x'
                    assert (a / f).y == y / f
                    assert (a / f).z == z / f


def test_distance_unit_x_to_unit_y():
    a = ml.VEC3_UNIT_X
    b = ml.VEC3_UNIT_Y

    assert a.distance_to(b) == sqrt(2.)


def test_distance_to_itself():
    a = Vec3(1.3, 3.7, 4.2)
    assert a.distance_to(a) == 0.


def test_Operator_equals_expect_true():
    a = Vec3(1.3, 3.7, 4.2)
    b = Vec3(1.3, 3.7, 4.2 + 1e-20)
    assert b == a
    assert a == b
    assert not (a != b)


def test_Operator_equals_expect_false():
    a = Vec3(1.3, 3.7, 4.2)
    b = Vec3(1.3, 3.7, 4.2 + 2e-7)
    assert b != a
    assert a != b
    assert not (a == b)


def test_parallel_to_x_y_plane():
    a = Vec3(1., 2., 0.)
    assert a.is_parallel_to_x_y_plane()

    b = Vec3(1.3, 3.7, 4.2 + 1e-9)
    assert not (b.is_parallel_to_x_y_plane())

    c = Vec3(0., 2., 5.5)
    assert not (c.is_parallel_to_x_y_plane())


def test_parallel_to_z_axis():
    a = Vec3(0., 0., 1.)
    assert a.is_paralell_to_z_axis()
    a.set(0., 0., -1.)
    assert a.is_paralell_to_z_axis()
    a.set(0., 0., 0.)
    assert not (a.is_paralell_to_z_axis())


def test_normalize():
    a = Vec3(1., 2., 3.)
    assert a.norm() != 1.

    a.normalize()
    assert a.norm() == 1.

    a *= 2.
    assert a.norm() != 1.

    a.normalize()
    assert a.norm() == 1.

    a = Vec3(0, 0, 0)
    assert a.norm() != 1.

    a.normalize()
    assert np.isnan(a.norm())


def test_angle_in_between():
    a = ml.VEC3_UNIT_X
    b = ml.VEC3_UNIT_X

    assert a.angle_in_between(b) == 0.
    assert a.angle_in_between(b) == b.angle_in_between(a)

    c = ml.VEC3_UNIT_X * 5.
    assert c.norm() != 1.

    d = ml.VEC3_UNIT_X * 5.
    assert d.norm() != 1.

    assert d.angle_in_between(c) == 0.
    assert d.angle_in_between(c) == c.angle_in_between(d)

    foo = ml.VEC3_UNIT_X * 5. + ml.VEC3_UNIT_Y * 5.
    assert c.norm() != 1.

    bar = ml.VEC3_UNIT_X * 5.
    assert d.norm() != 1.

    assert ml.deg2rad(45.) == pytest.approx(
        foo.angle_in_between(bar),
        abs=1e-5
    )


def test_octant_encoding():
    # x y z sector
    # - - -   0
    # - - +   1
    # - + -   2
    # - + +   3
    # + - -   4
    # + - +   5
    # + + -   6
    # + + +   7
    a = Vec3()
    p = +1.
    n = -1.

    a.set(n, n, n)
    assert a.octant() == 0

    a.set(n, n, p)
    assert a.octant() == 1

    a.set(n, p, n)
    assert a.octant() == 2

    a.set(n, p, p)
    assert a.octant() == 3

    a.set(p, n, n)
    assert a.octant() == 4

    a.set(p, n, p)
    assert a.octant() == 5

    a.set(p, p, n)
    assert a.octant() == 6

    a.set(p, p, p)
    assert a.octant() == 7

    a.set(0., 0., 0.)
    assert a.octant() == 7
    a.set(0., 0., p)
    assert a.octant() == 7
    a.set(0., p, 0.)
    assert a.octant() == 7
    a.set(p, 0., 0.)
    assert a.octant() == 7
    a.set(0., p, p)
    assert a.octant() == 7
    a.set(p, p, 0.)
    assert a.octant() == 7
    a.set(p, 0., p)
    assert a.octant() == 7


def test_projection_on_xz_plane():
    v = Vec3(42., 13.37, 3.141)
    assert v.project_in_x_z_plane() == Vec3(42., 0., 3.141)


def test_projection_on_yz_plane():
    v = Vec3(42., 13.37, 3.141)
    assert v.project_in_y_z_plane() == Vec3(0., 13.37, 3.141)


def test_projection_on_xy_plane():
    v = Vec3(42., 13.37, 3.141)
    assert v.project_in_x_y_plane() == Vec3(42., 13.37, 0.)
