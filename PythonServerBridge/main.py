import time
import serial

ser = serial.Serial(
    port='COM5',
    baudrate=115200
)

while True:
    ser.write(bytes('o', "utf-8"))
    print("LED ON")
    time.sleep(0.5)
    ser.write(bytes('f', "utf-8"))
    print("LED OFF")
    time.sleep(0.5)
