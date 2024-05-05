import serial
import time
# Configure the serial port
ser = serial.Serial('/dev/cu.usbserial-0001', 9600)  # Change '/dev/ttyUSB0' to the appropriate port and 9600 to your baud rate

ser.setRTS(True)
ser.setDTR(True)

while True:
    b = ser.read_all()
    print(b)
    time.sleep(0.5)
