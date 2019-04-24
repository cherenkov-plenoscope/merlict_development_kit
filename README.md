# merlict-development-kit [![Build Status](https://travis-ci.org/cherenkov-plenoscope/merlict_development_kit.svg?branch=master)](https://travis-ci.org/cherenkov-plenoscope/merlict_development_kit) [![codecov](https://codecov.io/gh/cherenkov-plenoscope/merlict_development_kit/branch/master/graph/badge.svg)](https://codecov.io/gh/cherenkov-plenoscope/merlict_development_kit)

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

At the moment there is a tool called `amalgamate.py` which can convert the current merlict
sources into one header and one cpp file. It can be used like:

```bash
cd <merlict-development-kit folder>
./amalgamate.py
```

It creates these 2 files `merlict.h` and `merlict.cpp` and in addition it creates
a test file `merlict_test.cpp`, which can be used to test the project.

You can build it like

```bash
g++ merlict_test.cpp merlict.cpp -o merlict_test
./merlict_test
```


---

# Playing with Python

## TLDR

Do this:

    ./one_source.py && swig -c++ -python -o merlict_wrap.cpp merlict.i && pip install -e .
    pytest

## Longer form

For Python Wrapping, we started with the *two file version of merlict*, i.e.
with `merlict.h` and `merlict.cpp`, this we assume this makes the build
process of the wrapped python code simpler.

So the very first step is, make the two file following the steps in the
chapter above this one.

Currently this is all pretty manual and cumbersome, but we plan to streamline
everything once we think it works more or less.

As a first step, generate the wrapper code using swig
(I use SWIG Version 3.0.12 installed from conda) like this:

    swig -c++ -python -o merlict_wrap.cpp merlict.i

This will generate the files `merlict_wrap.cpp` and the file `merlict.py` from
the so called *interface file* `merlict.i`. You might see a couple of warnings
we are aware of them and working on them.

Do not yet try to use the `merlict.py` file ... it does not yet work. As a
next step we need to compile the `merlict_wrap.cpp` file into a shared object
file. The compiler call for this is a bit cumbersome, since one needs to
find the `Python.h` file for the Python version, one is currently using.
In order to avoid this, we wrote a `setup.py` file for you.

So the next call is just:

    python setup.py build_ext --inplace

You will see a lot of warnings and the last few compilatio or linking steps
take a while ... so one might get the impression something hangs ... just give
it a few seconds. (On my system it takes about 30sec)

The result of this call is a file named like this:

    _merlict.cpython-37m-x86_64-linux-gnu.so

Depending on your python version the name might differ a bit.


Now since we are sure, the build worked and we had no errors, we can install
`merlict` with pip locally to play with it.

    pip install -e .

Now you can execute the python tests by simply typing

    pytest

---

## style
- 80 columns limit
- whitespaces instead of tabulators
- [cpplint](https://github.com/cpplint/cpplint) for C++
- [google's C++ styleguide](https://google.github.io/styleguide/cppguide.html) for e.g. naming
- [pycodestyle](https://pypi.org/project/pycodestyle/) for python
