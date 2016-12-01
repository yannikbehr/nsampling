import inspect
import os
import unittest

import numpy as np

from sampling import Uniform


class DistributionsTestCase(unittest.TestCase):

    def test_uniform(self):
        x = Uniform('x', -2., 2.)
        y = x.clone()
        self.assertEqual(x.get_value(), y.get_value())
        self.assertEqual(y.get_name(), x.get_name())
        x.draw()
        self.assertNotEqual(x.get_value(), y.get_value())
        self.assertEqual(x.get_name(), y.get_name())


def suite():
    return unittest.makeSuite(DistributionsTestCase, 'test')

if __name__ == '__main__':
    unittest.main(defaultTest='suite')
