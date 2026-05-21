import serial
import datetime

# READ SERIAL PRINTOUTS FROM ARDUINO PROGRAM
hwInfo = serial.Serial('/dev/ttyACM0', 9600)

# CONSTANTLY LISTEN TO PORT /dev/ttyACM0
# PRINT ANY INCOMING SIGNALS
while True:
    line = hwInfo.readline().decode().strip()
    timestamp = datetime.datetime.now().strftime("%d-%m-%Y %H:%M:%S")

    print(f"[{timestamp}] {line}")
