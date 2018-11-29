from subprocess import Popen, PIPE
import glob
import sys
import unittest
from parameterized import parameterized

parameterized_list = []

for file in glob.glob('semantic_test/*.c'):
    # run compiler
    p = Popen (
        ['compiler467', '-Da', file],
        stdout = PIPE,
        stderr = PIPE
        )
    test_output = p.stderr.read() + p.stdout.read()

    # test against existed output
    with open(file + '.out', 'r') as f:
        record_output = f.read()
        f.closed

    # put into parameterized list
    file = file.replace('semantic_test/', '').replace('.c', '')
    parameterized_list.append ([file, test_output, record_output])




class TestOutput (unittest.TestCase):
    @parameterized.expand(parameterized_list)

    def test(self, name, test_output, recorded_output):
        self.longmessage = True
        self.assertEqual(test_output, record_output, test_output)

        
suite = unittest.TestLoader().loadTestsFromTestCase(TestOutput)
unittest.TextTestRunner(verbosity=5).run(suite)
