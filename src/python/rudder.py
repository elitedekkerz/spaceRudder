#!/usr/bin/env python3
import SDClient
import serial
import logging
import threading
import sys
import time
import numpy

logging.basicConfig(level=logging.INFO)

ser = serial.Serial()
ser.port = '/dev/ttyUSB0'
ser.baudrate = 115200
ser.open()


class rudder():
    def __init__(self, server = 'localhost', ship = 'testship'):
        #configure spacegame client
        c = SDClient.client(server, 'spaceRudder4700', ship)
        self.serverRudder = c.gameVariable(['rudder','yaw'])
        self.position = 0
        #translate maximum steering speed
        self.scalar = (1200/4)

    def update(self,position):
        self.position = numpy.clip(self.position+position,-self.scalar,self.scalar)
        logging.info(self.position/self.scalar)

    def updateLoop(self):
        self.run = True
        while self.run:
            self.serverRudder.parse([str(self.position/self.scalar)])
            self.position = 0
            time.sleep(0.1)

r = rudder()
updateThread = threading.Thread(target=r.updateLoop)
updateThread.start()

try:
    while True:
        position = 0;
        charactersToRead = ser.inWaiting()
        if not charactersToRead:
            charactersToRead = 1
        for char in ser.read(charactersToRead).decode("utf-8"):
            if char == ">":
                position += 1
            elif char == "<":
                position -= 1
        r.update(position)
except KeyboardInterrupt:
    pass

r.run = False
updateThread.join()
