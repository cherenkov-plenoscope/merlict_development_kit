from distutils.core import setup, Extension

setup(
    name='merlict',
    version='0.1',
    author="Sebastian Müller et al",
    description="merlict-raytracer",
    ext_modules=[
        Extension(
            '_merlict',
            sources=[
                'merlict_wrap.cpp',
                'merlict.cpp'
            ],
        )
    ],
    py_modules=["merlict"],
)
