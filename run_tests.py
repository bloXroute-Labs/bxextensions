import os
import glob
import subprocess
import unittest
import task_pool_executor as tpe


class TaskPoolExecutorTest(unittest.TestCase):

    def setUp(self):
        self.errors = [
            (tpe.throw_TaskNotExecutedError, tpe.TaskNotExecutedError),
            (tpe.throw_TaskNotCompletedError, tpe.TaskNotCompletedError),
            (tpe.throw_EncryptionError, tpe.EncryptionError),
            (tpe.throw_DecryptionError, tpe.DecryptionError),
            (tpe.throw_InvalidKeyError, tpe.InvalidKeyError),
            (tpe.throw_IndexError, tpe.IndexError),
            (tpe.throw_CryptoInitializationError, tpe.CryptoInitializationError),
            (tpe.throw_AggregatedException, tpe.AggregatedException)
        ]

    def test_raise_errors(self):
        for throw_error, error_type in self.errors:
            self.assertRaises(error_type, throw_error)


def main():
    print("\n\n***********************CPP TESTS***************************\n\n")
    root_dir = os.path.dirname(os.path.abspath(__file__))
    bin_dir = os.path.join(root_dir, "bin")
    test_exec_files = glob.glob(os.path.join(bin_dir, "check_*"))
    for test_exec_file in test_exec_files:
        subprocess.check_call(test_exec_file)
    print("\n\n**********************Python TESTS*************************\n\n")
    unittest.main()


if __name__ == "__main__":
    main()

