#!/usr/bin/env python3

import numpy as np
import socket

TCP_IP      = '10.0.75.1'
TCP_PORT    = 5025
BUFFER_SIZE = 150

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.bind((TCP_IP, TCP_PORT))
s.setblocking(True)

while True:
    s.listen(1)

    conn, addr = s.accept()
    conn.setblocking(True)
    print('Connected to {0}'.format(addr))

    writes = []
    points = 201
    while True:
        print('Waiting for data')
        data = conn.recv(BUFFER_SIZE)
        data = data.decode('utf-8').strip()
        if not data:
            break
        print('data: {0}'.format(data))
        writes.append(data)
        if data.startswith('SETPOINTS'):
            points = int(data.split(' ')[1])
            print('set points: {0}'.format(points))
        elif 'FETC?' in data:
            print('fetching result')
            response = np.ones(points)
            response = [str(i) for i in response]
            response = ','.join(response)
            conn.sendall('{0}\n'.format(response).encode())
        elif '?' in data:
            print('responding to misc query')
            conn.sendall(b'1\n')
    print('disconnected')
    conn.close()
