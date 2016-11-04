import inspect
import os
import unittest

import numpy as np

from sampling import NestedSampling, Uniform, Callback, d_array


class DistributionsTestCase(unittest.TestCase):

    def test_uniform(self):
        x = Uniform('x', -2., 2., 0.1)
        y = x.clone()
        self.assertEqual(x.get_value(), y.get_value())
        self.assertEqual(y.get_name(), x.get_name())
        x.draw()
        self.assertNotEqual(x.get_value(), y.get_value())
        self.assertEqual(x.get_name(), y.get_name())
        z = Uniform('z', 0, 5., 0.2)
        z.draw()
        print z.get_value(), x.get_value()
        z = x
        print z.get_name(), z.get_value()


def suite():
    return unittest.makeSuite(DistributionsTestCase, 'test')

if __name__ == '__main__':
    unittest.main(defaultTest='suite')
