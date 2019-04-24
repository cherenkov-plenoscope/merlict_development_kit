from math import sqrt
import pytest
import merlict as ml
import numpy as np

def test_causal_limits_correct():
    # should not throw
    ml.Limits(0.0, 1.0)


@pytest.mark.skip
def test_causal_limits_false():
    # will cause seg fault
    ml.Limits(0.0, -1.0)


def test_causal_limits_lower_included():
    limits = ml.Limits(0., 1.)
    limits.assert_contains(0.)


@pytest.mark.skip
def test_causal_limits_upper_excluded():
    limits = ml.Limits(0., 1.)

    with pytest.raises(out_of_range):
        limits.assert_contains(1.0)


@pytest.mark.skip
def test_causal_limits_above_upper():
    limits = ml.Limits(0., 1.)

    with pytest.raises(out_of_range):
        limits.assert_contains(1.1)


@pytest.mark.skip
def test_causal_limits_below_lower():
    limits = ml.Limits(0., 1.)

    with pytest.raises(out_of_range):
        limits.assert_contains(-0.1)


def test_check_limits():
    limits = ml.Limits(4.2, 133.7)
    assert limits.lower == 4.2
    assert limits.upper == 133.7


def test_default_limits():
    limits = ml.Limits()
    assert limits.lower == 0.
    assert limits.upper == 0.


@pytest.mark.skip
def test_default_limits():
    limits = ml.Limits()

    # the stuff below seg faults
    limits.assert_contains(-0.1)
    limits.assert_contains(0.0)
    limits.assert_contains(+0.1)


@pytest.mark.skip
def test_limit_default_range():
    limits = ml.Limits()
    assert limits.range() == 0.0


def test_limit_range():
    limits = ml.Limits(0., 1.)
    assert limits.range() == 1.0


def test_polynom3_size():
    num_samples = 137
    f = ml.polynom3(0, 0, 1, 0, -1, 1, num_samples)
    assert f.size() == num_samples


def test_polynom3_limits():
    num_samples = 137
    f = ml.polynom3(0, 0, 1, 0, -1, 1, num_samples)

    # both C++ vector and Python lingo works
    assert f.size() == num_samples
    assert len(f) == num_samples

    assert len(f[0]) == 2
    assert f[0][0] == -1

    assert len(f[-1]) == 2
    assert f[-1][0] == 1


def test_sampling_table_size():
    f = ml.Func1(ml.polynom3(0, 0, 1, 0, -4.2, 1.337, 4096))
    sample_xy = ml.sample(f, 1000)

    # testing Pythonic STL-vector interface only
    assert len(sample_xy) == 1000

    for point in sample_xy:
        assert len(point) == 2


def test_sampling_table_x_and_y_values():
    samples = 10

    f = ml.Func1(ml.polynom3(0, 1, 0, 0, -4.2, 1.337, 4096))
    sample_xy = ml.sample(f, samples)

    assert len(sample_xy) == samples
    for i, point in enumerate(sample_xy):
        assert len(point) == 2
        x = -4.2 + i * (1.337 + 4.2) / samples
        assert point[0] == pytest.approx(x, abs=1.5e-3)
        assert point[1] == pytest.approx(x * x, abs=1e-6)
        assert f.evaluate(x) == pytest.approx(x * x, abs=1e-6)


def test_numerical_integration_const():
    # f(x) = 1
    # F(x) = x
    f = ml.Func1(ml.polynom3(0, 0, 0, 1, 0, 1, 4096))
    F = ml.integral(f)
    assert F.limits.lower == f.limits.lower
    assert F.limits.upper == f.limits.upper
    # F(0) != 0
    assert 0.0 == pytest.approx(F.evaluate(0.0), abs=1e-3)
    # F(0.5) != 0.5
    assert 0.5 == pytest.approx(F.evaluate(0.5), abs=1e-3)
    # F(1) != 1
    assert 1.0 == pytest.approx(F.evaluate(1.0 - 1e-6), abs=1e-3)


def test_numerical_integration_linear():
    # f(x) = x
    # F(x) = 1/2 x^2
    f = ml.Func1(ml.polynom3(0, 0, 1, 0, 0, 1, 4096))
    F = ml.integral(f)
    assert F.limits.lower == f.limits.lower
    assert F.limits.upper == f.limits.upper
    # F(0) != 0
    assert 0.0 == pytest.approx(F.evaluate(0.0), abs=1e-3)
    # F(0.5) != 0.5
    assert 0.125 == pytest.approx(F.evaluate(0.5), abs=1e-3)
    # F(1) != 1
    assert 0.5 == pytest.approx(F.evaluate(1.0 - 1e-6), abs=1e-3)


def test_numerical_inverse_limits():
    # f(x) = 2x [0, 1)
    # f_inv(x) = 1/2x [0, 2)
    f = ml.Func1(ml.polynom3(0, 0, 2, 0, 0, 1, 4096))
    f_inv = ml.inverse(f)
    assert 0.0 == pytest.approx(f_inv.limits.lower, abs=1e-6)
    assert 2.0 == pytest.approx(f_inv.limits.upper, abs=1e-6)


