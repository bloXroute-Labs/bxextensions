import argparse
import os
import sys
import subprocess

DEFAULT_SRC_DIR = os.path.dirname(os.path.abspath(__file__))
DEFAULT_BUILD_DIR = os.path.join(DEFAULT_SRC_DIR, "build")
DEFAULT_OUTPUT_DIR = DEFAULT_SRC_DIR
DEFAULT_EXTENSIONS_FOLDER = os.path.join(DEFAULT_SRC_DIR, "interface", "bxextensions")
DEFAULT_EXTENSIONS_LIST = ";".join(
    filter(lambda x: os.path.isdir(os.path.join(
            DEFAULT_EXTENSIONS_FOLDER,
            x
        )),
        os.listdir(DEFAULT_EXTENSIONS_FOLDER)
    )
)
DEFAULT_BUILD_TYPE = "Release"
VERSION = "1.0"
DEFAULT_RUN_TESTS = True


def str_to_bool(v):
    if v.lower() in ('yes', 'true', 't', 'y', '1'):
        return True
    elif v.lower() in ('no', 'false', 'f', 'n', '0'):
        return False
    else:
        raise argparse.ArgumentTypeError('Boolean value expected.')


def main(src_dir, build_dir, output_dir, extensions_list, build_type, run_tests):
    if not os.path.exists(build_dir):
        os.makedirs(build_dir)
    extensions_dir = os.path.join(src_dir, "interface", "bxextensions")
    ext_module_dirs = ";".join(os.path.join(extensions_dir, ext) for ext in extensions_list)
    cmake_args = [
        "-DCMAKE_LIBRARY_OUTPUT_DIRECTORY={}".format(output_dir),
        "-DCMAKE_RUNTIME_OUTPUT_DIRECTORY={}".format(os.path.join(output_dir, "bin")),
        "-DPYTHON_EXECUTABLE={}".format(sys.executable),
        "-DEXTENSION_MODULES={}".format(ext_module_dirs),
        "-DRUN_TESTS={}".format(run_tests),
        "-DCMAKE_BUILD_TYPE={}".format(build_type)
    ]
    env = os.environ.copy()
    build_args = ["--config", build_type, '--', '-j2']
    env['CXXFLAGS'] = '{} -DVERSION_INFO=\\"{}\\"'.format(env.get('CXXFLAGS', ''), VERSION)
    subprocess.check_call(['cmake', src_dir] + cmake_args, cwd=build_dir, env=env)
    subprocess.check_call(['cmake', '--build', '.'] + build_args, cwd=build_dir)


if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        prog="Bloxroute extension module builder",
        description="building and deploying python extension modules"
    )
    parser.add_argument(
        "--src-dir",
        help="the source code root directory (default: {})".format(DEFAULT_SRC_DIR),
        type=str,
        default=DEFAULT_SRC_DIR
    )
    parser.add_argument(
        "--build-dir",
        help="the build directory (default: {})".format(DEFAULT_BUILD_DIR),
        type=str,
        default=DEFAULT_BUILD_DIR
    )
    parser.add_argument(
        "--output-dir",
        help="the build output directory (default: {})".format(DEFAULT_OUTPUT_DIR),
        type=str,
        default=DEFAULT_OUTPUT_DIR
    )
    parser.add_argument(
        "--extensions-list",
        help="a list of which extensions to build (default: {})".format(DEFAULT_EXTENSIONS_LIST),
        type=str,
        default=DEFAULT_EXTENSIONS_LIST
    )
    parser.add_argument(
        "--build-type",
        help="The build type (either Debug or Release, default: {})".format(DEFAULT_BUILD_TYPE),
        type=str,
        default=DEFAULT_BUILD_TYPE
    )
    parser.add_argument(
        "--run-tests",
        help="if true than tests will be ran as a part of the build process (default: {})".format(DEFAULT_RUN_TESTS),
        type=str_to_bool,
        default=DEFAULT_RUN_TESTS
    )
    args = parser.parse_args()

    main(
        args.src_dir,
        args.build_dir,
        args.output_dir,
        args.extensions_list.split(";"),
        args.build_type,
        args.run_tests
    )
