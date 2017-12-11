# mctracer scientific ray tracing
# Author: Sebastian Achim Mueller
cmake_minimum_required(VERSION 2.8)
set (CMAKE_CXX_FLAGS "-std=c++1z -Wall -Wextra -pedantic -O1 -g -I")
project(mctracer)

#--------------------------------
# Assert compiler supports C++ 11
include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
if(COMPILER_SUPPORTS_CXX11)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
else()
    message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
endif()

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS 5)
    message(FATAL_ERROR "Insufficient gcc version, need at least gcc 5")
  endif()
endif()
#-------------------
# External libraries
# 	OpenMP
find_package(OpenMP)
if(OPENMP_FOUND)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()

# 	OpenCV
find_package(OpenCV 2.4.8 REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})
link_directories(${OpenCV_LIB_DIR})
#-------------------------------------------------------------------------------
# Cmake
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake-modules)
#-----------
# The Source
set(SOURCE 	
	${CMAKE_CURRENT_SOURCE_DIR}/TelescopeArrayControl.cpp		
	${CMAKE_CURRENT_SOURCE_DIR}/ZdAzMount.cpp
)
add_subdirectory("${PROJECT_SOURCE_DIR}/Visual")
add_subdirectory("${PROJECT_SOURCE_DIR}/Core")
add_subdirectory("${PROJECT_SOURCE_DIR}/Corsika")
add_subdirectory("${PROJECT_SOURCE_DIR}/Scenery")
add_subdirectory("${PROJECT_SOURCE_DIR}/Plenoscope")
add_subdirectory("${PROJECT_SOURCE_DIR}/PhotonSensor")
add_subdirectory("${PROJECT_SOURCE_DIR}/Tools")
add_subdirectory("${PROJECT_SOURCE_DIR}/Xml")
add_subdirectory("${PROJECT_SOURCE_DIR}/PhotonsReader")
add_subdirectory("${PROJECT_SOURCE_DIR}/SignalProcessing")
add_subdirectory("${PROJECT_SOURCE_DIR}/DocOpt")
#-------------------------------
# mctracer library
add_library(mct SHARED ${SOURCE})
include_directories(/usr/include)
include_directories(/usr/local/include)
target_link_libraries(mct ${OpenCV_LIBS})

#---------------------------------
# Python bindings 
add_subdirectory (Pybind11)
add_subdirectory (Bindings)
pybind11_add_module(mctPy ${SOURCE} ${PYBIND11_SOURCE})

#---------
# Show exe
add_executable(mctShow Main/Show.cpp)
target_link_libraries(mctShow mct)
target_link_libraries(mctShow docopt)

#---------------
# Propagate exe
add_executable(mctPropagate Main/Propagate.cpp)
target_link_libraries(mctPropagate mct)
target_link_libraries(mctPropagate docopt)

#----------------
# ShowPhotons exe
add_executable(mctShowPhotons Main/ShowPhotons.cpp)
target_link_libraries(mctShowPhotons mct)
target_link_libraries(mctShowPhotons docopt)

#------------------------
# light field calibration
add_executable(mctPlenoscopeCalibration Main/PlenoscopeCalibration.cpp)
target_link_libraries(mctPlenoscopeCalibration mct)
target_link_libraries(mctPlenoscopeCalibration stdc++fs)
target_link_libraries(mctPlenoscopeCalibration docopt)

#------------------------
# EventIOConverter
add_executable(mctEventIOConverter Main/EventIOConverter.cpp)
target_link_libraries(mctEventIOConverter mct)
target_link_libraries(mctEventIOConverter stdc++fs)
target_link_libraries(mctEventIOConverter docopt)

#------------------------
# light field propagation
add_executable(mctPlenoscopePropagation Main/PlenoscopePropagation.cpp)
target_link_libraries(mctPlenoscopePropagation mct)
target_link_libraries(mctPlenoscopePropagation stdc++fs)
target_link_libraries(mctPlenoscopePropagation docopt)

#------------------------
# light field nsb only propagation
add_executable(mctPlenoscopeNsbOnlyPropagation Main/PlenoscopeNsbOnlyPropagation.cpp)
target_link_libraries(mctPlenoscopeNsbOnlyPropagation mct)
target_link_libraries(mctPlenoscopeNsbOnlyPropagation stdc++fs)
target_link_libraries(mctPlenoscopeNsbOnlyPropagation docopt)

#---------------------------------
# unit test exe
add_subdirectory("${PROJECT_SOURCE_DIR}/Tests/gtest-1.7.0")
include_directories("${PROJECT_SOURCE_DIR}/Tests/gtest-1.7.0/include")
add_subdirectory("${PROJECT_SOURCE_DIR}/Tests")

add_executable(mctTest ./Tests/test.cpp ${TEST_SOURCE})
target_link_libraries(mctTest gtest gtest_main)
target_link_libraries(mctTest mct)

#---------
# STL scaling
add_executable(mctScaleStl Main/ScaleStl.cpp)
target_link_libraries(mctScaleStl mct)
target_link_libraries(mctScaleStl docopt)

#---------------------------------
# Benchmark
add_subdirectory("${PROJECT_SOURCE_DIR}/Benchmarks")

add_executable(mctBenchMark ./Benchmarks/benchmark.cpp ${TEST_BENCHMARK})
target_link_libraries(mctBenchMark gtest gtest_main)
target_link_libraries(mctBenchMark mct)
