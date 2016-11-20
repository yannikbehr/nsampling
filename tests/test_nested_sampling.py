import inspect
import os
import unittest

import numpy as np

from sampling import NestedSampling, Uniform, CUniform, Callback, d_array


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
        # print "Python callback with x={}, y={}, and likelihood={}".format(x,
        # y, logL)
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

        npts = len(D)
        d = d_array(npts)         # Array of 10-million integers
        for i in xrange(len(D)):        # Set some values
            d[i] = D[i]
        ns = NestedSampling(vars=[x, y])
        pycb = PyCallback(D)
        pycb.__disown__()
        ns.setCallback(pycb)
        rs = ns.explore(vars=[x, y], initial_samples=100,
                        maximum_steps=1000)
        rs.summarize()
        # Final maximum should be around x=1.26, y=0.93
        # E[x] = 1.24 Var[x] = 0.18^2; E[y]=1.00 Var[y] = 0.19^2


def suite():
    return unittest.makeSuite(NestedSamplingTestCase, 'test')

if __name__ == '__main__':
    unittest.main(defaultTest='suite')
