import serial  # import Serial Library
import numpy  # Import numpy
import matplotlib.pyplot as plt  # import matplotlib library
import time
import datetime
from drawnow import *

#Making a variable for the current count and the max count.
count = 0
maxCount = 50

#Defining the update interval of the program.
update_interval = 0.1
data_array = []

#Setting the length of all of the lists we need.
accel_x = [None] * maxCount
accel_y = [None] * maxCount
accel_z = [None] * maxCount
speed = [None] * maxCount
rpm = [None] * maxCount
throttle = [None] * maxCount

#Setting the graph to interactive.
plt.ion()


#Defining the filename.
currentDateTime = datetime.datetime.now().strftime("%Y-%m-%d_%H-%M-%S")
fileName = (str(currentDateTime)+ "_tele.csv")
#Defining a file, and if it does not already exist (spoiler alert, it won't) then it will make a new file.
dataFile = open(str(fileName), 'w+')
#Writes the first line in the file, creating columns for each data type.
dataFile.write("Y-Accel, X-Accel, Z-Accel, Speed, RPM, Throttle Position \n")

#Setting up Arduino...
ser = serial.Serial('COM4', 115200, timeout=1)
time.sleep(3) #Waiting for the arduino to connect.

#For getting the values from the arduino.
def get_values():
    #Prompting the arduino to send data. Sends 'g' to the arduino.
    ser.write(b'g')
    #Returning the data from the arduino, decoding it to ascii and removing the staring characters associated.
    return ser.readline().decode('ascii').rstrip()


def make_fig():
    plt.figure(1)


    plt.subplot(4 ,1 ,1)                                #Creating a graph for the accelerometer.
    plt.ylim(-4, 4)                                     #Setting the y limits to 4 and -4.
    plt.grid(True)                                      #Enabling the grid.
    plt.xticks([])                                      #Disable X-Axis Tick Marks.
    plt.title('Accelerometer')                          #Setting the title of the graph.
    plt.ylabel('G-Force')                                #Setting a label for the Y-Axis.
    plt.plot(accel_x, '#e74c3c', label='Front/Back')    #Plotting the X-Acceleration, with a color code and a label.
    plt.plot(accel_y, '#2980b9', label='Left/Right')    #Plotting the Y-Acceleration, with a color code and a label.
    plt.plot(accel_z, '#2ecc71', label='Up/Down')       #Plotting the Z-Acceleration, with a color code and a label.
    plt.legend(loc='upper left')                        #Creating a legend for the graph.

    plt.subplot(4,1,2)                                  #Creating a graph for the Speed.
    plt.ylim(0, 280)                                    #Setting the y limits to 0 and 280.
    plt.grid(True)                                      #Enabling the grid.
    plt.xticks([])                                      #Disable X-Axis Tick Marks.
    plt.title('Speed')                                  #Setting the title of the graph.
    plt.ylabel('km/h')                                  #Setting a label for the Y-Axis.
    plt.plot(speed, '#f39c12', label='Speed')           #Plotting the Speed, with a color code and a label.
    plt.legend(loc='upper left')                        #Creating a legend of the graph.

    plt.subplot(4,1,3)                          #Creating a graph for the RPM.
    plt.ylim(0, 8000)                           #Setting the y limits of the graph to 0 to 8000
    plt.grid(True)                              #Enabling the grid.
    plt.xticks([])                              #Disabiling the X-Axis tick marks.
    plt.title('RPM')                            #Setting a title of the graph.
    plt.ylabel('rpm')                           #Setting a title for the Y-Axis.
    plt.plot(rpm, '#8e44ad', label='Speed')     #Plotting the rpm.
    plt.legend(loc='upper left')                #Setting a legend for the graph.

    #Same stuff as before...
    plt.subplot(4, 1, 4)
    plt.ylim(0, 100)
    plt.grid(True)
    plt.xticks([])
    plt.title('Throttle Position')
    plt.ylabel('%')
    plt.plot(throttle, '#2c3e50', label='Throttle Pos')
    plt.legend(loc='upper left')


while 1:
    #Get the values from the arduino, decode them and split them into a list by a key character of '|'.
    data_array = get_values().split('|')
    #print(data_array)

    #Set those variables to the appropriate lists.
    accelxin = data_array[1]
    accelyin = data_array[0]
    accelzin = data_array[2]
    speedin = data_array[4]
    rpmin = data_array[3]
    throttlein = data_array[5]

    #Appending the data to the nessacary lists.
    accel_x.append(float(accelxin))
    accel_y.append(float(accelyin))
    accel_z.append(float(accelzin))
    speed.append(float(speedin))
    rpm.append(float(rpmin))
    throttle.append(float(throttlein))
    #Drawing the graph.
    drawnow(make_fig)
    #Preventing an oof.
    plt.pause(.000001)
    #Increment the count.
    count = count + 1

    #Keeping that list manageable in size.
    if count > maxCount:
        accel_x.pop(0)
        accel_y.pop(0)
        accel_z.pop(0)
        speed.pop(0)
        rpm.pop(0)
        throttle.pop(0)

    #Creating a string to write to the file.
    dataToWrite = str(data_array[0]) + ',' + str(data_array[1]) + ',' + str(data_array[2]) + ',' + str(data_array[4]) + ',' + str(data_array[3]) + ',' + str(data_array[5]) + '\n'
    #Writes to the file.
    dataFile.write(dataToWrite)