#!/usr/bin/env python3

import numpy as np
import os
import glob
copyright_notice = "// Copyright 2018 Sebastian A. Mueller\n"


def inside_namespace(txt):
    start_first_namespace = txt.find("{", txt.find('namespace merlict')) + 1
    if start_first_namespace == 0:
        return ""
    pos = start_first_namespace
    bracket_count = 1

    while bracket_count > 0:
        next_o = txt.find("{", pos)
        next_c = txt.find("}", pos)
        if next_o == -1:
            next_o = 9999999999
        if next_c == -1:
            next_c = 9999999999

        if next_c < next_o:
            bracket_count -= 1
            pos = next_c + 1
        elif next_o < next_c:
            bracket_count += 1
            pos = next_o + 1
        else:
            break

    end_namespace = pos

    in_first_namespace = txt[start_first_namespace:end_namespace - 1]

    rest_of_txt = txt[end_namespace:]

    if rest_of_txt.find("namespace merlict") == -1:
        return in_first_namespace
    else:
        return in_first_namespace + "\n" + inside_namespace(rest_of_txt)


def all_includes(txt):
    includes = []
    for line in txt.splitlines():
        if line.find("#include <") != -1:
            includes.append(line)
    return includes


def all_usings(txt):
    usings = []
    for line in txt.splitlines():
        if line.find("using ") == 0:
            usings.append(line)
    return usings


def without_include_lines(txt):
    out = []
    for line in txt.splitlines():
        if not line.find("#include") != -1:
            out.append(line)
    return "\n".join(out)


def find_all_header_files_in_dirs(dirs):
    headers = []
    for d in dirs:
        for header in glob.glob(os.path.join(d, '*.h')):
            headers.append(header)
    return headers


def make_pri_heads(headers, go_first):
    prior = 1000
    pri_heads = []
    for header in headers:
        has_pri = False
        for pri in go_first:
            if pri in header:
                pri_heads.append(go_first[pri])
                has_pri = True
        if not has_pri:
            pri_heads.append(prior)
            prior += 1
    return pri_heads


def sort_headers_with_pri_heads(headers, pri_heads):
    pri_heads = np.array(pri_heads)
    argsort = np.argsort(pri_heads)

    headers = [headers[i] for i in argsort]
    return headers


def find_sources_from_headers(headers):
    sources = []
    for header in headers:
        potentiual_source_path = os.path.splitext(header)[0] + '.cpp'
        if os.path.exists(potentiual_source_path):
            sources.append(potentiual_source_path)
    return sources


def make_header_includes(headers):
    header_includes = []
    for header in headers:
        with open(header, 'rt') as fin:
            txt = fin.read()
        incs = all_includes(txt)
        for inc in incs:
            header_includes.append(inc)

    header_includes = list(set(header_includes))
    return header_includes


def find_source_incl_and_usings(sources):
    source_includes = []
    source_usings = []

    for source in sources:
        with open(source, 'rt') as fin:
            txt = fin.read()
        incs = all_includes(txt)
        for inc in incs:
            source_includes.append(inc)
        usis = all_usings(txt)
        for usi in usis:
            source_usings.append(usi)

    source_includes = list(set(source_includes))
    source_usings = list(set(source_usings))

    return source_includes, source_usings


def write_the_single_header_file(out_path_h, headers, header_includes):
    with open(out_path_h, 'wt') as fout:
        fout.write(copyright_notice)
        fout.write("\n")

        fout.write("#ifndef MERLICT_H_\n")
        fout.write("#define MERLICT_H_\n")
        fout.write("\n")

        for header_include in header_includes:
            fout.write(header_include + '\n')
        fout.write("\n")

        fout.write("namespace merlict {\n")

        for header in headers:
            with open(header, 'rt') as fin:
                h_txt = fin.read()
            fout.write(inside_namespace(h_txt))
            fout.write("\n")

        fout.write("}  // namespace merlict\n")
        fout.write("\n")

        fout.write("#endif  // MERLICT_H_\n")


def write_the_single_cpp_file(out_path_cpp, sources, source_includes):
    with open(out_path_cpp, 'wt') as fout:
        fout.write(copyright_notice)
        fout.write("\n")

        fout.write('#include "merlict.h"\n')
        for source_include in source_includes:
            fout.write(source_include + '\n')
        fout.write("\n")

        fout.write("namespace merlict {\n")

        for source in sources:
            with open(source, 'rt') as fin:
                h_txt = fin.read()
            fout.write(without_include_lines(inside_namespace(h_txt)))
            fout.write("\n")

        fout.write("}  // namespace merlict\n")
        fout.write("\n")


def do_something_with_tests(headers, sources, out_path_test):

    tests = []
    test_includes = []

    for header in headers:
        potentiual_source_path = os.path.splitext(header)[0]+'.cpp'
        if os.path.exists(potentiual_source_path):
            sources.append(potentiual_source_path)

    for test_source in test_sources:
        with open(test_source, 'rt') as fin:
            txt = fin.read()
        incs = all_includes(txt)
        for inc in incs:
            test_includes.append(inc)

    test_includes = list(set(test_includes))

    with open(out_path_test, 'wt') as fout:
        fout.write(copyright_notice)
        fout.write("\n")
        fout.write("#define CATCH_CONFIG_MAIN\n")
        fout.write("\n")
        fout.write('#include "merlict.h"\n')
        fout.write('#include "merlict/tests/catch.hpp"\n')
        for test_include in test_includes:
            fout.write(test_include + '\n')
        fout.write("\n")

        for test_source in test_sources:
            with open(test_source, 'rt') as fin:
                h_txt = fin.read()
            fout.write(without_include_lines(h_txt))
            fout.write("\n")

        fout.write("\n")



