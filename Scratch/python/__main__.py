#!/usr/bin/env python
from   measurement import Measurement
import numpy           as np

m = Measurement()

freq_points  = 101
power_points = 31
total_points = freq_points * power_points

freq  = np.linspace(1e9, 2e9, 101)[::-1]
power = np.linspace(-30,   0,  31)[::-1]
s     = [[0,1], [1,0]]

for p in power:
    for f in freq:
        m.add_point(f, p, s)

print(m)
print('first point: {0}'.format(m.points[0]))
print('last point:  {0}'.format(m.points[-1]))

print('\n***sort***\n')

m.sort()
print(m)
print('first point: {0}'.format(m.points[0]))
print('last point:  {0}'.format(m.points[-1]))
