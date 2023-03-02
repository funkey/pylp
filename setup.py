from setuptools import setup
from setuptools.extension import Extension
from Cython.Build import cythonize

setup(
        name='ilpy',
        version='0.2',
        description='Python wrappers for popular MIP solvers.',
        url='https://github.com/funkelab/ilpy',
        author='Jan Funke',
        author_email='funkej@janelia.hhmi.org',
        license='MIT',
        packages=[
            'ilpy'
        ],
        ext_modules=cythonize([
            Extension(
                'ilpy.wrapper',
                sources=[
                    'ilpy/wrapper.pyx'
                ],
                extra_compile_args=['-O3', '-std=c++11'],
                include_dirs=['ilpy/impl'],
                libraries=['scip'],
                language='c++')
        ])
)
