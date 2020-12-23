from setuptools import setup
from setuptools.extension import Extension
from Cython.Build import cythonize

setup(
        name='pylp',
        version='0.1',
        description='Python wrappers for popular MIP solvers.',
        url='https://github.com/funkey/pylp',
        author='Jan Funke',
        author_email='funkej@janelia.hhmi.org',
        license='MIT',
        packages=[
            'pylp'
        ],
        ext_modules=cythonize([
            Extension(
                'pylp.wrapper',
                sources=[
                    'pylp/wrapper.pyx'
                ],
                extra_compile_args=['-O3', '-std=c++11'],
                include_dirs=['pylp/impl'],
                libraries=['gurobi80'],
                language='c++')
        ])
)
