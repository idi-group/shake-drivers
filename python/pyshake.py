# Copyright (c) 2006-2015, University of Glasgow
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without modification, are
# permitted provided that the following conditions are met:
#
#    * Redistributions of source code must retain the above copyright notice, this list of 
#           conditions and the following disclaimer.
#    * Redistributions in binary form must reproduce the above copyright notice, this list
#           of conditions and the following disclaimer in the documentation and/or other
#           materials provided with the distribution.
#    * Neither the name of the University of Glasgow nor the names of its contributors 
#           may be used to endorse or promote products derived from this software without
#           specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY 
# EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
# MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
# THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
# SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
# OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
# HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
# OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
# SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


import atexit, thread, re, string, os
from time import sleep

from pyshake_constants import *
import pyshake_sk6 
import pyshake_sk7
import pyshake_serial_pc as pyshake_serial

class shake_error(Exception):
    def __init__(self, value):
        self.value = value
    def __str__(self):
        return repr(self.value)

#       Debugging stuff
debugging = False
def debug(str, opennew=False):
    if not debugging:
        return

    f = None
    if opennew:
        f = open("c:/debug.txt", "w")
    else:
        f = open("c:/debug.txt", "a")

    f.write(str + "\n")
    f.close()

# this function is registered to be called on exit from a Python 
# shell, and just calls the close method of any active shake_device
# instances
def cleanup():
    for i in shake_device.instances:
        if i != None:
            i.close()

atexit.register(cleanup)

