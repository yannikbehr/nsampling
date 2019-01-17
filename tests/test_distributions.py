import unittest

import numpy as np

from sampling import Uniform, Normal, Constant


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

    def test_constant(self):
        val = 6.2
        c = Constant('c', val)
        self.assertEqual(c.draw(), val)
        self.assertEqual(c.draw(0.1), val)
        self.assertEqual(c.get_value(), val)
        c1 = c.clone()
        self.assertEqual(c1.get_value(), c.get_value())
        self.assertEqual(c1.get_name(), c.get_name())


def suite():
    return unittest.makeSuite(DistributionsTestCase, 'test')

if __name__ == '__main__':
    unittest.main(defaultTest='suite')
