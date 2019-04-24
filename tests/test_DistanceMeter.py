import merlict as ml


def test_is_facing():
    sphere = ml.Sphere("sphere_1", ml.Vec3(0, 0, 0), ml.Rot3(0, 0, 0))
    sphere.set_radius(0.5)
    ray = ml.Ray(ml.Vec3(0, 0, -1), ml.Vec3(0, 0, 1))
    dist_meter = ml.DistanceMeter(ray, sphere)

    assert dist_meter.faces_an_object
    assert dist_meter.distance_to_closest_object == 0.5


def test_is_not_facing():
    sphere = ml.Sphere("sphere_1", ml.Vec3(0, 0, 0), ml.Rot3(0, 0, 0))
    sphere.set_radius(0.5)
    ray = ml.Ray(ml.Vec3(0, 0, +1), ml.Vec3(0, 0, 1))
    dist_meter = ml.DistanceMeter(ray, sphere)

    assert not dist_meter.faces_an_object
