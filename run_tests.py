import os
import glob
import subprocess


def main():
    root_dir = os.path.dirname(os.path.abspath(__file__))
    tests_dir = os.path.join(root_dir, "tests")
    test_exec_files = glob.glob(os.path.join(tests_dir, "check_*"))
    for test_exec_file in test_exec_files:
        subprocess.check_call(test_exec_file)


if __name__ == "__main__":
    main()

