import inspect
import os
import unittest

import numpy as np

from sampling.sampling import NestedSampling, Uniform, CUniform, Callback


class PyCallback(Callback):

    def __init__(self, data):
        Callback.__init__(self)
        self.data = data

    def set_data(self, data):
        self.data = data

    def run(self, vals):
        x = vals[0]
        y = vals[1]
        N = len(self.data)
        logL = 0
        for k in range(0, N):
            logL += np.log((y / np.pi) /
                           ((self.data[k] - x) * (self.data[k] - x) + y * y))
        return logL


class NestedSamplingTestCase(unittest.TestCase):

    def setUp(self):
        pass

    def test_nested_sampling(self):
        x = CUniform('x', -2., 2.)
        y = CUniform('y', 0., 2.)
        D = [4.73, 0.45, -1.73, 1.09, 2.19, 0.12,
             1.31, 1.00, 1.32, 1.07, 0.86, -0.49, -2.59, 1.73, 2.11,
             1.61, 4.98, 1.71, 2.23, -57.20, 0.96, 1.25, -1.56, 2.45,
             1.19, 2.17, -10.66, 1.91, -4.16, 1.92, 0.10, 1.98, -2.51,
             5.55, -0.47, 1.91, 0.95, -0.78, -0.84, 1.72, -0.01, 1.48,
             2.70, 1.21, 4.41, -4.79, 1.33, 0.81, 0.20, 1.58, 1.29,
             16.19, 2.75, -2.38, -1.79, 6.50, -18.53, 0.72, 0.94, 3.64,
             1.94, -0.11, 1.57, 0.57]

        ns = NestedSampling()
        pycb = PyCallback(D)
        pycb.__disown__()
        ns.setCallback(pycb)
        rs = ns.explore(vars=[x, y], initial_samples=100,
                        maximum_steps=1000)
        ep = rs.getexpt()
        ev = rs.getZ()
        h = rs.getH()
        var = rs.getvar()
        m = rs.getmax()
        self.assertAlmostEqual(ep[0], 1.24208, 5)
        self.assertAlmostEqual(ep[1], 1.00426, 5)
        self.assertAlmostEqual(np.sqrt(var[0]), 0.182403, 6)
        self.assertAlmostEqual(np.sqrt(var[1]), 0.194502, 6)
        self.assertAlmostEqual(m[0], 1.27219, 5)
        self.assertAlmostEqual(m[1], 0.915382, 6)
        self.assertAlmostEqual(m[2], -156.412, 3)
        self.assertAlmostEqual(ev[0], -160.293, 3)
        self.assertAlmostEqual(ev[1], 0.163994, 6)
        self.assertAlmostEqual(h, 2.68939, 5)


def suite():
    return unittest.makeSuite(NestedSamplingTestCase, 'test')

if __name__ == '__main__':
    unittest.main(defaultTest='suite')
