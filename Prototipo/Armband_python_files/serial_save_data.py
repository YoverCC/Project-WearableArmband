# -*- coding: utf-8 -*-
"""
Abstract: Serial read and save to .txt
@author: Yover Castro Campos
@email: ycastroc
"""

import serial # import Serial Library
from time import sleep
import pandas as pd

sensor_01 = []
sensor_02 =[]
arduinoData = serial.Serial('COM11', 115200,timeout=1, xonxoff=False, rtscts=False, dsrdtr=False) #Creating our serial object named arduinoData
arduinoData.flushInput()
arduinoData.flush()
arduinoData.flushOutput()
sleep(.1)
cnt=0
 
while True: # While loop that loops forever
    while (arduinoData.inWaiting()==0): #Wait here until there is data
        pass #do nothing
        
    bytesToRead = arduinoData.inWaiting()
    arduinoString = arduinoData.read(bytesToRead)
    
  #  try:
    if(cnt==0):
        arduinoString =  arduinoData.readline()
        print("Init reading")
     #read the line of text from the serial port
    
    if(cnt>1):
        arduinoString =  arduinoString.rstrip().decode()
        dataArray = arduinoString.split(';')   #Split it into an array called dataArray
        semg1 = float( dataArray[0])            #Convert first element to floating number and put in temp
        semg2 = float( dataArray[1])            #Convert second element to floating number and put in P
        sensor_01.append(semg1)                     #Build our sensor_01 array by appending temp readings
        sensor_02.append(semg2)                     #Building our sensor_02 array by appending P readings
        if(cnt==2000):
            print("Prepare")
        if(cnt==3000):
            print("GO")            
        if(cnt>3000):
            print(arduinoString)        
        if(cnt>8000):                            #If you have 50 or more points, delete the first one from the array
#            with open('semg1.txt', 'w') as output1:
#                output1.write(sensor_01)
#            with open('semg2.txt', 'w') as output2:
#                output2.write(sensor_02)
            arduinoData.close()
            break
    cnt=cnt+1

   # except ValueError:
   #    continue
    

'''
NOTA: El codigo no captura correctamente, falta corregir el problema del buffer
Este problema genera que en algunos casos no lea correctamente y no ejecuta los
codigos dentro de try.
'''