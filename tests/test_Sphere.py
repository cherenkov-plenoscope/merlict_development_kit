import pytest
import merlict as ml


def test_set_positiv_radius():
    S = ml.Sphere()
    assert S.set_radius(1.0) is None


@pytest.mark.skip(reason='CPP_EXCEPTION_NOT_WRAPPED_BY_SWIG')
def test_set_negativ_radius():
    S = ml.Sphere()
    with pytest.raises(ml.BadRadius):
        S.set_radius(-1.0)


@pytest.mark.skip(reason='CPP_EXCEPTION_NOT_WRAPPED_BY_SWIG')
def test_set_zero_radius():
    S = ml.Sphere()
    with pytest.raises(ml.BadRadius):
        S.set_radius(-1.0)
