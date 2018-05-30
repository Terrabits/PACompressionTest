import copy
import operator
from   point import Point, PointKey


class Measurement:
    def __init__(self, points=[]):
        self.points = points
    def __repr__(self):
        if not self.points or not self.keys:
            return '<measurement.Measurement 0 points>'
        params = self.keys
        if 's' in params:
            params.remove('s')
            sparam = 'S[{0}-Port]'.format(len(self.points[0].s))
            params.append(sparam)
        params = ','.join(params)
        repr   = '<measurement.Measurement {0}, {1} points>'
        return repr.format(params, len(self.points))



    def _freqs(self):
        return self.get_values_by_key('freq')
    freqs = property(_freqs)
    def _powers(self):
        return self.get_values_by_key('power')
    powers = property(_powers)
    def _s(self):
        return self.get_values_by_key('s')
    s = property(_s)

    def _keys(self):
        if not self.points:
            return ()
        else:
            return self.points[0].keys
    keys = property(_keys)



    def get_values_by_key(self, key):
        assert key in PointKey
        all    = [point[key] for point in self.points]
        unique = []
        for i in all:
            if not i in unique:
                unique.append(i)
        unique.sort()
        return unique
    def data_at(self, key, value):
        assert key in self.keys
        points = [i.copy_except(key) for i in self.points if i[key] == value]
        return Measurement(points)
    def add_point(self, *args):
        if isinstance(args[0], Point):
            self.points.append(arg[0])
        else:
            self.points.append(Point(*args))
    def sort(self, *args):
        if not self.points:
            return
        if args:
            keys = args[0]
            assert keys
        else:
            keys = self.keys
        for key in keys:
            if not key in self.keys:
                raise AttributeError(key)
        self.points = sorted(self.points, key=operator.itemgetter(*keys))
