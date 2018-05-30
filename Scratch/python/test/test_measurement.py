from   ddt         import ddt, data
from   measurement import Measurement
import numpy           as np
from   point       import Point
import unittest

@ddt
class TestMeasurement(unittest.TestCase):
    def test_sort(self):
        m = Measurement()
        s     = [[0,1],[1,0]]
        freq  = np.linspace(1e9, 2e9, 11)[::-1]
        power = np.linspace(-30,   0, 31)[::-1]
        for p in power:
            for f in freq:
                m.add_point(f, p, s)
        m.sort()
        self.assertEqual(m.points[ 0], Point(1e9,   -30, s))
        self.assertEqual(m.points[31], Point(1.1e9, -30, s))
        self.assertEqual(m.points[61], Point(1.1e9,   0, s))
        self.assertEqual(m.points[-1], Point(2e9,     0, s))


    @data({'points': [Point(1e9, 0, [[0,1],[1,0]]), Point(2e9, 0, [[0,2],[2,0]])],
           'data_at':     ('freq', 1e9),
           'result': [Point(power=0, s=[[0,1],[1,0]])]})
    def test_data_at(self, data):
        m = Measurement(data['points'])
        at_args = data['data_at']
        self.assertEqual(m.data_at(*at_args).points, data['result'])
