import time
import serial
import tkinter as tk

ser = serial.Serial(
    port='COM5',
    baudrate=115200
)

def green_click():
    ser.write(b"g")

def red_click():
    ser.write(b"r")

def blue_click():
    ser.write(b"b")

def layout_click():
    ser.write(b"j")

def update_clock():
    # Execute your logic here
    while ser.in_waiting > 0:
        # Read a line and decode from bytes to string
        line = ser.readline().decode('utf-8').rstrip()
        print(line)

    root.after(1000, update_clock) # Schedule itself again in 1 second
    

root = tk.Tk()
root.title("Send green")
button = tk.Button(root, text="green", command=green_click)
button.pack(pady=20)
button2 = tk.Button(root, text="red", command=red_click)
button2.pack(pady=5)
button3 = tk.Button(root, text="blue", command=blue_click)
button3.pack(pady=5)
button4 = tk.Button(root, text="mesh layout", command=layout_click)
button4.pack(pady=5)
update_clock()
root.mainloop()