def test_numerical_inverse_linear():
    # f(x) = x
    # f_inv(x) = x
    f = ml.Func1(ml.polynom3(0, 0, 1, 0, 0, 1, 4096))
    f_inv = ml.inverse(f)
    assert 0.0 == pytest.approx(f_inv.limits.lower, abs=1e-6)
    assert 1.0 == pytest.approx(f_inv.limits.upper, abs=1e-6)
    # f_inv(0) != 0
    assert 0.0 == pytest.approx(f_inv.evaluate(0.0), abs=1e-3)
    # f_inv(0.5) != 0.5
    assert 0.5 == pytest.approx(f_inv.evaluate(0.5), abs=1e-3)
    # f_inv(1) != 1
    assert 1.0 == pytest.approx(f_inv.evaluate(1.0 - 1e-6), abs=1e-3)


def test_numerical_inverse_quadratic():
    # f(x) = x^2
    # f_inv(x) = sqrt(x)
    f = ml.Func1(ml.polynom3(0, 1, 0, 0, 0, 1, 4096))
    f_inv = ml.inverse(f)
    assert 0.0 == pytest.approx(f_inv.limits.lower, abs=1e-6)
    assert 1.0 == pytest.approx(f_inv.limits.upper, abs=1e-6)

    # f_inv(0) != sqrt(0) = 0
    assert 0.0 == pytest.approx(f_inv.evaluate(0.0), abs=1e-3)
    # f_inv(0.5) != sqrt(0.5) = sqrt(2)/2
    assert sqrt(2.0) / 2.0 == pytest.approx(f_inv.evaluate(0.5), abs=1e-3)
    # f_inv(1) != sqrt(1.0) = 1.0
    assert 1.0 == pytest.approx(f_inv.evaluate(1.0 - 1e-6), abs=1e-3)


def test_numerical_derivative_of_constant_function():
    # f(x) = 1
    # f'(x) = 0
    f = ml.Func1(ml.polynom3(0, 0, 0, 1, 0, 1, 4096))
    f_prime = ml.derivative(f)
    assert f_prime.limits.lower == f.limits.lower
    assert f_prime.limits.upper == f.limits.upper
    for x in np.arange(0., 1., 1e-3):
        assert 0.0 == pytest.approx(f_prime.evaluate(x), abs=1e-3)


def test_numerical_derivative_of_linear_function():
    # f(x) = x + 1
    # f'(x) = 1
    f = ml.Func1(ml.polynom3(0, 0, 1, 1, 0, 1, 4096))
    f_prime = ml.derivative(f)
    assert f_prime.limits.lower == f.limits.lower
    assert f_prime.limits.upper == f.limits.upper
    for x in np.arange(0., 1.0, 1e-3):
        assert 1.0 == pytest.approx(f_prime.evaluate(x), abs=1e-3)


def test_numerical_derivative_of_quadratic_function():
    # f(x) = x^2
    # f'(x) = 2*x
    f = ml.Func1(ml.polynom3(0, 1, 0, 0, 0, 1, 4096))
    f_prime = ml.derivative(f)
    assert f_prime.limits.lower == f.limits.lower
    assert f_prime.limits.upper == f.limits.upper
    for x in np.arange(0., 1.0, 1e-3):
        assert 2.0 * x == pytest.approx(f_prime.evaluate(x), abs=1e-3)


def test_numerical_sign_flip_of_value_of_constant_function():
    # f(x) = 0
    # sign flip -> no
    f = ml.Func1(ml.polynom3(0, 0, 0, 0, 0, 1, 4096))
    assert not ml.value_flips_sign(f)


def test_numerical_sign_flip_of_value_of_linear_function():
    # f(x) = x [-1, 1)
    # sign flip -> yes
    f = ml.Func1(ml.polynom3(0, 0, 1, 0, -1, 1, 4096))
    assert ml.value_flips_sign(f)


def test_numerical_sign_flip_of_value_of_quadratic_function():
    # f(x) = x^2 - 1 [-1, 1)
    # sign flip -> yes
    f = ml.Func1(ml.polynom3(0, 1, 0, -1, -1, 1, 4096))
    assert ml.value_flips_sign(f)


def test_numerical_sign_flip_of_value_of_quadratic_function_no():
    # f(x) = x^2 [0, 1)
    # sign flip -> no
    f = ml.Func1(ml.polynom3(0, 1, 0, 0, 0, 1, 4096))
    assert not ml.value_flips_sign(f)


def test_mean():
    # f(x) = x [0, 1)
    # sign flip -> no
    f = ml.Func1(ml.polynom3(0, 0, 1, 0, 0, 1, 4096))
    mean = ml.function_mean(f, 1337)
    assert mean == pytest.approx(0.5, abs=1e-3)
