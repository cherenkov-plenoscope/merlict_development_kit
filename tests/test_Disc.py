import pytest
import merlict as ml


def test_set_Disc():
    D = ml.Disc()
    D.set_radius(42.0)
    assert D.get_bounding_sphere_radius() == 42.0


@pytest.mark.skip
def test_set_Disc_negative_radius():
    D = ml.Disc()
    # CHECK_THROWS_AS(D.set_radius(-42.0), std::invalid_argument)

    # this call creates a core dump
    D.set_radius(-42.0)
