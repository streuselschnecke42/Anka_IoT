import serial
import datetime
import parser

# READ SERIAL PRINTOUTS FROM ARDUINO PROGRAM
hwInfo = serial.Serial('/dev/ttyACM0', 9600)

# CONSTANTLY LISTEN TO PORT /dev/ttyACM0
# PRINT ANY INCOMING SIGNALS
while True: # interrupts instead
    # CRAFT STRING SNIPPETS FOR PARSER
    line = hwInfo.readline().decode().strip()
    timestamp = datetime.datetime.now().strftime("%d-%m-%Y %H:%M:%S")

    # FOR DEBUGGING PURPOSES ONLY
    print(f"[{timestamp}] {line}")

    # PARSE STRING IN JSON FORMAT & FORWARD TO NODE.JS
    data = parser.formatToJson(timestamp, line)
    parser.sendData(data)
