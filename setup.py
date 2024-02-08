from setuptools import Extension, setup

setup(
    name='cryptit',
    version='1.0.0',
    ext_modules=[
        Extension(
            name='cryptit',
            sources=['src/cryptit.c'],
            libraries=['crypt']
        )
    ]
)