dirs = [
    "merlict/function",
    "merlict/random",
    "merlict",
    "merlict/sensor",
    "merlict/scenery",
    "merlict/scenery/geometry",
    "merlict/scenery/primitive",
    "merlict/scenery/segmented_imaging_reflector",
]

pri_queu = [
    "Vec2",
    "Vec3",
    "Rot3",
    "HomTra3",
    "merlict/Color",
    "Limits",
    "Func1",
    "random/Generator",
    "random/Mt19937",
    "random/random.h",
    "merlict/Frame.h",
    "merlict/SurfaceEntity.h",
    "merlict/Frames",
    "merlict/Intersection",
    "merlict/Ray.h",
    "merlict/RayAndFrame",
    "merlict/RayForPropagation",
    "merlict/Photon.h",
    "merlict/PropagationConfig",
    "merlict/PropagationEnvironment",
    "merlict/FresnelRefractionAndReflection.h",
    "merlict/Propagator.h",
    "scenery/segmented_imaging_reflector/Config.h",
    "scenery/segmented_imaging_reflector/Geometry.h",
    "scenery/segmented_imaging_reflector/Factory.h",
    "scenery/geometry/PrismZ.h",
    "sensor/PhotonArrival.h",
    "sensor/PhotonArrivals.h",
    "sensor/Sensor.h",
    "scenery/ResourceMap.h",
    "ColorMap",
    "SensorMap",
    "FunctionMap",
    'merlict/scenery/geometry/ZaxisCylinderRayIntersectionEquation.h',
    'merlict/scenery/geometry/TwoSolutionSurfaceRayEquation.h',
    'merlict/scenery/geometry/SurfaceWithOuterPrismBound.h',
    'merlict/scenery/geometry/XyPlaneRayIntersectionEquation.h',
    'merlict/scenery/geometry/SphericalCapRayIntersectionEquation.h',
    'merlict/scenery/geometry/HexagonalPrismZ.h',
    'merlict/scenery/geometry/HexGridFlower.h',
    'merlict/scenery/geometry/EllipticalCapRayIntersectionEquation.h',
    'merlict/scenery/geometry/HexGridAnnulus.h',
]

test_sources = [
    #"tests/PrintSizeOfTest.cpp",
    "tests/ColorTest.cpp",
    "tests/PhotonsTest.cpp",
    "tests/AsciiIoTest.cpp",
    "tests/PhotonTest.cpp",
    "tests/BiConvexLensTest.cpp",
    "tests/PlaneIntersectionTest.cpp",
    "tests/PostInitFrameSpeed.cpp",
    "tests/PropagationEnvironmentTest.cpp",
    "tests/QuadraticEquationTest.cpp",
    "tests/DiscTest.cpp",
    "tests/RandomGeneratorTest.cpp",
    "tests/DualSphericalPrismZTest.cpp",
    "tests/RayForPropagationTest.cpp",
    "tests/RayTest.cpp",
    "tests/RayAndFrameTest.cpp",
    "tests/Rot3Test.cpp",
    "tests/SensorStorageTest.cpp",
    "tests/SensorAssignmentTest.cpp",
    "tests/SensorInOutTest.cpp",
    "tests/FrameTest.cpp",
    "tests/FramesTest.cpp",
    "tests/FresnelRefractionAndReflectionTest.cpp",
    "tests/SphereIntersectionTest.cpp",
    "tests/SphereTest.cpp",
    "tests/FunctionLinInterpolTest.cpp",
    "tests/StringToolsTest.cpp",
    "tests/FunctionTest.cpp",
    "tests/GridNeighborhoodTopoligyTest.cpp",
    "tests/HexagonalPrismZTest.cpp",
    "tests/HexGridAnnulusTest.cpp",
    "tests/ThinLensEquationTest.cpp",
    "tests/Histogram1Test.cpp",
    "tests/HomTra3Test.cpp",
    "tests/ToolTest.cpp",
    "tests/StereoLitographyTest.cpp",
    "tests/LensMakerTest.cpp",
    "tests/Vec2Test.cpp",
    "tests/Vec3Test.cpp",
    "tests/XyPlaneRayEquationTest.cpp",
    "tests/PathToolsTest.cpp",
    "tests/ZaxisCylinderRayIntersectionEquationTest.cpp",
    "tests/SmallBallsTest.cpp",
    "tests/OwnerShipTest.cpp",
    "tests/ColorMapTest.cpp",
    "tests/FunctionMapTest.cpp",
    "tests/SensorMapTest.cpp",
    "tests/DistanceMeterTest.cpp",
]

test_sources = [
    os.path.join('merlict', x)
    for x in test_sources
]

def main():
    go_first = {
        pri_q: s
        for s, pri_q
        in enumerate(pri_queu)
    }

    out_path_h = 'merlict.h'
    out_path_cpp = 'merlict.cpp'
    out_path_test = 'merlict_test.cpp'

    headers = find_all_header_files_in_dirs(dirs)
    pri_heads = make_pri_heads(headers, go_first)

    headers = sort_headers_with_pri_heads(headers, pri_heads)
    sources = find_sources_from_headers(headers)
    header_includes = make_header_includes(headers)

    source_includes, source_usings = find_source_incl_and_usings(sources)

    write_the_single_header_file(out_path_h, headers, header_includes)
    write_the_single_cpp_file(out_path_cpp, sources, source_includes)
    do_something_with_tests(headers, sources, out_path_test)



# g++ docopt/docopt.cpp lict_prop.cpp -o prop -std=gnu++11
if __name__ == '__main__':
    main()
