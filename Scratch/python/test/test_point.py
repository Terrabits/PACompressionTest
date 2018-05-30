from   ddt         import ddt, data
from   measurement import Point, PointKey
import unittest

@ddt
class TestPoint(unittest.TestCase):
    @data({'freq': 1, 'power': 2, 's': [[0,1],[1,0]]})
    def test_init(self, data):
        point = Point(data['freq'],
                      data['power'],
                      data['s'])
        self.assertEqual(point.__getattribute__('freq' ), data['freq' ])
        self.assertEqual(point.__getattribute__('power'), data['power'])
        self.assertEqual(point.__getattribute__('s'    ), data['s'    ])
    @data({'point1': Point(),      'point2': Point()     , 'is_equal': True },
          {'point1': Point(1,2,3), 'point2': Point(1,2,3), 'is_equal': True },
          {'point1': Point(0,2,3), 'point2': Point(1,2,3), 'is_equal': False},
          {'point1': Point(1,0,3), 'point2': Point(1,2,3), 'is_equal': False},
          {'point1': Point(1,2,0), 'point2': Point(1,2,3), 'is_equal': False},
          {'point1': Point(1,2,3), 'point2': Point(0,2,3), 'is_equal': False},
          {'point1': Point(1,2,3), 'point2': Point(1,0,3), 'is_equal': False},
          {'point1': Point(1,2,3), 'point2': Point(1,2,0), 'is_equal': False},
          {'point1': Point(1,2),   'point2': Point(1,2,3), 'is_equal': False},
          {'point1': Point(1,2),   'point2': Point(1,2),   'is_equal': True })
    def test_eq(self, data):
        if data['is_equal']:
            self.assertEqual(data['point1'], data['point2'])
        else:
            self.assertNotEqual(data['point1'], data['point2'])
    @data(*[str(i) for i in PointKey])
    def test_delattr(self, key):
        point = Point(0,1,[[2]])
        del point[key]
        self.assertRaises(KeyError,       point.__getitem__,      key)
        self.assertRaises(AttributeError, point.__getattribute__, key)
