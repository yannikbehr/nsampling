import inspect
import os
import unittest

import numpy as np

from sampling import Uniform, Normal


class DistributionsTestCase(unittest.TestCase):

    def test_uniform(self):
        x = Uniform('x', -2., 2.)
        y = x.clone()
        self.assertEqual(x.get_value(), y.get_value())
        self.assertEqual(y.get_name(), x.get_name())
        x.draw()
        self.assertNotEqual(x.get_value(), y.get_value())
        self.assertEqual(x.get_name(), y.get_name())

    def test_normal(self):
        mean = 1.
        sigma = 0.2
        x = Normal('x', mean, sigma)
        y = x.clone()
        self.assertEqual(x.get_value(), y.get_value())
        self.assertEqual(y.get_name(), x.get_name())
        x.draw()
        self.assertNotEqual(x.get_value(), y.get_value())
        self.assertEqual(x.get_name(), y.get_name())
        vals = []
        for i in range(100000):
            x.draw()
            vals.append(x.get_value())
        self.assertTrue(abs(np.mean(vals)-mean) < 0.001)
        self.assertTrue(abs(np.std(vals)-sigma) < 0.001)


def suite():
    return unittest.makeSuite(DistributionsTestCase, 'test')

if __name__ == '__main__':
    unittest.main(defaultTest='suite')
