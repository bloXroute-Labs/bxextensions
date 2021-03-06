import os
import re
import sys
import platform
import subprocess
import json

from setuptools import setup, Extension
from setuptools.command.build_ext import build_ext
from distutils.version import LooseVersion

VERSION = "v0.0.0.0"
MANIFEST_FILE_NAME = "MANIFEST.MF"


def get_version(src_dir):
    manifest_path = os.path.join(src_dir, "release", MANIFEST_FILE_NAME)
    try:
        with open(manifest_path) as manifest_file:
            manifest_data = json.load(manifest_file)
        return manifest_data["source_version"]
    except (IOError, json.JSONDecodeError):
        return VERSION


class CMakeExtension(Extension):
    def __init__(self, name, sourcedir=''):
        Extension.__init__(self, name, sources=[])
        self.sourcedir = os.path.abspath(sourcedir)


class CMakeBuild(build_ext):
    def run(self):
        try:
            out = subprocess.check_output(['cmake', '--version'])
        except OSError:
            raise RuntimeError("CMake must be installed to build the following extensions: " +
                               ", ".join(e.name for e in self.extensions))

        if platform.system() == "Windows":
            cmake_version = LooseVersion(re.search(r'version\s*([\d.]+)', out.decode()).group(1))
            if cmake_version < '3.1.0':
                raise RuntimeError("CMake >= 3.1.0 is required on Windows")

        self.build()

    def build(self):
        srcdir = os.path.abspath("")
        ext_modules_root = os.path.join("interface", "bxextensions")
        ext_module_dirs = ""
        for ext in self.extensions:
            ext_module_dirs += os.path.join(ext_modules_root, ext.name)
            ext_module_dirs += ";"
        ext_module_dirs = ext_module_dirs[:-1]
        cmake_args = [
            "-DCMAKE_INSTALL_PREFIX={}".format(srcdir),
            "-DPYTHON_EXECUTABLE={}".format(sys.executable),
            "-DCMAKE_INSTALL_RPATH_USE_LINK_PATH=FALSE",
            "-DCMAKE_CXX_STANDARD=17",
            "-DEXTENSION_MODULES={}".format(ext_module_dirs),
            "-DRUN_TESTS=TRUE",
            "-DCMAKE_INSTALL_RPATH={};{}".format(srcdir, os.path.join(srcdir, "tests")),
            "-DINSTALL_TESTS=FALSE"
        ]
        cfg = 'Debug' if self.debug else 'Release'
        build_args = []
        if platform.system() == "Windows":
            cmake_args += ['-DCMAKE_INSTALL_PREFIX_{}={}'.format(cfg.upper(), srcdir)]
            if sys.maxsize > 2**32:
                cmake_args += ['-A', 'x64']
        else:
            cmake_args += ['-DCMAKE_BUILD_TYPE=' + cfg]
            build_args += ['-j2']
        env = os.environ.copy()
        env['CXXFLAGS'] = '{} -DVERSION_INFO=\\"{}\\"'.format(env.get('CXXFLAGS', ''),
                                                              self.distribution.get_version())
        if not os.path.exists(self.build_temp):
            os.makedirs(self.build_temp)
        
        subprocess.check_call(['cmake', srcdir] + cmake_args, cwd=self.build_temp, env=env)
        subprocess.check_call(['make', 'install'] + build_args, cwd=self.build_temp)


setup(
    name="bxextensions",
    version=get_version(os.path.abspath("")),
    author="Avraham Mahfuda",
    author_email="avraham.mahfuda@bloxroute.com",
    description="bloXroute c++ python extension modules",
    long_description="",
    ext_modules=[
        CMakeExtension(name="task_pool_executor"),
        CMakeExtension(name="template_module")
    ],
    cmdclass=dict(build_ext=CMakeBuild),
    zip_safe=False,
)
