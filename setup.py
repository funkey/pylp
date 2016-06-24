#!/usr/bin/env python

import os
import glob
from subprocess import call
from distutils.core import setup
from distutils.command.build_py import build_py

class copy_lib(build_py):

    def run(self):

        # TODO: change based on architecture
        lib_extensions = [ ".so", ".dylib", ".dll" ]

        module_name = "pylp"
        build_dir   = os.path.abspath(".")
        lib_dir     = os.path.join(build_dir, "python")
        lib_base    = os.path.join(lib_dir, module_name)
        lib_file    = [ lib_base + e for e in lib_extensions if os.path.isfile(lib_base + e) ][0]

        if not self.dry_run:

            target_dir = self.build_lib
            print "target_dir: " + target_dir
            module_dir = os.path.join(target_dir, module_name)

            # make sure the module dir exists
            self.mkpath(module_dir)

            # copy our library to the module dir
            self.copy_file(lib_file, module_dir)

        # run parent implementation
        build_py.run(self)

setup(
    name='pylp',
    version='0.0.1',
    author='Jan Funke',
    author_email='jfunke@iri.upc.edu',
    description='Python interface to popular linear program solvers.',
    packages=['pylp'],
    cmdclass={'build_py' : copy_lib}
)
