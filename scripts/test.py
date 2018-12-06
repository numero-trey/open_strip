import serial
from cobs import cobs

ser = serial.Serial('/dev/cu.OpenStrip-ESP32_SPP_SER', timeout=1)

def sendData(serial, values):
    serial.write(cobs.encode(bytes(values)) + b'\x00')

#sendData(ser, [255,0,0,0,0,0,1,0,0,0,255,255])

# Red/Green wipe
sendData(ser, [
    # Dim
    255,
    # Program
    2,
    # Speed
    200,
    # Hold
    0,
    # Position
    127,
    # Fade
    0,
    # NumColors
    2,
    # Option A
    0,
    # Option B
    0,
    # Color 1
    0,255,255,
    # Color 2
    96,255,255
])

# Red/Green dance
sendData(ser, [
    # Dim
    255,
    # Program
    2,
    # Speed
    131,
    # Hold
    0,
    # Position
    127,
    # Fade
    0,
    # NumColors
    2,
    # Option A
    250,
    # Option B
    0,
    # Color 1
    0,255,255,
    # Color 2
    96,255,255
])


# Multi-color scroll
sendData(ser, [
    # Dim
    255,
    # Program
    2,
    # Speed
    130,
    # Hold
    0,
    # Position
    127,
    # Fade
    0,
    # NumColors
    5,
    # Option A
    255,
    # Option B
    0,
    # Color 1
    50,255,255,
    # Color 2
    163,255,255,
    # Color 3
    94,255,255,
    # Color 4
    0,255,255,
    # Color 5
    189,255,255,
    # Color 6
    96,255,255
])
