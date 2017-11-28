#!/usr/bin/env python3
import SDClient
import serial
import logging
import sys

logging.basicConfig(level=logging.INFO)

ser = serial.Serial()
ser.timeout = 0
ser.port = '/dev/ttyUSB0'
ser.baudrate = 115200
ser.open()

#configure spacegame client
c = SDClient.client('localhost', 'spaceRudder4700', 'testship')
rudder = c.gameVariable(['rudder','yaw'])

while True:
    position = 0;
    for char in ser.read(ser.inWaiting()).decode("utf-8"):
        if char == ">":
            position += 1
        elif char == "<":
            position -= 1
    rudder.parse([str(position)])
    logging.info(position)
