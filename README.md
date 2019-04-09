# merlict-development-kit [![Build Status](https://travis-ci.org/cherenkov-plenoscope/merlict_development_kit.svg?branch=master)](https://travis-ci.org/cherenkov-plenoscope/merlict_development_kit)

[![License: GPL v3](https://img.shields.io/badge/License-GPL%20v3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0)

scientific photon propagation and ray tracing in complex sceneries
![img](Readme/fact_and_spider_web.jpg)

In this development-kit, all the sub-projects of the merlict-raytracer are build and tested together.

### dependencies
* git
* g++ >= 6
* cmake
* libopencv-dev >= 2.4.8

### make
```
git clone https://github.com/cherenkov-plenoscope/merlict_development_kit.git
mkdir build
```
The `/build` directory is best located side by side to the `/merlict` directory.

```bash
cd build
cmake ..
make
```

## run
merlict has several executeables. To interacively explore a scenery use `merlict-show`.
```bash
cd ../merlict_viewer/apps/examples
../../../build/merlict-show --scenery fact.json
```

## test
Run the unit tests in the `merlict_development_kit` directory to ensure your build is fine.


```bash
cd ..
./build/merlict-test
```

---

# For Developers playing around with the single header + single source version.

At the moment we are trying to improve the build system, trying to make
merlict a header only library.

At the moment there is a tool called `one_source.py` which can convert the current merlict
sources into one header and one cpp file. It can be used like:

```bash
cd <merlict-development-kit folder>
./one_source.py
```

It creates these 2 files `merlict.h` and `merlict.cpp` and in addition it creates
a test file `merlict_test.cpp`, which can be used to test the project.

You can build it like

```bash
g++ merlict_test.cpp merlict.cpp -o merlict_test
./merlict_test
```
