import pytest
import merlict as ml


def test_init_empty():
    my_colors = ml.ColorMap()
    assert not my_colors.has("red")


@pytest.mark.skip
def test_get_not_existing_color():
    my_colors = ml.ColorMap()

    # the call below creates a core dump

    # CHECK_THROWS_AS(my_colors.get("red"), ml.ColorMap::NoSuchKey)
    my_colors.get("red")


def test_add_color_and_check_it_is_in():
    my_colors = ml.ColorMap()
    my_colors.add("red", ml.Color(255, 0, 0))
    assert my_colors.has("red")


@pytest.mark.skip
def test_add_color_and_add_it_again():
    my_colors = ml.ColorMap()
    my_colors.add("red", ml.Color(255, 0, 0))

    # the call below creates a core dump

    # CHECK_THROWS_AS(my_colors.add("red", ml.Color(128, 0, 0)), ml.ColorMap::KeyAlreadyInUse)
    my_colors.add("red", ml.Color(128, 0, 0))


def test_add_color_and_get_it_again():
    my_colors = ml.ColorMap()
    novel_red = ml.Color(255, 0, 0)
    my_colors.add("red", novel_red)
    my_red = my_colors.get("red")
    assert novel_red.r == my_red.r
    assert novel_red.g == my_red.g
    assert novel_red.b == my_red.b