# an instance of this class represents a single SHAKE device
class shake_device:
    shake_handle_count      = 0
    instances = []

    #
    #       Constructor
    #

    #       2nd parameter indicates type of device (SK6 or SK7). Default is SK6
    def __init__(self, type = SHAKE_SK7):

        self.handle = shake_device.shake_handle_count
        shake_device.shake_handle_count += 1

        self.device_type = type
        self.port = None
        self.thread = None

        self.waiting_for_ack = False
        self.waiting_for_ack_signal = False
        self.serial = "missing"
        self.fwrev = -1
        self.hwrev = -1
        self.lastfid = ""
        self.bluetoothfwrev = -1

        if type == SHAKE_SK6:
            self.modules = [SK6_MODULE_NONE for x in range(2)]
            self.SHAKE = pyshake_sk6.SK6(self, type)
        else:
            self.modules = [SK7_MODULE_NONE for x in range(5)]
            self.SHAKE = pyshake_sk7.SK7(self, type)

        self.checksum = False
        self.navcb = None
        self.data_callback = None

        self.data_recv = 0L
        self.wait_for_acks = True

        self.lastack = False
        self.lastaddr, self.lastval = 0,0
        self.ack_timeout_ms = 500

        self.lastevent = 0
        self.logfp = None
        self.packets_read = 0L
        self.peek_flag = False
        self.peek = 0

        self.device_address = None
        self.write_to_port = None
        self.thread_done = True
        self.thread_exit = False
        self.synced = False

        shake_device.instances.append(self)

    #
    #       Connect/disconnect functions
    #
    def connect(self, addr):
        if not self.thread_done or addr == None:
            return False

        self.device_address = addr
        if self.port_setup() == SHAKE_ERROR:
            return False

        self.thread_done = False
        self.thread_exit = False
        thread.start_new_thread(self.run, ())
        #self.run()

        elapsed = 0
        while elapsed < 10.0 and not self.synced:
            sleep(0.01)
            elapsed += 0.01

        if self.thread_done:
            return False

        return self.synced

    #       Closes the connection associated with the instance
    def close(self):
        self.thread_done = True

        try:
            shake_device.instances.remove(self)
        except:
            pass

    #
    #       Read/parse data 
    #
    def read_data(self, num_bytes):
        if self.port == None:
            return None

        bytes = ""
        if self.peek_flag:
            bytes += chr(self.peek)
            num_bytes -= 1
            self.peek_flag = False

            if num_bytes == 0:
                return bytes

        self.synced = True
        allbytes = bytes + self.port.read(num_bytes)
        return allbytes

    def port_setup(self):
        # create the port (this is done inside the thread started by connect())
        try:
            self.port = pyshake_serial.serial_port(self.device_address)
            baud = 230400
            if self.device_type == SHAKE_SK7:
                baud = 460800

            if not self.port.open(baud):
                debug("port creation failed")
                self.thread_done = True
                return SHAKE_ERROR

            self.write_to_port = self.port.write
        except pyshake_serial.pyshake_serial_error:
            debug('error; port creation failed')
            self.thread_done = True
            return SHAKE_ERROR

    #       Thread function
    def run(self):
        try:
            self.thread_done = False

            while not self.thread_done:
                packet_type = SHAKE_BAD_PACKET

                while not self.thread_done and packet_type == SHAKE_BAD_PACKET:
                    (packet_type, packet) = self.SHAKE.get_next_packet()

                if self.thread_done:
                    break

                self.SHAKE.parse_packet(packet, packet_type)

            if self.port != None:
                self.port.close()
            self.thread_exit = True
        except:
            import sys, traceback
            print("\n".join(traceback.format_exception(*sys.exc_info())))
            return

    #
    #       Data access functions
    #

    def data_timestamp(self, sensor):
        if sensor < SHAKE_SENSOR_ACC or sensor > SHAKE_SENSOR_ANA1:
            return -1

        return self.SHAKE.data.timestamps[sensor]

    #       Returns x-axis accelerometer reading
    def accx(self):
        x = self.SHAKE.data.accx
        self.SHAKE.data.timestamps[SHAKE_SENSOR_ACC] = self.SHAKE.data.internal_timestamps[SHAKE_SENSOR_ACC]
        return x

    #       Returns y-axis accelerometer reading
    def accy(self):
        y = self.SHAKE.data.accy
        self.SHAKE.data.timestamps[SHAKE_SENSOR_ACC] = self.SHAKE.data.internal_timestamps[SHAKE_SENSOR_ACC]
        return y

    #       Returns z-axis accelerometer reading
    def accz(self):
        z = self.SHAKE.data.accz
        self.SHAKE.data.timestamps[SHAKE_SENSOR_ACC] = self.SHAKE.data.internal_timestamps[SHAKE_SENSOR_ACC]
        return z

    #       Returns accelerometer readings in a list: [x, y, z]
    def acc(self):
        xyz = [self.SHAKE.data.accx, self.SHAKE.data.accy, self.SHAKE.data.accz]
        self.SHAKE.data.timestamps[SHAKE_SENSOR_ACC] = self.SHAKE.data.internal_timestamps[SHAKE_SENSOR_ACC]
        return xyz

    #       Returns x-axis gyro reading
    def gyrx(self):
        x = self.SHAKE.data.gyrx
        self.SHAKE.data.timestamps[SHAKE_SENSOR_GYRO] = self.SHAKE.data.internal_timestamps[SHAKE_SENSOR_GYRO]
        return x

    #       Returns y-axis gyro reading
    def gyry(self):
        y = self.SHAKE.data.gyry
        self.SHAKE.data.timestamps[SHAKE_SENSOR_GYRO] = self.SHAKE.data.internal_timestamps[SHAKE_SENSOR_GYRO]
        return y

    #       Returns z-axis gyro reading
    def gyrz(self):
        z = self.SHAKE.data.gyrz
        self.SHAKE.data.timestamps[SHAKE_SENSOR_GYRO] = self.SHAKE.data.internal_timestamps[SHAKE_SENSOR_GYRO]
        return z

    #       Returns gyro readings in a list: [x, y, z]
    def gyro(self):
        xyz = [self.SHAKE.data.gyrx, self.SHAKE.data.gyry, self.SHAKE.data.gyrz]
        self.SHAKE.data.timestamps[SHAKE_SENSOR_GYRO] = self.SHAKE.data.internal_timestamps[SHAKE_SENSOR_GYRO]
        return xyz

    #       Returns x-axis mag reading
    def magx(self):
        x = self.SHAKE.data.magx
        self.SHAKE.data.timestamps[SHAKE_SENSOR_MAG] = self.SHAKE.data.internal_timestamps[SHAKE_SENSOR_MAG]
        return x

    #       Returns y-axis mag reading
    def magy(self):
        y = self.SHAKE.data.magy
        self.SHAKE.data.timestamps[SHAKE_SENSOR_MAG] = self.SHAKE.data.internal_timestamps[SHAKE_SENSOR_MAG]
        return y

    #       Returns z-axis mag reading
    def magz(self):
        z = self.SHAKE.data.magz
        self.SHAKE.data.timestamps[SHAKE_SENSOR_MAG] = self.SHAKE.data.internal_timestamps[SHAKE_SENSOR_MAG]
        return z

    #       Returns mag readings in a list: [x, y, z]
    def mag(self):
        xyz = [self.SHAKE.data.magx, self.SHAKE.data.magy, self.SHAKE.data.magz]
        self.SHAKE.data.timestamps[SHAKE_SENSOR_MAG] = self.SHAKE.data.internal_timestamps[SHAKE_SENSOR_MAG]
        return xyz

    #       Returns heading
    def heading(self):
        heading = self.SHAKE.data.heading
        self.SHAKE.data.timestamps[SHAKE_SENSOR_HEADING] = self.SHAKE.data.internal_timestamps[SHAKE_SENSOR_HEADING]
        return heading

    #       [SK6] Returns proximity value for cap switch 0
    def sk6_cap0(self):
        c = self.SHAKE.data.cap_sk6[0]
        self.SHAKE.data.timestamps[SHAKE_SENSOR_CAP0] = self.SHAKE.data.internal_timestamps[SHAKE_SENSOR_CAP0]
        return c

    #       [SK6] Returns proximity value for cap switch 1
    def sk6_cap1(self):
        c = self.SHAKE.data.cap_sk6[1]
        self.SHAKE.data.timestamps[SHAKE_SENSOR_CAP1] = self.SHAKE.data.internal_timestamps[SHAKE_SENSOR_CAP1]
        return c

    #       [SK6] Returns proximity values for cap switch [0, 1]
    #       [SK7] Returns proximity values for cap switches [0, 1, ..., 11]
    def cap(self):
        if self.device_type == SHAKE_SK6:
            cap = self.SHAKE.data.cap_sk6
            self.SHAKE.data.timestamps[SHAKE_SENSOR_CAP0] = self.SHAKE.data.cap0seq
            self.SHAKE.data.timestamps[SHAKE_SENSOR_CAP1] = self.SHAKE.data.cap1seq
        else:
            cap = self.SHAKE.data.cap_sk7[0]
            self.SHAKE.data.timestamps[SHAKE_SENSOR_CAP] = self.SHAKE.data.internal_timestamps[SHAKE_SENSOR_CAP]
        return cap

    #       [SK7] Returns proximity values for external capacitive board attached to an SK7
    #           Blocks parameter can be 0, 1 or 2. 0 = return first 12 values, 1 = return second 12 values, 2 = return all 24 values
    def cap_ext(self, blocks):
        if blocks == 0:
            return self.SHAKE.data.cap_sk7[1]
        elif blocks == 1:
            return self.SHAKE.data.cap_sk7[2]
        elif blocks == 2:
            return self.SHAKE.data.cap_sk7[1] + self.SHAKE.data.cap_sk7[2]
        else:
            return None

    #       Returns value of analog input 0
    def analog0(self):
        a = self.SHAKE.data.ana0
        self.SHAKE.data.timestamps[SHAKE_SENSOR_ANA0] = self.SHAKE.data.internal_timestamps[SHAKE_SENSOR_ANA0]
        return a

    #       Returns value of analog input 1
    def analog1(self):
        a = self.SHAKE.data.ana1
        self.SHAKE.data.timestamps[SHAKE_SENSOR_ANA1] = self.SHAKE.data.internal_timestamps[SHAKE_SENSOR_ANA1]
        return a

    #       Returns value of both analog inputs as [analog0, analog1]
    def analog(self):
        ana = [self.SHAKE.data.ana0, self.SHAKE.data.ana1]
        self.SHAKE.data.timestamps[SHAKE_SENSOR_ANA0] = self.SHAKE.data.internal_timestamps[SHAKE_SENSOR_ANA0]
        self.SHAKE.data.timestamps[SHAKE_SENSOR_ANA1] = self.SHAKE.data.internal_timestamps[SHAKE_SENSOR_ANA1]
        return ana

    def shaking_peakaccel(self):
        return self.SHAKE.data.peakaccel

    def shaking_direction(self):
        return self.SHAKE.data.direction

    def shaking_timestamp(self):
        return self.SHAKE.data.timestamp

    #       [SK7] return [roll, pitch, heading] data 
    def sk7_roll_pitch_heading(self):
        return self.SHAKE.data.rph

    def sk7_roll_pitch_heading_quaternions(self):
        return self.SHAKE.data.rphq

    def sk7_configure_roll_pitch_heading(self, val):
        return self.write(SK7_NV_REG_RPH_CONFIG, val)

    def register_data_callback(self, callback):
        self.data_callback = callback

    #
    #       Device information
    #

    def info_retrieve(self):
        if self.fwrev == None:
            if self.write_data_request(1) == SHAKE_ERROR:
                return SHAKE_ERROR
            count = 0

            while count < 1000 and self.fwrev == None:
                count+=1
                sleep(0.001)
        return SHAKE_SUCCESS

    def info_firmware_revision(self):
        self.info_retrieve()
        return self.fwrev

    def info_hardware_revision(self):
        self.info_retrieve()
        return self.hwrev

    def info_serial_number(self):
        self.info_retrieve()
        return self.serial

    def info_module(self, slotnumber):
        self.info_retrieve()
        if slotnumber < 0 or slotnumber > len(self.modules):
            return None
        return self.modules[slotnumber]

    def info_module_name(self, module):
        # TODO supposed to receive an int enum value, but haven't got
        # that enum completed yet so just return raw string from device 
        if isinstance(module, str):
            return module

        for i in range(SK6_MODULE_NONE, SK6_MODULE_LAST, 1):
            if module == i:
                return pyshake_sk6.SK6_modules[i]
        for i in range(SK7_MODULE_NONE, SK7_MODULE_LAST, 1):
            if module == i:
                return pyshake_sk7.SK7_modules[i-SK7_MODULE_NONE]
        return "unknown module type"

    def info_bluetooth_firmware_revision(self):
        self.info_retrieve()
        return self.bluetoothfwrev

    #
    #       Register access
    #

    #       Reads register <address> and returns a 2-tuple: (success/error, value), where
    #       the first item in the tuple is SHAKE_SUCCESS or SHAKE_ERROR depending on the
    #       result of the operation. value will be 0 on error, otherwise it will be the
    #       current contents of the specified register.
    def read(self, address):
        scp = '$REA,%04X,00' % (address)

        if self.waiting_for_ack:
            return SHAKE_ERROR

        self.write_to_port(scp)
        debug("SENT: " + scp)

        self.waiting_for_ack_signal = True
        self.waiting_for_ack = True

        timeout = self.ack_timeout_ms
        while self.waiting_for_ack_signal:
            sleep(0.001)
            timeout -= 1
            if timeout == 0:
                break

        self.waiting_for_ack = False

        if not self.lastack:
            return (SHAKE_ERROR, 0)

        self.lastack = False

        return (SHAKE_SUCCESS, self.lastval)

    #       Writes <value> into register <address> and returns either SHAKE_ERROR or
    #       SHAKE_SUCCESS to indicate the result.
    def write(self, address, value):
        scp = '$WRI,%04X,%02X' % (address, value)

        if self.waiting_for_ack:
            debug("write() already waiting for ack")
            return SHAKE_ERROR

        self.write_to_port(scp)
        debug("+++ SENT: " + scp)

        self.waiting_for_ack_signal = True
        self.waiting_for_ack = True

        timeout = self.ack_timeout_ms
        while timeout != 0 and self.waiting_for_ack_signal:
            sleep(0.001)
            timeout -= 1

        debug("+++ ACK WAIT OVER timeout = " + str(timeout))

        self.waiting_for_ack = False
        if not self.lastack:
            debug("write() failed to get ACK")
            return SHAKE_ERROR

        self.lastack = False

        return SHAKE_SUCCESS

    def register_event_callback(self, callback):
        self.navcb = callback

    #       All functions below here are used to read/write the configuration
    #       registers on the SHAKE. The constants referred to in the comments
    #       can all be found in the pyshake_constants.py file.

    #       Read or write the SHAKE_NV_REG_POWER1 register. This controls power to the
    #       various sensors on the device. When writing a value, use a logical
    #       OR of one or more of the SHAKE_POWER_* constants. Eg to turn on the
    #       accelerometers and gyros and disable everything else, use:
    #       SHAKE_POWER_ACC | SHAKE_POWER_GYR
    # 
    #       Reading from the register will return a 2-tuple consisting of a 
    #       return code (either SHAKE_SUCCESS or SHAKE_ERROR) followed by
    #       the current value of the register.
    def read_power_state(self):
        return self.read(SHAKE_NV_REG_POWER1)

    def write_power_state(self, value):
        if value < 0 or value > 255:
            return SHAKE_ERROR
        return self.write(SHAKE_NV_REG_POWER1, value)

    def read_power_state_2(self):
        return self.read(SHAKE_NV_REG_POWER1)

    def write_power_state_2(self, value):
        if value < 0 or value > 255:
            return SHAKE_ERROR
        return self.write(SHAKE_NV_REG_POWER2, value)

    #       Read or write the SHAKE_NV_REG_DATAFMT register. This controls the output
    #       format of the sensor data (either ASCII (default) or raw mode
    #
    #       When writing to the register, the value should either be 
    #       SHAKE_DATAFMT_ASCII or SHAKE_DATAFMT_RAW, other values will be
    #       rejected. 
    #       
    #       Reading from the register will return a 2-tuple consisting of a 
    #       return code (either SHAKE_SUCCESS or SHAKE_ERROR) followed by
    #       the current value of the register, which will be one of the two
    #       values detailed above. 
    def read_data_format(self):
        return self.read(SHAKE_NV_REG_DATAFMT)

    def write_data_format(self, value):
        return self.write(SHAKE_NV_REG_DATAFMT, value)

    def read_acc_config(self):
        return self.read(SHAKE_NV_REG_ACCCONFIG)

    def write_acc_config(self, value):
        return self.write(SHAKE_NV_REG_ACCCONFIG, value)

    #       Read or write the registers which control output sample rates for the
    #       different sensors on the SHAKE. In both cases, the <sensor> parameter
    #       should be set to one of the SHAKE_SENSOR_* constants (eg SHAKE_SENSOR_ACC) 
    #
    #       When writing to the register for a particular sensor, <value> must be
    #       between 0 and the corresponding SHAKE_<sensor>_MAX_RATE limit. For example
    #       the accelerometer sampling range is between 0 and SHAKE_ACC_MAX_RATE. Values
    #       outside this range will be rejected.
    #       
    #       Reading from the register will return a 2-tuple consisting of a 
    #       return code (either SHAKE_SUCCESS or SHAKE_ERROR) followed by
    #       the current sample rate for the selected sensor.
    def read_sample_rate(self, sensor):
        if sensor < SHAKE_SENSOR_ACC or sensor > SHAKE_SENSOR_ANA1:
            return SHAKE_ERROR

        return self.read(SHAKE_NV_REG_ACCOUT + sensor)

    def write_sample_rate(self, sensor, value):
        if sensor < SHAKE_SENSOR_ACC or sensor > SHAKE_SENSOR_ANA1:
            return SHAKE_ERROR

        return self.write(SHAKE_NV_REG_ACCOUT + sensor, value)

    def read_digital_filter(self, sensor):
        if sensor < SHAKE_SENSOR_ACC or sensor > SHAKE_SENSOR_ANA1:
            return (SHAKE_ERROR, 0)

        return self.read(SHAKE_NV_REG_DIGFIL_ACC + sensor)

    def write_digital_filter(self, sensor, value):
        if sensor < SHAKE_SENSOR_ACC or sensor > SHAKE_SENSOR_ANA1:
            return SHAKE_ERROR

        return self.write(SHAKE_NV_REG_DIGFIL_ACC + sensor, value)

    def read_calib_bypass(self):
        return self.read(SHAKE_NV_REG_CALIB_BYPASS)

    def write_calib_bypass(self, value):
        return self.write(SHAKE_NV_REG_CALIB_BYPASS, value)

    def read_cx_bypass(self):
        return self.read(SHAKE_NV_REG_CX_CALIB_BYPASS)

    def write_cx_bypass(self, value):
        return self.write(SHAKE_NV_REG_CX_CALIB_BYPASS, value)

    def read_temp_compensation(self):
        return self.read(SHAKE_NV_REG_TEMP_COMPENSATION)

    def write_temp_compensation(self, value):
        return self.write(SHAKE_NV_REG_TEMP_COMPENSATION, value)

    def read_packet_streaming(self):
        return self.read(SHAKE_NV_REG_STREAM_DISABLE)

    def write_packet_streaming(self, value):
        return self.write(SHAKE_NV_REG_STREAM_DISABLE, value)

    def sk6_read_cs0_inc(self):
        return self.read(SK6_NV_REG_CS0_INC)

    def sk6_write_cs0_inc(self, value):
        return self.write(SK6_NV_REG_CS0_INC, value)

    def sk6_read_cs0_dec(self):
        return self.read(SK6_NV_REG_CS0_DEC)

    def sk6_write_cs0_dec(self, value):
        return self.write(SK6_NV_REG_CS0_DEC, value)

    def sk6_read_cs0_inc_profile(self):
        return self.read(SK6_NV_REG_CS0_INC_PROFILE)

    def sk6_write_cs0_inc_profile(self, value):
        return self.write(SK6_NV_REG_CS0_INC_PROFILE, value)

    def sk6_read_cs0_dec_profile(self):
        return self.read(SK6_NV_REG_CS0_DEC_PROFILE)

    def sk6_write_cs0_dec_profile(self, value):
        return self.write(SK6_NV_REG_CS0_DEC_PROFILE, value)

    def sk6_read_cs1_inc(self):
        return self.read(SK6_NV_REG_CS1_INC)

    def sk6_write_cs1_inc(self, value):
        return self.write(SK6_NV_REG_CS1_INC, value)

    def sk6_read_cs1_dec(self):
        return self.read(SK6_NV_REG_CS1_DEC)

    def sk6_write_cs1_dec(self, value):
        return self.write(SK6_NV_REG_CS1_DEC, value)

    def sk6_read_cs1_inc_profile(self):
        return self.read(SK6_NV_REG_CS1_INC_PROFILE)

    def sk6_write_cs1_inc_profile(self, value):
        return self.write(SK6_NV_REG_CS1_INC_PROFILE, value)

    def sk6_read_cs1_dec_profile(self):
        return self.read(SK6_NV_REG_CS1_DEC_PROFILE)

    def sk6_write_cs1_dec_profile(self, value):
        return self.write(SK6_NV_REG_CS1_DEC_PROFILE, value)

    def read_audio_config(self):
        return self.read(SHAKE_NV_REG_AUDIO_CONFIG)

    def write_audio_config(self, value):
        return self.write(SHAKE_NV_REG_AUDIO_CONFIG, value)

    def write_midi_amplitude(self, value):
        return self.write(SHAKE_VO_REG_MIDI_AMPLITUDE, value)

    def write_midi_note(self, value):
        return self.write(SHAKE_VO_REG_MIDI_NOTE, value)

    def write_midi_waveform(self, value):
        return self.write(SHAKE_VO_REG_MIDI_WAVEFORM, value)

    def sk6_read_cap_thresholds(self):
        vals = []
        vals.append(self.read(SK6_NV_REG_CS0_INC)[1])
        vals.append(self.read(SK6_NV_REG_CS0_DEC)[1])
        vals.append(self.read(SK6_NV_REG_CS0_INC_PROFILE)[1])
        vals.append(self.read(SK6_NV_REG_CS0_DEC_PROFILE)[1])
        vals.append(self.read(SK6_NV_REG_CS1_INC)[1])
        vals.append(self.read(SK6_NV_REG_CS1_DEC)[1])
        vals.append(self.read(SK6_NV_REG_CS1_INC_PROFILE)[1])
        vals.append(self.read(SK6_NV_REG_CS1_DEC_PROFILE)[1])
        return vals

    def sk6_write_cap_thresholds(self, values):
        if values == None or len(values) != 8:
            return SHAKE_ERROR

        self.write(SK6_NV_REG_CS0_INC, values[0])
        self.write(SK6_NV_REG_CS0_DEC, values[1])
        self.write(SK6_NV_REG_CS0_INC_PROFILE, values[2])
        self.write(SK6_NV_REG_CS0_DEC_PROFILE, values[3])
        self.write(SK6_NV_REG_CS1_INC, values[4])
        self.write(SK6_NV_REG_CS1_DEC, values[5])
        self.write(SK6_NV_REG_CS1_INC_PROFILE, values[6])
        self.write(SK6_NV_REG_CS1_DEC_PROFILE, values[7])
        return SHAKE_SUCCESS

    def sk7_read_cap_thresholds(self):
        vals = []
        vals.append(self.read(SK7_NV_REG_CAP_INC)[1])
        vals.append(self.read(SK7_NV_REG_CAP_DEC)[1])
        vals.append(self.read(SK7_NV_REG_CAP_INC_PROFILE)[1])
        vals.append(self.read(SK7_NV_REG_CAP_DEC_PROFILE)[1])
        return vals

    def sk7_write_cap_thresholds(self, values):
        if len(values) != 4:
            return SHAKE_ERROR

        self.write(SK7_NV_REG_CAP_INC, values[0])
        self.write(SK7_NV_REG_CAP_DEC, values[1])
        self.write(SK7_NV_REG_CAP_INC_PROFILE, values[2])
        self.write(SK7_NV_REG_CAP_DEC_PROFILE, values[3])

        return SHAKE_SUCCESS

    # Logging...
    def logging_pause(self):
        return self.write(SHAKE_VO_REG_LOGGING_CTRL, SHAKE_LOGGING_PAUSE)

    def logging_stop(self):
        if self.logfp:
            self.logfp.close()
        return self.write(SHAKE_VO_REG_LOGGING_CTRL, SHAKE_LOGGING_STOP)

    def logging_record(self):
        if self.logfp:
            self.logfp.close()
        return self.write(SHAKE_VO_REG_LOGGING_CTRL, SHAKE_LOGGING_RECORD)

    def logging_reset(self):
        return self.write(SHAKE_VO_REG_LOGGING_CTRL, SHAKE_LOGGING_RESET)

    def logging_play(self, filename):
        self.logfp = open(filename, 'w')
        return self.write(SHAKE_VO_REG_LOGGING_CTRL, SHAKE_LOGGING_PLAY)

    def logging_status(self):
        ret = self.read(SHAKE_VO_REG_LOGGING_STATUS)
        if ret[0] == SHAKE_ERROR:
            return SHAKE_ERROR

        return ret[1]

    def logging_packet_count(self):
        ret = self.read(SHAKE_NV_REG_LOGGING_PKT_LSB)
        if ret[0] == SHAKE_ERROR:
            return SHAKE_ERROR
        lsb = ret[1]

        ret = self.read(SHAKE_NV_REG_LOGGING_PKT_MSB)
        if ret[0] == SHAKE_ERROR:
            return SHAKE_ERROR
        msb = ret[1]

        return 100 * ((msb << 8) + lsb)

    # Shaking configuration
    def read_shaking_config(self):
        return self.read(SHAKE_NV_REG_SHAKING_CONFIG)

    def write_shaking_config(self, value):
        return self.write(SHAKE_NV_REG_SHAKING_CONFIG, value)

    def read_shaking_accel_threshold(self):
        return self.read(SHAKE_NV_REG_SHAKING_ACCEL_THRESHOLD)

    def write_shaking_accel_threshold(self, value):
        return self.write(SHAKE_NV_REG_SHAKING_ACCEL_THRESHOLD, value)

    def read_shaking_holdoff_time(self):
        return self.read(SHAKE_NV_REG_SHAKING_HOLDOFF_TIME)

    def write_shaking_holdoff_time(self, value):
        return self.write(SHAKE_NV_REG_SHAKING_HOLDOFF_TIME, value)

    def read_shaking_vibration_profile(self):
        return self.read(SHAKE_NV_REG_SHAKING_VIBRATION_PROFILE)

    def write_shaking_vibration_profile(self, value):
        return self.write(SHAKE_NV_REG_SHAKING_VIBRATION_PROFILE, value)

    def read_shaking_hpf_constant(self):
        return self.read(SHAKE_NV_REG_SHAKING_HPF_CONSTANT)

    def write_shaking_hpf_constant(self, value):
        return self.write(SHAKE_NV_REG_SHAKING_HPF_CONSTANT, value)

    def read_shaking_lpf_constant(self):
        return self.read(SHAKE_NV_REG_SHAKING_LPF_CONSTANT)

    def write_shaking_lpf_constant(self, value):
        return self.write(SHAKE_NV_REG_SHAKING_LPF_CONSTANT, value)

    def reset_shaking_detection(self):
        return self.write(SHAKE_NV_REG_SHAKING_CONFIG, 0xFF)
        return self.write(SHAKE_NV_REG_SHAKING_ACCEL_THRESHOLD, 0x0A)
        return self.write(SHAKE_NV_REG_SHAKING_HOLDOFF_TIME, 0x06)
        return self.write(SHAKE_NV_REG_SHAKING_VIBRATION_PROFILE, 0x00)
        return self.write(SHAKE_NV_REG_SHAKING_HPF_CONSTANT, 0x20)
        return self.write(SHAKE_NV_REG_SHAKING_LPF_CONSTANT, 0x06)

    def read_heart_rate_config(self):
        return self.read(SHAKE_NV_REG_HEART_RATE_CONFIG)

    def write_heart_rate_config(self, value):
        return self.write(SHAKE_NV_REG_HEART_RATE_CONFIG, value)

    def read_expansion_config(self):
        return self.read(SHAKE_NV_REG_EXPANSION_CONFIG)

    def write_expansion_config(self, value):
        return self.write(SHAKE_NV_REG_EXPANSION_CONFIG, value)

    def read_rfid_config(self):
        return self.read(SHAKE_NV_REG_RFID_CONFIG)

    def write_rfid_config(self, value):
        return self.write(SHAKE_NV_REG_RFID_CONFIG, value)

    def read_rfid_scan_freq(self):
        return self.read(SHAKE_NV_REG_RFID_FREQUENCY)

    def write_rfid_scan_freq(self, value):
        return self.write(SHAKE_NV_REG_RFID_FREQUENCY)

    def heart_rate(self):
        return self.SHAKE.data.hrbpm

    def rfid_tid(self):
        return self.SHAKE.lasttid

    def read_temperature(self):
        ret = self.read(SHAKE_VO_REG_TEMPERATURE)
        if ret[0] == SHAKE_ERROR:
            return -1.0

        return ret[1] / 4.0

    #       Writes a value to the SHAKE_VO_REG_PKTREQ register (write-only). This register 
    #       allows you to request a set of data from one or more sensors (in addition
    #       to or instead of the usual data stream from the device). 
    #
    #       When writing to the register, the value you use should be a logical OR of
    #       one or more SHAKE_REQ_<sensor> constants, eg SHAKE_REQ_ACC | SHAKE_REQ_GYR
    #       to request both accelerometer and gyro packets.
    #
    #       The data will be sent and processed in the usual way, so you should use the
    #       data access functions above to read it.
    def write_packet_request(self, value):
        return self.write(SHAKE_VO_REG_PKTREQ, value)

    def write_data_request(self, value):
        return self.write(SHAKE_VO_REG_DATAREQ, value)

    def read_battery_level(self):
        return self.read(SHAKE_VO_REG_BATTERY)

    #       Reads from the SHAKE_VO_REG_PWRSTA register (read-only). 
    #       This register contains the power status of the SHAKE device, made up from 
    #       three different values:
    #           1. External power on/off (AC adapter)
    #           2. Battery charging/not charging
    #           3. Battery charged/not fully charged
    #       The value returned from the function will be a logical OR of one or more
    #       of the following three constants, corresponding to the list above:
    #           1. SHAKE_EXT_POWER
    #           2. SHAKE_BAT_CHARGING
    #           3. SHAKE_BAT_FULL
    #       To test for each condition, simply use a logical AND with the appropriate
    #       constant. For example, to test for external power:
    #           val = shake.read_power_status()
    #           if val & SHAKE_EXT_POWER:
    #               print "external power on"
    def read_power_status(self):
        return self.read(SHAKE_VO_REG_PWRSTA)

    #
    #       Vibration playback + upload
    #

    def playvib(self, channel, profile):
        if profile < 0 or profile > SHAKE_VIB_PROFILE_MAX:
            return SHAKE_ERROR

        if channel < SHAKE_VIB_MAIN or channel > SHAKE_VIB_EXT_ACTUATOR:
            return SHAKE_ERROR

        if self.device_type == SHAKE_SK6:
            addr = SHAKE_VO_REG_VIB_MAIN + channel
            return self.write(SHAKE_VO_REG_VIB_MAIN + channel, profile)
        else:
            buf = None
            if channel == SHAKE_VIB_MAIN:
                buf = 'vm%02X' % profile
            elif channel == SHAKE_VIB_LEFT:
                buf = 'vl%02X' % profile
            elif channel == SHAKE_VIB_RIGHT:
                buf = 'vr%02X' % profile
            elif channel == SHAKE_VIB_FORCEREACTOR:
                buf = 'vf%02X' % profile
            elif channel == SHAKE_VIB_EXT_ACTUATOR:
                buf = 'vd%02X' % profile
            else:
                return SHAKE_ERROR

            self.write_to_port(buf)

        return SHAKE_SUCCESS

    #       [SK6] continuous vibration support
    def sk6_playvib_continuous(self, channel, amplitude, time):
        if channel != SHAKE_VIB_LEFT or channel != SHAKE_VIB_RIGHT:
            return SHAKE_ERROR

        if amplitude != 0 and amplitude != 33 and amplitude != 66 and amplitude != 100:
            return SHAKE_ERROR

        if time < 0 or time > 64:
            return SHAKE_ERROR

        vibbyte = 0
        if amplitude == 0:
            vibbyte = 0x00
        elif amplitude == 33:
            vibbyte = 0x40;
        elif amplitude == 66:
            vibbyte = 0x80;
        elif amplitude == 100:
            vibbyte = 0xc0;

        vibbyte += time;

        vibaddr = SHAKE_VO_REG_VIB_LEFT_CONTINUOUS;
        if(channel == SHAKE_VIB_RIGHT):
            vibaddr = SHAKE_VO_REG_VIB_RIGHT_CONTINUOUS;
        return self.write(vibaddr, vibbyte)

    #       [SK6] simple vib upload for SK6s
    def sk6_upload_vib_sample(self, profile, samples):
        return self.upload_vib_sample_extended(profile, samples, 0, 0, 0)

    #       [SK7] full vib upload function for SK7s
    def upload_vib_sample_extended(self, profile, samples, mode, freq, duty):
        if profile < 0 or profile > SHAKE_VIB_PROFILE_MAX:
            return SHAKE_ERROR

        if len(samples) < 2 or (len(samples) % 2 != 0):
            return SHAKE_ERROR

        debug("uploading to location " + str(profile))

        svp = '$VIB,%02X,%02X,%02X,%02X' % (profile, mode, freq, duty)

        i = 0
        while i < len(samples):
            s = ',%02X,%02X' % (samples[i], samples[i+1])
            svp += s
            i += 2

        svp += '~'
        debug("sending upload: " + svp + "\n")

        self.waiting_for_ack = True
        self.lastaddr = -1

        self.write_to_port(svp)

        elapsed = 0
        while self.waiting_for_ack and elapsed < 2000:
            sleep(0.01)
            elapsed += 10 

        self.waiting_for_ack = False

        if not self.lastack or self.lastaddr == -1:
            return SHAKE_ERROR

        return SHAKE_SUCCESS

    #
    #       Miscellaneous functions
    #

    def get_ack_timeout_ms(self):
        return self.ack_timeout_ms

    def set_ack_timeout_ms(self, new_timeout_ms):
        self.ack_timeout_ms = new_timeout_ms

    def read_info_line(self):
        pos = 0
        buf = ""
        while pos < 512:
            tmp = self.read_data(1)

            # skip nulls (some SHAKEs output stray nulls in the startup text)
            if ord(tmp) == 0:
                continue

            buf += tmp

            if len(buf) > 0 and (ord(buf[pos]) == 0xD or ord(buf[pos]) == 0xA):
                # compare last 2 bytes to delimiter
                if pos >= 1:
                    i = pos - 1
                    if (ord(buf[i]) == 0xA and ord(buf[i+1]) == 0xD) or (ord(buf[i]) == 0xD and ord(buf[i+1]) == 0xA):
                        return buf

            pos += 1

        return None
