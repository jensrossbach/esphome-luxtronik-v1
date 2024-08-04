# Copyright (c) 2024 Jens-Uwe Rossbach
#
# This code is licensed under the MIT License.
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

# This is a simple simulation of the Luxtronik V1 heating control unit. It simply
# responds to requests with example data.


import serial

def sendTemperatures():
    ser.write(b'1100;12;254;257;261;456;128;480;470;177;201;750;0;0\r\n')

def sendInputs():
    ser.write(b'1200;6;1;1;0;1;1;0\r\n')

def sendOutputs():
    ser.write(b'1300;13;0;0;1;1;0;1;0;0;0;0;0;0;0\r\n')

def sendProcessingTimes():
    ser.write(b'1400;29;0;0;0;0;0;0;0;0;0;0;0;0;0;0;1;1;3;0;0;0;0; 0;38;0;0;0;0;0;0\r\n')

def sendOperationTimes():
    ser.write(b'1450;9;3628317;2161;1678;3121663;2162;1443;372727; 0;6230122\r\n')

def sendErrors():
    ser.write(b'1500;5\r\n')
    ser.write(b'1500;1500;6;709;22;11;8;11;54\r\n')
    ser.write(b'1500;1501;6;715;22;11;8;12;4\r\n')
    ser.write(b'1500;1502;6;709;22;11;8;12;4\r\n')
    ser.write(b'1500;1503;6;711;11;12;8;20;24\r\n')
    ser.write(b'1500;1504;6;708;13;12;8;14;9\r\n')
    ser.write(b'1500;5\r\n')

def sendShutdowns():
    ser.write(b'1600;5\r\n')
    ser.write(b'1600;1600;6;010;19;12;8;12;32\r\n')
    ser.write(b'1600;1601;6;010;19;12;8;14;54\r\n')
    ser.write(b'1600;1602;6;010;19;12;8;17;10\r\n')
    ser.write(b'1600;1603;6;010;19;12;8;19;31\r\n')
    ser.write(b'1600;1604;6;010;19;12;8;21;14\r\n')
    ser.write(b'1600;5\r\n')

def sendInformation():
    ser.write(b'1700;12;14;V2.33;1;0;19;2;22;7;29;22;0;0\r\n')

def parseCommand(cmd):
    print('Parsing command: ' + cmd.decode('ascii'))

    try:
        if cmd.decode('ascii').startswith('1100'):
            sendTemperatures()
        elif cmd.decode('ascii').startswith('1200'):
            sendInputs()
        elif cmd.decode('ascii').startswith('1300'):
            sendOutputs()
        elif cmd.decode('ascii').startswith('1500'):
            sendErrors()
        elif cmd.decode('ascii').startswith('1600'):
            sendShutdowns()
        elif cmd.decode('ascii').startswith('1700'):
            sendInformation()
        elif cmd.decode('ascii').startswith('1800'):
            ser.write(b'1800;8\r\n')
            sendTemperatures()
            sendInputs()
            sendOutputs()
            sendProcessingTimes()
            sendOperationTimes()
            sendErrors()
            sendShutdowns()
            sendInformation()
            ser.write(b'1800;8\r\n')
    except UnicodeDecodeError:
        print('Invalid data received')

ser = serial.Serial(
        port = 'COM3',
        baudrate = 57600,
        bytesize = serial.EIGHTBITS,
        stopbits = 1,
        parity = serial.PARITY_NONE,
        timeout = 10)

try:
    cmd = bytearray()

    while True:
        c = ser.read(1)

        if c:
            if c[0] == 0x0D:
                print('0D:', end = '', flush = True)
            elif c[0] == 0x0A:
                print('0A')
                parseCommand(cmd)
                cmd = bytearray()
            else:
                print(f'{c[0]:02x}' + ':', end = '', flush = True)
                if c[0] < 0x7F:
                    cmd += c
        else:
            print('PING')
except KeyboardInterrupt:
    print('Exiting')
    ser.close()
