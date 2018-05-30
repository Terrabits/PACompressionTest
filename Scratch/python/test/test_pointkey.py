from   ddt         import ddt, data
from   measurement import PointKey
import unittest

@ddt
class TestPointKey(unittest.TestCase):
    @data(*[str(i) for i in PointKey])
    def test_contains(self, key):
        self.assertIn(key, PointKey)
