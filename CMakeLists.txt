# Copyright 2011-2018 Sebastian A. Mueller
cmake_minimum_required(VERSION 2.8)

project(merlict)

# Assert compiler supports C++ 17
include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++17" COMPILER_SUPPORTS_CXX17)
if(COMPILER_SUPPORTS_CXX17)
    set(CMAKE_CXX_FLAGS "-std=c++17 -pthread -Wall -Wextra -pedantic -O1 -g -I")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
else()
    message(
        STATUS
        "The compiler ${CMAKE_CXX_COMPILER} has no C++17 support.
        Please use a different C++ compiler.")
endif()

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS 6)
    message(FATAL_ERROR "Insufficient gcc version, need at least gcc 6")
  endif()
endif()

# OpenCV
find_package(OpenCV)
include_directories(${OpenCV_INCLUDE_DIRS})
link_directories(${OpenCV_LIB_DIR})

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})
set(
    CMAKE_MODULE_PATH
    ${CMAKE_MODULE_PATH}
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake-modules)

# source
add_subdirectory("${PROJECT_SOURCE_DIR}/merlict")
add_subdirectory("${PROJECT_SOURCE_DIR}/merlict_corsika")
add_subdirectory("${PROJECT_SOURCE_DIR}/merlict_json")
add_subdirectory("${PROJECT_SOURCE_DIR}/merlict_visual")
add_subdirectory("${PROJECT_SOURCE_DIR}/merlict_multi_thread")
add_subdirectory("${PROJECT_SOURCE_DIR}/merlict_portal_plenoscope")
add_subdirectory("${PROJECT_SOURCE_DIR}/merlict_signal_processing")
add_subdirectory("${PROJECT_SOURCE_DIR}/docopt")
add_subdirectory("${PROJECT_SOURCE_DIR}/merlict_viewer")

# merlict library
add_library(lib_merlict_dev SHARED ${SOURCE})
include_directories(/usr/include)
include_directories(/usr/local/include)
target_link_libraries(lib_merlict_dev ${OpenCV_LIBS})

#apps
add_executable(
    merlict-show
    merlict_viewer/apps/show.cpp)
target_link_libraries(merlict-show lib_merlict_dev)
target_link_libraries(merlict-show docopt)

add_executable(
    merlict-cameraserver
    merlict_visual/apps/camera_server.cpp)
target_link_libraries(merlict-cameraserver lib_merlict_dev)
target_link_libraries(merlict-cameraserver docopt)

add_executable(
    merlict-propagate
    merlict_tests/apps/propagate.cpp)
target_link_libraries(merlict-propagate lib_merlict_dev)
target_link_libraries(merlict-propagate docopt)

add_executable(
    merlict-show-photons
    merlict_viewer/apps/show_photons.cpp)
target_link_libraries(merlict-show-photons lib_merlict_dev)
target_link_libraries(merlict-show-photons docopt)

add_executable(
    merlict-plenoscope-calibration
    merlict_portal_plenoscope/apps/plenoscope_calibration.cpp)
target_link_libraries(merlict-plenoscope-calibration lib_merlict_dev)
target_link_libraries(merlict-plenoscope-calibration stdc++fs)
target_link_libraries(merlict-plenoscope-calibration docopt)

add_executable(
    merlict-eventio-converter
    merlict_corsika/apps/eventio_converter.cpp)
target_link_libraries(merlict-eventio-converter lib_merlict_dev)
target_link_libraries(merlict-eventio-converter stdc++fs)
target_link_libraries(merlict-eventio-converter docopt)

add_executable(
    merlict-plenoscope-propagation
    merlict_portal_plenoscope/apps/plenoscope_propagation.cpp)
target_link_libraries(merlict-plenoscope-propagation lib_merlict_dev)
target_link_libraries(merlict-plenoscope-propagation stdc++fs)
target_link_libraries(merlict-plenoscope-propagation docopt)

add_executable(
    merlict-plenoscope-raw-photon-propagation
    merlict_portal_plenoscope/apps/plenoscope_raw_photon_propagation.cpp)
target_link_libraries(merlict-plenoscope-raw-photon-propagation lib_merlict_dev)
target_link_libraries(merlict-plenoscope-raw-photon-propagation stdc++fs)
target_link_libraries(merlict-plenoscope-raw-photon-propagation docopt)

add_executable(
    merlict-fact
    merlict_fact/apps/fact_propagation.cpp)
target_link_libraries(merlict-fact lib_merlict_dev)
target_link_libraries(merlict-fact stdc++fs)
target_link_libraries(merlict-fact docopt)

# tests
add_subdirectory("${PROJECT_SOURCE_DIR}/merlict/tests")
add_subdirectory("${PROJECT_SOURCE_DIR}/merlict_corsika/tests")
add_subdirectory("${PROJECT_SOURCE_DIR}/merlict_visual/tests")
add_subdirectory("${PROJECT_SOURCE_DIR}/merlict_json/tests")
add_subdirectory("${PROJECT_SOURCE_DIR}/merlict_multi_thread/tests")
add_subdirectory("${PROJECT_SOURCE_DIR}/merlict_viewer/tests")
add_subdirectory("${PROJECT_SOURCE_DIR}/merlict_tests")
add_subdirectory("${PROJECT_SOURCE_DIR}/merlict_portal_plenoscope/tests")
add_subdirectory("${PROJECT_SOURCE_DIR}/merlict_signal_processing/tests")
add_executable(
    merlict-test
    merlict/tests/test.cpp
    ${TEST_SOURCE_PLENOSCOPE}
    ${TEST_SOURCE_SIGNAL_PROCESSING}
    ${TEST_SOURCE_MERLICT}
    ${TEST_SOURCE_MERLICT_CORSIKA}
    ${TEST_SOURCE_MERLICT_VISUAL}
    ${TEST_SOURCE_MERLICT_JSON}
    ${TEST_SOURCE_MERLICT_MULTI_THREAD}
    ${TEST_SOURCE_MERLICT_VIEWER}
    ${TEST_SOURCE_MERLICT_INTEGRATION})
target_link_libraries(merlict-test lib_merlict_dev)
