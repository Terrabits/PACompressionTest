import copy
from   decimal import Decimal
from   enum    import Enum, EnumMeta

class PointKeyMeta(EnumMeta):
    def __contains__(self, value):
        if EnumMeta.__contains__(self, value):
            return True
        try:
            self(value)
        except:
            return False
        return True

class PointKey(Enum, metaclass=PointKeyMeta):
    FREQ  = 'freq'
    POWER = 'power'
    S     = 's'
    def __str__(self):
        return self.value
    def __eq__(self, other):
        return str(self) == str(other)
    def __contains__(cls, value):
        try:
            cls(value)
        except:
            return cls.__contains__(self, value)
        return True

class Point(object):
    def __init__(self, freq=None, power=None, s=None):
        if freq  != None:
            self.freq  = freq
        if power != None:
            self.power = power
        if s     != None:
            self.s     = s
    def __getitem__(self, key):
        try:
            return self.__getattribute__(key)
        except:
            raise KeyError(key)
    def __eq__(self, other):
        for i in [str(key) for key in PointKey]:
            if (i in self) != (i in other):
                return False
            elif i in self and self[i] != other[i]:
                return False
        return True
    def __contains__(self, key):
        return key in self.__dict__
    def __delitem__(self, key):
        if not key in self.__dict__:
            raise KeyError(key)
        if key == 'freq':
            del self.freq
        elif key == 'power':
            del self.power
        elif key == 's':
            del self.s
        else:
            raise KeyError(key)
    def __repr__(self):
        params = []
        if 'freq' in self:
            freq   = Decimal(self.freq ).normalize().to_eng_string()
            freq   = 'freq={0}'.format(freq)
            params.append(freq)
        if 'power' in self:
            power = Decimal(self.power).normalize().to_eng_string()
            power = 'power={0}'.format(power)
            params.append(power)
        if 's' in self:
            sparam = 'S[{0}-Port]'.format(len(self.s))
            params.append(sparam)
        repr   = 'point.Point({0})'
        return repr.format(','.join(params))
    def _keys(self):
        return [str(i) for i in PointKey if str(i) in self]
    keys = property(_keys)
    def copy(self):
        return copy.copy(self)
    def copy_except(self, key):
        i = self.copy()
        del i[key]
        return i
    def remove(self, key):
        del self[key]
