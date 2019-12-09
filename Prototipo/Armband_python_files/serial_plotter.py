# -*- coding: utf-8 -*-

import serial
import numpy as np
from matplotlib import pyplot as plt

try:
    ser = serial.Serial('COM11',115200)  # open serial port
    ser.is_open
    line = ser.readline().rstrip().decode()
    sen1,sen2 = line.split(';')
    sensor_01 = float(sen1)
    sensor_02 = float(sen2)
    print(sensor_01)
    print(sensor_02)
    
except ValueError:
    ser.close()

ser.close()