# Copyright (c) 2024-2025 Jens-Uwe Rossbach
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

heating_mode = '4'
hot_water_set_temp = '460'
heating_curve = '0;320;220;0;350;350;200;0;350'

def respond(str):
    ser.write(str.encode('ascii'))
    ser.write(b'\r\n')
    print(f'>> {str}')

def echo(input):
    ser.write(input)
    ser.write(b'\r\n')
    print(f'>> {input.decode("ascii")}')

def send_temperatures():
    respond(f'1100;12;267;257;150;276;230;384;{hot_water_set_temp};238;235;750;0;0')

def send_inputs():
    respond('1200;6;0;1;0;1;1;0')

def send_outputs():
    respond('1300;13;0;0;0;0;0;1;0;0;0;0;0;0;0')

def send_processing_times():
    respond('1400;29;0;0;0;0;0;0;0;0;0;0;0;0;0;0;1;1;3;0;0;0;0;0;38;0;0;0;0;0;0')

def send_operation_times():
    respond('1450;9;3628317;2161;1678;3121663;2162;1443;372727;0;6230122')

def sendErrors():
    respond('1500;5')
    respond('1500;1500;6;718;31;7;24;9;28')
    respond('1500;1501;6;718;31;7;24;11;24')
    respond('1500;1502;6;718;31;7;24;14;11')
    respond('1500;1503;6;718;2;8;24;10;23')
    respond('1500;1504;6;718;3;8;24;10;48')
    respond('1500;5')

def send_deactivations():
    respond('1600;5')
    respond('1600;1600;6;010;3;8;24;23;47')
    respond('1600;1601;6;010;4;8;24;23;47')
    respond('1600;1602;6;010;5;8;24;23;48')
    respond('1600;1603;6;010;6;8;24;23;47')
    respond('1600;1604;6;010;7;8;24;23;46')
    respond('1600;5')

def send_information():
    respond('1700;12;12; V2.33;1;5;22;1;24;6;56;44;0;0')

def send_heating_curve(cmd):
    respond(f'{cmd};9;{heating_curve}')

def send_heating_mode(cmd):
    respond(f'{cmd};1;{heating_mode}')

def send_hot_water_set_temp():
    respond(f'3501;1;{hot_water_set_temp}')

def send_hot_water_mode():
    respond('3505;1;0')

def send_store_config_ack():
    respond('993')
    respond('993')
    respond('999')

def parse_command(cmd):
    try:
        cmd_str = cmd.decode('ascii')
        print('Parsing command: ' + cmd_str)

        if cmd_str == '1100':
            send_temperatures()
        elif cmd_str == '1200':
            send_inputs()
        elif cmd_str == '1300':
            send_outputs()
        elif cmd_str == '1400':
            send_processing_times()
        elif cmd_str == '1450':
            send_operation_times()
        elif cmd_str == '1500':
            sendErrors()
        elif cmd_str == '1600':
            send_deactivations()
        elif cmd_str == '1700':
            send_information()
        elif cmd_str == '1800':
            ser.write(b'1800;8\r\n')
            send_temperatures()
            send_inputs()
            send_outputs()
            send_processing_times()
            send_operation_times()
            sendErrors()
            send_deactivations()
            send_information()
            ser.write(b'1800;8\r\n')
        elif cmd_str == '3400':
            send_heating_curve('3400')
        elif cmd_str.startswith('3401'):
            if len(cmd_str) == 4:
                send_heating_curve('3401')
            elif len(cmd_str) > 7:
                global heating_curve
                heating_curve = cmd_str[7:]
                send_heating_curve('3401')
        elif cmd_str == '3405':
            send_heating_mode('3405')
        elif cmd_str.startswith('3406'):
            if len(cmd_str) == 4:
                send_heating_mode('3406')
            elif len(cmd_str) > 7:
                global heating_mode
                heating_mode = cmd_str[7:]
                send_heating_mode('3406')
        elif cmd_str.startswith('3501'):
            if len(cmd_str) == 4:
                send_hot_water_set_temp()
            elif len(cmd_str) > 7:
                global hot_water_set_temp
                hot_water_set_temp = cmd_str[7:]
                send_hot_water_set_temp()
        elif cmd_str == '3505':
            send_hot_water_mode()
        elif cmd_str == '999':
            send_store_config_ack()
    except UnicodeDecodeError:
        print('Invalid data received')

ser = serial.Serial(
        port = 'COM4',
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
                echo(cmd)
                parse_command(cmd)
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
