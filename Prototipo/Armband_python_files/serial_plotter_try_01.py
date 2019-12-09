# -*- coding: utf-8 -*-
import serial # import Serial Library
import numpy  # Import numpy
import matplotlib.pyplot as plt #import matplotlib library
from drawnow import *
 
sensor_01 = []
sensor_02 =[]
arduinoData = serial.Serial('COM11', 115200) #Creating our serial object named arduinoData
plt.ion() #Tell matplotlib you want interactive mode to plot live data
cnt=0
 
def makeFig(): #Create a function that makes our desired plot
    plt.ylim(0,10000)                                 #Set y min and max values
    plt.title('My Live Streaming Sensor Data')      #Plot the title
    plt.grid(True)                                  #Turn the grid on
    plt.ylabel('Sensor 01')                            #Set ylabels
    plt.plot(sensor_01, 'ro', label='sEMG1')       #plot the temperature
    plt.legend(loc='upper left')                    #plot the legend
    plt2=plt.twinx()                                #Create a second y axis
    plt.ylim(0,10000)                           #Set limits of second y axis- adjust to readings you are getting
    plt2.plot(sensor_02, 'b^', label='sEMG2') #plot sensor_02 data
    plt2.set_ylabel('Sensor 02')                    #label second y axis
    plt2.ticklabel_format(useOffset=False)           #Force matplotlib to NOT autoscale y axis
    plt2.legend(loc='upper right')                  #plot the legend
    
 
while True: # While loop that loops forever
    while (arduinoData.inWaiting()==0): #Wait here until there is data
        pass #do nothing
    try:
        arduinoString =  arduinoData.readline().rstrip().decode() #read the line of text from the serial port
        dataArray = arduinoString.split(';')   #Split it into an array called dataArray
        semg1 = float( dataArray[0])            #Convert first element to floating number and put in temp
        semg2 =    float( dataArray[1])            #Convert second element to floating number and put in P
        sensor_01.append(semg1)                     #Build our sensor_01 array by appending temp readings
        sensor_02.append(semg2)                     #Building our sensor_02 array by appending P readings
        print(semg1)
        print(semg2)
        #drawnow(makeFig)                       #Call drawnow to update our live graph
        #plt.pause(.00000001)                     #Pause Briefly. Important to keep drawnow from crashing
        cnt=cnt+1
        if(cnt>50):                            #If you have 50 or more points, delete the first one from the array
            sensor_01.pop(0)                       #This allows us to just see the last 50 data points
            sensor_02.pop(0)
    except ValueError:
        continue