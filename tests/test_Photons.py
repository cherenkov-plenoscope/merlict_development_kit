import os.path
import pytest
from pytest import approx
import merlict as ml
from merlict import Photon  # class under test here


def test_creation():
    wavelength = 433e-9
    pho = Photon(ml.VEC3_ORIGIN, ml.VEC3_UNIT_Z * 1.337, wavelength)

    assert pho.direction() == ml.VEC3_UNIT_Z
    assert pho.support() == ml.VEC3_ORIGIN
    assert pho.direction().norm() == 1.0
    assert pho.wavelength == wavelength
    # creation is an interaction
    assert pho.num_interactions() == 1


@pytest.mark.skip
def test_reject_negative_wavelenth__coredump():
    # These two result in Core Dumps!!
    # But they should raise an exception
    Photon(ml.VEC3_ORIGIN, ml.VEC3_UNIT_X, 0.0)
    Photon(ml.VEC3_ORIGIN, ml.VEC3_UNIT_X, -1.0)


def test_raw_row2photon():

    dir = ml.Vec3(66.6, 57.8, 99.9)
    dir.normalize()

    raw_row = ml.VectorOfDoubles(
        # id    sx    sy    sz    dx     dy     dz     wavelength
        [0.0, 13.0, 37.0, 42.0, dir.x, dir.y, dir.z, 433.5]
    )

    ph = ml.raw_row2photon(raw_row)

    assert raw_row[0] == ph.simulation_truth_id
    assert raw_row[1] == ph.support().x
    assert raw_row[2] == ph.support().y
    assert raw_row[3] == ph.support().z
    assert ph.direction().x == approx(raw_row[4], 1e-9)
    assert ph.direction().y == approx(raw_row[5], 1e-9)
    assert ph.direction().z == approx(raw_row[6], 1e-9)
    assert raw_row[7] == ph.wavelength


def test_bunch2raw_matrix2bunch():

    num_photons = int(1e3)
    photon_bunch = ml.VectorOfPhotons()
    prng = ml.Mt19937(0)

    for n in range(num_photons):
        sup = ml.Vec3(prng.uniform(), prng.uniform(), prng.uniform())
        dir = ml.Vec3(prng.uniform(), prng.uniform(), prng.uniform())

        id = int(prng.uniform())
        wavelength = prng.uniform()
        ph = Photon(sup, dir, wavelength)
        ph.simulation_truth_id = id
        photon_bunch.push_back(ph)

    raw_matrix = ml.photons2raw_matrix(photon_bunch)

    assert len(raw_matrix) == num_photons

    photon_bunch2 = ml.raw_matrix2photons(raw_matrix)

    assert photon_bunch2.size() == num_photons

    for n in range(num_photons):
        ph1 = photon_bunch[n]
        ph2 = photon_bunch2[n]

        assert ph1.simulation_truth_id == ph2.simulation_truth_id
        assert ph1.support().x == ph2.support().x
        assert ph1.support().y == ph2.support().y
        assert ph1.support().z == ph2.support().z
        assert ph2.direction().x == approx(ph1.direction().x, 1e-9)
        assert ph2.direction().y == approx(ph1.direction().y, 1e-9)
        assert ph2.direction().z == approx(ph1.direction().z, 1e-9)
        assert ph1.wavelength == ph2.wavelength


def test_bunch2raw_matrix2file(tmpdir):

    temp_path = os.path.join(tmpdir, 'bunch2raw_matrix2file.test')

    num_photons = int(1e3)
    photon_bunch = ml.VectorOfPhotons()
    prng = ml.Mt19937(0)

    for n in range(num_photons):
        sup = ml.Vec3(prng.uniform(), prng.uniform(), prng.uniform())
        dir = ml.Vec3(prng.uniform(), prng.uniform(), prng.uniform())

        id = int(prng.uniform())
        wavelength = prng.uniform()
        ph = Photon(sup, dir, wavelength)
        ph.simulation_truth_id = id
        photon_bunch.push_back(ph)

    ml.write_table_to_file(
        ml.photons2raw_matrix(photon_bunch),
        temp_path
    )

    photon_bunch2 = ml.raw_matrix2photons(
        ml.gen_table_from_file(temp_path)
    )

    assert photon_bunch2.size() == num_photons

    for n in range(num_photons):
        ph1 = photon_bunch[n]
        ph2 = photon_bunch2[n]

        assert ph1.simulation_truth_id == ph2.simulation_truth_id
        assert ph1.support().x == approx(ph2.support().x, 1e-9)
        assert ph1.support().y == approx(ph2.support().y, 1e-9)
        assert ph1.support().z == approx(ph2.support().z, 1e-9)
        assert ph2.direction().x == approx(ph1.direction().x, 1e-9)
        assert ph2.direction().y == approx(ph1.direction().y, 1e-9)
        assert ph2.direction().z == approx(ph1.direction().z, 1e-9)
        assert ph1.wavelength == approx(ph2.wavelength, 1e-9)



