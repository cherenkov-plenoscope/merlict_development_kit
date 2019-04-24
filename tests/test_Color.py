import merlict as ml
from merlict import Color
from pytest import approx


def test_default_ctor():
    print('early:', ml.VEC3_ORIGIN.str())
    c = Color()
    assert c.r == 0
    assert c.g == 0
    assert c.b == 0


def test_reflection_mix():
    base = Color(0, 0, 0)
    green = Color(0, 100, 0)
    reflectivity = 0.5
    base.reflection_mix(green, reflectivity)
    assert base.r == 0
    assert base.g == 50
    assert base.b == 0


def test_ctor_mix():
    rainbow = ml.VectorOfColors()
    rainbow.push_back(Color(100, 0, 0))
    rainbow.push_back(Color(0, 100, 0))
    rainbow.push_back(Color(0, 0, 100))
    mix = Color(rainbow)

    assert mix.r == approx(33, 1)
    assert mix.g == approx(33, 1)
    assert mix.b == approx(33, 1)
