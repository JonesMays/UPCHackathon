import serial
import time
import fastapi

app = fastapi.FastAPI()

@app.post("/")
def test():
    ser = serial.Serial('/dev/cu.usbserial-0001', 9600)  # Change '/dev/ttyUSB0' to the appropriate port and 9600 to your baud rate
    ser.write(b'wakeUp\n')
    return True

@app.get("/stop")
def test():
    ser = serial.Serial('/dev/cu.usbserial-0001', 9600)  # Change '/dev/ttyUSB0' to the appropriate port and 9600 to your baud rate
    ser.write(b'shutDown\n')
    return True
# Configure the serial port

# Close the serial port
# ser.close()
