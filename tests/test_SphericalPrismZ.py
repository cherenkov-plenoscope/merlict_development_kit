import merlict as ml


def test_is_inside():
    bound = ml.DualSphericalPrismZ()
    bound.set_x_hight_and_y_width(1.0, 2.0)
    v = ml.Vec3(0.0, 0.0, 0.0)
    assert bound.is_inside(v)
    v.set(0.0, 0.0, 1.0)
    assert bound.is_inside(v)
    v.set(0.0, 0.0, -1.0)
    assert bound.is_inside(v)
    v.set(0.0, +0.99, 0.0)
    assert bound.is_inside(v)
    v.set(0.0, -0.99, 0.0)
    assert bound.is_inside(v)
    v.set(+0.49, 0.0, 0.0)
    assert bound.is_inside(v)
    v.set(-0.49, 0.0, 0.0)
    assert bound.is_inside(v)


def test_is_outside():
    bound = ml.DualSphericalPrismZ()
    bound.set_x_hight_and_y_width(1.0, 2.0)
    v = ml.Vec3(0.0, 0.0, 0.0)
    v.set(+0.49, +0.99, 0.0)
    assert not bound.is_inside(v)
    v.set(+0.49, -0.99, 0.0)
    assert not bound.is_inside(v)
    v.set(-0.49, +0.99, 0.0)
    assert not bound.is_inside(v)
    v.set(-0.49, -0.99, 0.0)
    assert not bound.is_inside(v)

