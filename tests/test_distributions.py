import unittest

import numpy as np
from scipy.stats import norm, ksone
from scipy.integrate import trapz, cumtrapz

from nsampling import Uniform, Normal, Constant, InvCDF


class DistributionsTestCase(unittest.TestCase):

    def test_uniform(self):
        x = Uniform('x', -2., 2., seed=42)
        y = x.clone()
        self.assertEqual(x.get_value(), y.get_value())
        self.assertEqual(y.get_name(), x.get_name())
        x.draw()
        self.assertNotEqual(x.get_value(), y.get_value())
        self.assertEqual(x.get_name(), y.get_name())

    def test_normal(self):
        mean = 1.
        sigma = 0.2
        x = Normal('x', mean, sigma, seed=42324411)
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
        self.assertEqual(c.trial(0.1), val)
        self.assertEqual(c.get_value(), val)
        c1 = c.clone()
        self.assertEqual(c1.get_value(), c.get_value())
        self.assertEqual(c1.get_name(), c.get_name())

    def test_infcdf(self):
        x = np.linspace(0, 100, 10000)
        mean = 40.
        std = 5.
        # First test with a Gaussian distribution
        cdf = norm.cdf(x, mean, std)
        icdf = InvCDF('var', x, cdf, seed=42)
        ns = 10000
        rs_cdf = [icdf.draw() for i in range(ns)]
        rs_cdf.sort()
        y = (np.arange(ns)+1)/ns
        v_test = norm.cdf(rs_cdf, mean, std)
        d = np.abs(v_test - y).max()
        self.assertTrue(ksone.cdf(d, ns) < 0.99)

        # Now test with an arbitrary pdf constructed
        # by adding up 2 Gaussian pdfs
        y1 = norm.pdf(x, mean, std)
        y2 = norm.pdf(x, 80, 10)
        y_pdf = y1 + y2
        fact = trapz(y_pdf, x)
        y_pdf /= fact
        y_cdf = cumtrapz(y_pdf, x, initial=0.)
        icdf1 = InvCDF('var1', x, y_cdf, seed=42)
        rs_cdf1 = [icdf1.draw() for i in range(ns)]
        rs_cdf1.sort()
        y1 = (np.arange(ns)+1)/ns
        rmse = np.sqrt(np.sum((y_cdf-y1)**2)/ns)
        self.assertTrue(rmse < 0.135)


def suite():
    return unittest.makeSuite(DistributionsTestCase, 'test')

if __name__ == '__main__':
    unittest.main(defaultTest='suite')
