import merlict as ml


def test_ctor_xyz_mode():
    x = -3.141
    y = 2.0
    z = 1.0
    r = ml.Rot3(x, y, z)
    assert r.rot_x() == x
    assert r.rot_y() == y
    assert r.rot_z() == z
    assert r.uses_xyz_angels()


def test_ctor_axis_mode():
    v = ml.Vec3(0.0, 0.0, 1.0)
    angle = 1.52
    p = ml.Rot3(v, angle)
    assert p.rot_angle() == angle
    assert p.rot_axis().x == v.x
    assert p.rot_axis().y == v.y
    assert p.rot_axis().z == v.z
    assert not p.uses_xyz_angels()


def test_set_xyz_mode():
    x = -3.141
    y = -2.0
    z = 1.0000
    r = ml.Rot3()
    r.set(x, y, z)
    assert r.rot_x() == x
    assert r.rot_y() == y
    assert r.rot_z() == z
    assert r.uses_xyz_angels()


def test_set_axis_mode():
    v = ml.Vec3(0.0, 0.0, 1.0)
    angle = 1.52
    p = ml.Rot3()
    p.set(v, angle)
    assert p.rot_angle() == angle
    assert p.rot_axis().x == v.x
    assert p.rot_axis().y == v.y
    assert p.rot_axis().z == v.z
    assert not p.uses_xyz_angels()


def test_zero_rot_angle():
    r = ml.Rot3(ml.VEC3_ORIGIN, 0.)
    assert r == ml.ROT3_UNITY

    s = ml.Rot3(ml.VEC3_UNIT_Z, 0.)
    assert s == ml.ROT3_UNITY

    t = ml.Rot3(ml.VEC3_UNIT_Y, 0.)
    assert t == ml.ROT3_UNITY
