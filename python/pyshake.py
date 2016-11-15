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

"""
pyshake

Top-level module for the Python SHAKE driver implementation. Supports both 
SK6 and SK7 SHAKEs. 
"""

import thread
from time import sleep

from pyshake_constants import *
import pyshake_sk6 
import pyshake_sk7
import pyshake_serial_pc as pyshake_serial

# only enable this if you are debugging the driver itself... 
debugging = False
def debug(str, opennew=False):
    if not debugging:
        return

    f = None
    if opennew:
        f = open("debug.txt", "w")
    else:
        f = open("debug.txt", "a")

    f.write(str + "\n")
    f.close()

class ShakeDevice:
    """
    Objects of this class represent a single SHAKE device and the current
    connection (if any) to the host device. It contains methods to configure
    the device, query the current configuration and receive sensor data and
    events.
    """

    # 2nd parameter indicates type of device. Default is SK7.
    def __init__(self, type=SHAKE_SK7):
        """
        Create a new ShakeDevice object.

        :param type: the model of SHAKE that this object will be used to
            control. Defaults to the newer SK7 model. 
        """
        self.device_type = type
        self.port = None
        self.port_lock = None

        self.waiting_for_ack = False
        self.waiting_for_ack_signal = False
        self.serial = "missing"
        self.fwrev = -1
        self.hwrev = -1
        self.bluetoothfwrev = -1

        if type == SHAKE_SK6:
            self.modules = [SK6_MODULE_NONE for x in range(2)]
            self.SHAKE = pyshake_sk6.SK6(self, type)
        elif type == SHAKE_SK7:
            self.modules = [SK7_MODULE_NONE for x in range(5)]
            self.SHAKE = pyshake_sk7.SK7(self, type)
        else:
            raise Exception("Unknown device type")

        self.checksum = False
        self.navcb = None
        self.data_callback = None

        self.data_recv = 0
        self.wait_for_acks = True

        self.lastack = False
        self.lastaddr, self.lastval = 0, 0
        self.ack_timeout_ms = 500

        self.lastevent = 0
        self.logfp = None
        self.packets_read = 0

        self.device_address = None
        self.write_to_port = None
        self.thread_done = True

    def connect(self, addr):
        """
        Connect to a SHAKE identified by an address (which may be a COM port
        number or a character device filename depending on the current 
        platform). 

        :param addr: the address of the device. For Windows users this should 
            be a COM port identifier, either as a plain integer or a string such
            as 'COM9:' (basically any valid pyserial identifier should work). For
            OSX/Linux it will typically be a character device filename, eg 
            '/dev/tty.SHAKESK7SN0077-SPPDev'
        :returns: True if connection succeeded, False otherwise
        """

        if self.port or not self.thread_done or addr is None:
            return False

        self.device_address = addr
        if self._open_port(addr) == SHAKE_ERROR:
            return False

        self.thread_done = False

        thread.start_new_thread(self._reader_thread, ())

        elapsed = 0
        while elapsed < 5.0 and not self.SHAKE.synced:
            sleep(0.01)
            elapsed += 0.01

        return self.SHAKE.synced

    def connect_debug(self, filename):
        """
        (debugging use only) This function can be used to force the driver to 
        read data from a local file rather than a seriald device.

        :param filename: name of the file containing data to parse 
        :returns: True if opening the file succeeded, False otherwise
        """

        if self.port or not self.thread_done or filename is None:
            return False

        self.device_address = filename
        try:
            self.port = open(filename, 'rb')
        except:
            return False

        self.thread_done = False

        thread.start_new_thread(self._reader_thread, ())

        self.SHAKE.synced = True
        return self.SHAKE.synced

    def close(self):
        """
        Closes an active connection to a device.

        :returns: True if the connection was successfully closed or was closed
            already, False if an active connection failed to close.
        """
        if not self.port or self.thread_done:
            return True

        self.thread_done = True
        sleep(0)  # let the thread close itself.

        self.port.close() 
        self.port = None

        # avoids intermittent exceptions that seem to occur when the thread
        # reading from the serial port hasn't completely finished by the time
        # the main thread is exiting
        if self.port_lock.acquire():
            self.port_lock.release()

        return True

    def _open_port(self, addr):
        """
        Attempt to open a connection to the SHAKE with the given address.

        :param addr: the port identifier (COM port number, char device filename)
        :returns: SHAKE_SUCCESS or SHAKE_ERROR (TODO getlasterror type func?)
        """
        try:
            self.port = pyshake_serial.serial_port(addr)
            baud = 230400
            if self.device_type != SHAKE_SK6:
                baud = 460800

            if not self.port.open(baud):
                debug("port creation failed")
                self.thread_done = True
                return SHAKE_ERROR

            self.write_to_port = self.port.write
        except:
            debug('error; port creation failed')
            self.thread_done = True
            return SHAKE_ERROR

        return SHAKE_SUCCESS

    def _reader_thread(self):
        """
        Runs in a thread, continually parsing packets as they arrive on the
        the serial port. 
        """
        if self.port_lock and self.port_lock.locked():
            self.port_lock.release()

        self.port_lock = thread.allocate_lock()
        with self.port_lock: # acquires the lock
            try:
                self.thread_done = False
                while not self.thread_done:
                    packet_type = SHAKE_BAD_PACKET

                    while not self.thread_done and packet_type == SHAKE_BAD_PACKET:
                        (packet_type, packet) = self.SHAKE.get_next_packet()

                    if self.thread_done:
                        break

                    self.SHAKE.parse_packet(packet, packet_type)
            except Exception as e:
                import sys, traceback
                print("\n".join(traceback.format_exception(*sys.exc_info())))
                # TODO do something more sensible with exception info, eg use 
                # it for a getlasterror() function
                pass

    def data_timestamp(self, sensor):
        """
        Used to obtain the latest sequence number for the data stream of
        a given sensor. These sequence numbers can be used to check for
        missing packets in the data stream, or to perform some action
        when new data arrives. Note that the value of the timestamp will
        depend on the output format that the device is configured to use. In 
        ASCII mode, the output will range from 0-99 and then wrap back to 0. 
        In basic raw mode, there are no sequence numbers. In raw mode with 
        serial numbers enabled, the values range from 0-255 before wrapping,
        but the value 127 is always skipped due to it being used as a header
        identifier in the data stream.

        :param sensor: the sensor identifier (eg SHAKE_SENSOR_ACC)
        :returns: -1 if an invalid sensor selected, otherwise the last
            available timestamp for the selected sensor. 
        """
        if sensor < SHAKE_SENSOR_ACC or sensor > SHAKE_SENSOR_ANA1:
            return -1

        return self.SHAKE.data.timestamps[sensor]

    def accx(self):
        """Return latest x-axis accelerometer value"""
        x = self.SHAKE.data.accx
        self.SHAKE.data.timestamps[SHAKE_SENSOR_ACC] = self.SHAKE.data.internal_timestamps[SHAKE_SENSOR_ACC]
        return x

    def accy(self):
        """Return latest y-axis accelerometer value"""
        y = self.SHAKE.data.accy
        self.SHAKE.data.timestamps[SHAKE_SENSOR_ACC] = self.SHAKE.data.internal_timestamps[SHAKE_SENSOR_ACC]
        return y

    def accz(self):
        """Return latest z-axis accelerometer value"""
        z = self.SHAKE.data.accz
        self.SHAKE.data.timestamps[SHAKE_SENSOR_ACC] = self.SHAKE.data.internal_timestamps[SHAKE_SENSOR_ACC]
        return z

    def acc(self):
        """
        Return latest accelerometer values as a list.

        :returns: accelerometer values in a 3 element list ([x, y, z])
        """
        xyz = [self.SHAKE.data.accx, self.SHAKE.data.accy, self.SHAKE.data.accz]
        self.SHAKE.data.timestamps[SHAKE_SENSOR_ACC] = self.SHAKE.data.internal_timestamps[SHAKE_SENSOR_ACC]
        return xyz

    def gyrx(self):
        """Return latest x-axis gyro value"""
        x = self.SHAKE.data.gyrx
        self.SHAKE.data.timestamps[SHAKE_SENSOR_GYRO] = self.SHAKE.data.internal_timestamps[SHAKE_SENSOR_GYRO]
        return x

    def gyry(self):
        """Return latest y-axis gyro value"""
        y = self.SHAKE.data.gyry
        self.SHAKE.data.timestamps[SHAKE_SENSOR_GYRO] = self.SHAKE.data.internal_timestamps[SHAKE_SENSOR_GYRO]
        return y

    def gyrz(self):
        """Return latest z-axis gyro value"""
        z = self.SHAKE.data.gyrz
        self.SHAKE.data.timestamps[SHAKE_SENSOR_GYRO] = self.SHAKE.data.internal_timestamps[SHAKE_SENSOR_GYRO]
        return z

    def gyro(self):
        """
        Return latest gyroscope values as a list.

        :returns: gyroscope values in a 3 element list ([x, y, z])
        """
        xyz = [self.SHAKE.data.gyrx, self.SHAKE.data.gyry, self.SHAKE.data.gyrz]
        self.SHAKE.data.timestamps[SHAKE_SENSOR_GYRO] = self.SHAKE.data.internal_timestamps[SHAKE_SENSOR_GYRO]
        return xyz

    def magx(self):
        """Return latest x-axis magnetometer value"""
        x = self.SHAKE.data.magx
        self.SHAKE.data.timestamps[SHAKE_SENSOR_MAG] = self.SHAKE.data.internal_timestamps[SHAKE_SENSOR_MAG]
        return x

    def magy(self):
        """Return latest y-axis magnetometer value"""
        y = self.SHAKE.data.magy
        self.SHAKE.data.timestamps[SHAKE_SENSOR_MAG] = self.SHAKE.data.internal_timestamps[SHAKE_SENSOR_MAG]
        return y

    def magz(self):
        """Return latest z-axis magnetometer value"""
        z = self.SHAKE.data.magz
        self.SHAKE.data.timestamps[SHAKE_SENSOR_MAG] = self.SHAKE.data.internal_timestamps[SHAKE_SENSOR_MAG]
        return z

    def mag(self):
        """
        Return latest magnetometer values as a list.

        :returns: magnetometer values in a 3 element list ([x, y, z])
        """
        xyz = [self.SHAKE.data.magx, self.SHAKE.data.magy, self.SHAKE.data.magz]
        self.SHAKE.data.timestamps[SHAKE_SENSOR_MAG] = self.SHAKE.data.internal_timestamps[SHAKE_SENSOR_MAG]
        return xyz

    def heading(self):
        """
        Return latest heading (note that this only applies to the basic 
        builtin heading calculation algorithm, not the more complex
        'roll-pitch-heading' functionality detailed in the manual. 

        :returns: latest heading value (tenths of a degree, 0-3599)
        """
        heading = self.SHAKE.data.heading
        self.SHAKE.data.timestamps[SHAKE_SENSOR_HEADING] = self.SHAKE.data.internal_timestamps[SHAKE_SENSOR_HEADING]
        return heading

    def sk6_cap0(self):
        """
        (SK6 only) Return the value of the first capacitive sensor

        :returns: latest capacitive sensor value (0-255)
        """
        c = self.SHAKE.data.cap_sk6[0]
        self.SHAKE.data.timestamps[SHAKE_SENSOR_CAP0] = self.SHAKE.data.internal_timestamps[SHAKE_SENSOR_CAP0]
        return c

    def sk6_cap1(self):
        """
        (SK6 only) Return the value of the second capacitive sensor

        :returns: latest capacitive sensor value (0-255)
        """
        c = self.SHAKE.data.cap_sk6[1]
        self.SHAKE.data.timestamps[SHAKE_SENSOR_CAP1] = self.SHAKE.data.internal_timestamps[SHAKE_SENSOR_CAP1]
        return c

    def cap(self):
        """
        (SK6 only) Returns the values of both capacitive sensors as a list
        (SK7 only) Returns the values of the standard 12 capacitive sensors 
        as a list.

        :returns: (SK6) 2-element list, (SK7) 12-element list
        """
        if self.device_type == SHAKE_SK6:
            cap = self.SHAKE.data.cap_sk6
            self.SHAKE.data.timestamps[SHAKE_SENSOR_CAP0] = self.SHAKE.data.cap0seq
            self.SHAKE.data.timestamps[SHAKE_SENSOR_CAP1] = self.SHAKE.data.cap1seq
        else:
            cap = self.SHAKE.data.cap_sk7[0]
            self.SHAKE.data.timestamps[SHAKE_SENSOR_CAP] = self.SHAKE.data.internal_timestamps[SHAKE_SENSOR_CAP]
        return cap

    def cap_ext(self, blocks):
        """
        (SK7 only) Returns the capacitive sensor values for an SK7-ExtCs1 board
        attached to an SK7.

        :param blocks: selects the board(s) to return values from. 0 = first 
            block of 12 sensors only, 1 = second block of 12 sensors only, 3 = 
            all 24 sensors.
        :returns: the selected capacitive sensor values as a list, or None if
            an invalid blocks parameter is supplied.
        """
        if blocks == 0:
            return self.SHAKE.data.cap_sk7[1]
        elif blocks == 1:
            return self.SHAKE.data.cap_sk7[2]
        elif blocks == 2:
            return self.SHAKE.data.cap_sk7[1] + self.SHAKE.data.cap_sk7[2]
        else:
            return None

    def analog0(self):
        """
        Returns the latest value from the first analogue input channel.

        :returns: latest value from analogue channel 0
        """
        a = self.SHAKE.data.ana0
        self.SHAKE.data.timestamps[SHAKE_SENSOR_ANA0] = self.SHAKE.data.internal_timestamps[SHAKE_SENSOR_ANA0]
        return a

    def analog1(self):
        """
        Returns the latest value from the second analogue input channel.

        :returns: latest value from analogue channel 1
        """
        a = self.SHAKE.data.ana1
        self.SHAKE.data.timestamps[SHAKE_SENSOR_ANA1] = self.SHAKE.data.internal_timestamps[SHAKE_SENSOR_ANA1]
        return a

    def analog(self):
        """
        Returns the latest values from both analogue inputs.

        :returns: latest values from both analogue inputs as a 2-element list.
        """
        ana = [self.SHAKE.data.ana0, self.SHAKE.data.ana1]
        self.SHAKE.data.timestamps[SHAKE_SENSOR_ANA0] = self.SHAKE.data.internal_timestamps[SHAKE_SENSOR_ANA0]
        self.SHAKE.data.timestamps[SHAKE_SENSOR_ANA1] = self.SHAKE.data.internal_timestamps[SHAKE_SENSOR_ANA1]
        return ana

    def sk7_imus(self):
        """
        Returns the latest values from all SK7 IMUs.

        :returns: latest values from all SK7 IMUs.
        """
        return self.SHAKE.data.imudata 

    def sk7_imu(self, index):
        """
        Returns the latest values from a selected IMU (index 0-4).

        :returns: latest values from the selected IMU.
        """
        if index < 0 or index > 4:
            return None
        
        return self.SHAKE.data.imudata[index]

    # TODO: do these functions actually still work? They used to, but the latest
    # manual doesn't mention this functionality at all...
    # def shaking_peakaccel(self):
    #    return self.SHAKE.data.peakaccel
    #
    # def shaking_direction(self):
    #    return self.SHAKE.data.direction
    #
    # def shaking_timestamp(self):
    #    return self.SHAKE.data.timestamp

    #       [SK7] return [roll, pitch, heading] data 
    def sk7_roll_pitch_heading(self):
        """
        (SK7 only) Return latest roll-pitch-heading (RPH) data

        :returns: RPH data as a 3-element list ([roll, pitch, heading]). All 3
            are in units of tenths of a degree. Roll and pitch range between 
            -1800 and 1799, heading between 0 and 3599. 
        """
        return self.SHAKE.data.rph

    def sk7_roll_pitch_heading_quaternions(self):
        """
        (SK7 only) Return latest roll-pitch-heading (RPH) data in quaternion
        format, as a list. 

        :returns: the quaternion parameters A,B,C and D in a 4-element list 
           ([A, B, C, D])
        """
        return self.SHAKE.data.rphq

    def sk7_configure_roll_pitch_heading(self, val):
        """
        Update the current roll-pitch-heading (RPH) data output settings. 

        :param val: For complete details, see the section of the user manual
            called "AHRS Configuration Register 0x0046". Common values would be: 
            0 (disable RPH functionality and fall back to simple heading output), 
            3 (enable RPH functionality and standard output mode)
            7 (enable RPH functionality and quaternion output mode)
        :returns: SHAKE_SUCCESS or SHAKE_ERROR
        """
        return self.write(SK7_NV_REG_RPH_CONFIG, val)

    def sk7_gyro_temperatures(self):
        """
        (SK7 only) Return the latest set of gyro sensor temperature readings. The
        return value is always a 4 element list with pitch, roll, yaw axis temperatures
        as the first 3 values. In ASCII output mode the final value is unset, but 
        in binary output mode it will be set to the temperature from a sensor on the
        SK7 mainboard next to the acc/mag sensors.
        """
        return self.SHAKE.data.gyro_temps

    def register_data_callback(self, callback):
        """
        Register a callback function which will be called each time a new
        data packet arrives from the device. The function should expect 3
        parameters: (sensor_id, sensor_data, sensor_timestamp). 
        
        sensor_id identifies the sensor (eg SHAKE_SENSOR_ACC).

        sensor_data contains the sensor data in a list, the contents of which
        will vary depending on the sensor (eg a 3-element list for the 
        accelerometer). 

        sensor_timestamp contains the corresponding sequence number for the
        sensor data (if available). 

        :param callback: callable object
        """
        self.data_callback = callback

    def info_retrieve(self):
        """
        Attempts to retrieve information about the SHAKE, which can then be
        accessed by the other info functions. Does nothing if the data is
        already available (which it usually will be as the driver attempts
        to parse it immediately after the connection is first opened). 

        :returns: SHAKE_SUCCESS or SHAKE_ERROR
        """
        if self.fwrev == -1:
            if self.write_data_request(1) == SHAKE_ERROR:
                return SHAKE_ERROR

            count = 0
            while count < 1000 and self.fwrev == -1:
                count += 10
                sleep(0.01)

        if self.fwrev == -1:
            return SHAKE_ERROR

        return SHAKE_SUCCESS

    def info_firmware_revision(self):
        """
        Returns the device firmware revision number.

        :returns: The revision number as a floating point value (with 2 
            significant digits). Returns -1 if an error occurred.
        """
        self.info_retrieve()
        return self.fwrev

    def info_hardware_revision(self):
        """
        Returns the device hardware revision number.

        :returns: The revision number as a floating point value (with 2 
            significant digits). Returns -1 if an error occurred.
        """
        self.info_retrieve()
        return self.hwrev

    def info_serial_number(self):
        """
        Returns the device serial number.

        :returns: The serial number as a string, format will be 'SN<number>',
            eg 'SN0077'. Returns 'missing' if an error occurred. 
        """
        self.info_retrieve()
        return self.serial

    def info_num_slots(self):
        """
        Returns the number of hardware expansion slots the device contains.

        :returns: number of hardware expansion slots
        """
        return len(self.modules)

    def info_module(self, slotnumber):
        """
        TODO: doesn't work for SK7s, need a list of available modules!
        
        Return an identifier for the type of extension module installed
        in the connected SHAKE device (if any)

        :param slotnumber: the slot number, 0-based (the SK6 has 2 slots, the
            SK7 has 5).
        :returns: module ID (which may be SK7_MODULE_NONE/SK6_MODULE_NONE) 
            or None if an error occurred:
        """
        self.info_retrieve()
        if slotnumber < 0 or slotnumber > len(self.modules):
            return None
        return self.modules[slotnumber]

    def info_module_name(self, module):
        """
        Returns a string identifier for a given expansion module ID.

        :param module: an integer module identifier 
        :returns: a string describing the module, or an empty string if an 
            error occurred.
        """
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
        return ''

    def info_bluetooth_firmware_revision(self):
        """
        Returns the device Bluetooth firmware revision number. 

        :returns: The revision number as a floating point value (with 2 
            significant digits). Returns -1 if an error occurred.
        """
        self.info_retrieve()
        return self.bluetoothfwrev

    def read(self, address):
        """
        Reads a register on the device.

        :param address: the register address to read from (see the user manual
            for a full list of available register addresses)
        :returns: a 2-tuple (result, value). On success, result will be set to
            SHAKE_SUCCESS and value to the register content. On error, result 
            will be SHAKE_ERROR and value zeroed.
        """
        scp = '$REA,%04X,00' % (address)

        if self.waiting_for_ack:
            return SHAKE_ERROR

        self.write_to_port(scp)
        debug("SENT: " + scp)

        self.waiting_for_ack_signal = True
        self.waiting_for_ack = True

        timeout = self.ack_timeout_ms
        while self.waiting_for_ack_signal and timeout > 0:
            sleep(0.01)
            timeout -= 10

        self.waiting_for_ack = False

        if not self.lastack:
            return (SHAKE_ERROR, 0)

        self.lastack = False

        return (SHAKE_SUCCESS, self.lastval)

    def write(self, address, value):
        """
        Write a new value into a register on the device.

        :param address: the register address to write to (see the user manual
            for a full list of the available register addresses)
        :param value: the new value (0-255)
        :returns: SHAKE_SUCCESS if the value was successfully written, or
            SHAKE_ERROR if an error occurred. 
        """
        if not isinstance(value, int) or value < 0 or value > 255:
            return SHAKE_ERROR

        scp = '$WRI,%04X,%02X' % (address, value)

        if self.waiting_for_ack:
            debug("write() already waiting for ack")
            return SHAKE_ERROR

        self.write_to_port(scp)
        debug("+++ SENT: " + scp)

        self.waiting_for_ack_signal = True
        self.waiting_for_ack = True

        timeout = self.ack_timeout_ms
        while self.waiting_for_ack_signal and timeout > 0:
            sleep(0.01)
            timeout -= 10

        debug("+++ ACK WAIT OVER timeout = " + str(timeout))

        self.waiting_for_ack = False
        if not self.lastack:
            debug("write() failed to get ACK")
            return SHAKE_ERROR

        self.lastack = False

        return SHAKE_SUCCESS

    def register_event_callback(self, callback):
        """
        Register a callback function to be called when events are received
        from the SHAKE (as opposed to streaming sensor data). The function
        should expect a single parameter, which will be an integer identifying
        the event type (eg SHAKE_NAV_UP).

        :param callback: callable object
        """
        self.navcb = callback

    def read_power_state(self):
        """
        Read the main power state configuration register. 

        :returns: a 2-tuple (result, value). On success, result will be set to
            SHAKE_SUCCESS and value to the register content. On error, result 
            will be SHAKE_ERROR and value zeroed.
        """
        return self.read(SHAKE_NV_REG_POWER1)

    def write_power_state(self, value):
        """
        Write to the main power state configuration register. You can use this
        to enable/disable the main onboard sensors. For example, to enable
        the accelerometer and magnetometer only:

        write_power_state(SHAKE_POWER_ACC | SHAKE_POWER_MAG)

        See the user manual for more details.

        :param value: new register value. (0-255). 
        :returns: SHAKE_SUCCESS or SHAKE_ERROR
        """
        return self.write(SHAKE_NV_REG_POWER1, value)

    def read_power_state_2(self):
        """
        Read the secondary power state configuration register.

        :returns: a 2-tuple (result, value). On success, result will be set to
            SHAKE_SUCCESS and value to the register content. On error, result 
            will be SHAKE_ERROR and value zeroed.
        """
        return self.read(SHAKE_NV_REG_POWER1)

    def write_power_state_2(self, value):
        """
        Write to the main power state configuration register. This controls
        some optional/extra sensors. See the user manual for more details.

        :param value: new register value (0-255).
        :returns: SHAKE_SUCCESS or SHAKE_ERROR
        """
        return self.write(SHAKE_NV_REG_POWER2, value)

    def read_data_format(self):
        """
        Read the data output format register. 

        :returns: a 2-tuple (result, value). On success, result will be set to
            SHAKE_SUCCESS and value to the register content. On error, result 
            will be SHAKE_ERROR and value zeroed.
        """
        return self.read(SHAKE_NV_REG_DATAFMT)

    def write_data_format(self, value):
        """
        Write the the data output format register. This controls the packet 
        format generated by the SHAKE. The main use for this function is to
        switch between ASCII output (verbose but human readable) and raw binary
        output (much more bandwidth efficient). 

        :param value: the new register value. 0 enables ASCII mode, 2 enables
            raw/binary output mode, 6 enables raw/binary output with packet 
            sequence numbering.
        :returns: SHAKE_SUCCESS or SHAKE_ERROR
        """
        return self.write(SHAKE_NV_REG_DATAFMT, value)

    def read_acc_config(self):
        return self.read(SHAKE_NV_REG_ACC_CONFIG)

    def write_acc_config(self, value):
        return self.write(SHAKE_NV_REG_ACC_CONFIG, value)

    def read_sample_rate(self, sensor):
        """
        Read one of the registers that control the sample rate for the main
        onboard sensors. 

        :param sensor: one of the SHAKE_SENSO constants.
        :returns: a 2-tuple (result, value). On success, result will be set to
            SHAKE_SUCCESS and value to the register content. On error, result 
            will be SHAKE_ERROR and value zeroed. The sample rate is given in Hz. 
        """
        if sensor < SHAKE_SENSOR_ACC or sensor > SHAKE_SENSOR_ANA1:
            return SHAKE_ERROR

        return self.read(SHAKE_NV_REG_ACCOUT + sensor)

    def write_sample_rate(self, sensor, value):
        """
        Write to one of the registers that control the sample rate for the main
        onboard sensors. 

        :param sensor: one of the SHAKE_SENSOR constants.
        :returns: SHAKE_SUCCESS or SHAKE_ERROR
        """
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
        if values is None or len(values) != 8:
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


    def logging_pause(self):
        """
        Pauses any current logging operation (playback or recording). 

        :returns: SHAKE_SUCCESS or SHAKE_ERROR
        """
        return self.write(SHAKE_VO_REG_LOGGING_CTRL, SHAKE_LOGGING_PAUSE)

    def logging_stop(self):
        """
        Stops any current logging operation (playback or recording), and moves
        the internal R/W pointer to the beginning of the memory space. 

        :returns: SHAKE_SUCCESS or SHAKE_ERROR
        """
        if self.logfp:
            self.logfp.close()
        self.logfp = None
        return self.write(SHAKE_VO_REG_LOGGING_CTRL, SHAKE_LOGGING_STOP)

    def logging_record(self):
        """
        Starts logging data to the internal memory from current R/W position.

        :returns: SHAKE_SUCCESS or SHAKE_ERROR
        """
        if self.logfp:
            self.logfp.close()
        self.logfp = None
        return self.write(SHAKE_VO_REG_LOGGING_CTRL, SHAKE_LOGGING_RECORD)

    def logging_reset(self):
        """
        Zeroes the internal clock used to timestamp data logged to the internal
        memory space. Normally called just before recording is started.

        :returns: SHAKE_SUCCESS or SHAKE_ERROR
        """
        return self.write(SHAKE_VO_REG_LOGGING_CTRL, SHAKE_LOGGING_RESET)

    def logging_play(self, filename):
        """
        Begins playback of logged data to a local file. Note that this function
        will always return immediately - this does NOT mean that playback has
        completed. To ensure you receive all of the data, register an event
        callback function before calling this function, and wait for the 
        SHAKE_PLAYBACK_COMPLETE event to be received. 

        :returns: SHAKE_SUCCESS or SHAKE_ERROR
        """
        self.logfp = open(filename, 'w')
        return self.write(SHAKE_VO_REG_LOGGING_CTRL, SHAKE_LOGGING_PLAY)

    def logging_status(self):
        """
        Read the register containing the current state of the logging system.

        :returns: SHAKE_ERROR if an error occurred. Otherwise the value 
            returned indicates the state of the logging system as follows:

            value & SHAKE_LOGGING_RECORDING != 0 # recording in progress
            value & SHAKE_LOGGING_PLAYING != 0 # playback in progress
            value & SHAKE_LOGGING_MEMORYFULL != 0 # memory almost full
        
            If value is 0, logging is not active.
        """
        ret = self.read(SHAKE_VO_REG_LOGGING_STATUS)
        if ret[0] == SHAKE_ERROR:
            return SHAKE_ERROR

        return ret[1]

    def logging_packet_count(self):
        """
        Returns the number of packets logged to internal memory (not an exact
        count).

        :returns: number of packets logged to internal memory. 
        """
        ret = self.read(SHAKE_NV_REG_LOGGING_PKT_LSB)
        if ret[0] == SHAKE_ERROR:
            return SHAKE_ERROR
        lsb = ret[1]

        ret = self.read(SHAKE_NV_REG_LOGGING_PKT_MSB)
        if ret[0] == SHAKE_ERROR:
            return SHAKE_ERROR
        msb = ret[1]

        return 100 * ((msb << 8) + lsb)

    # TODO: is this still available?? (see above)
    # def read_shaking_config(self):
    #    return self.read(SHAKE_NV_REG_SHAKING_CONFIG)
    #
    # def write_shaking_config(self, value):
    #    return self.write(SHAKE_NV_REG_SHAKING_CONFIG, value)
    #
    # def read_shaking_accel_threshold(self):
    #    return self.read(SHAKE_NV_REG_SHAKING_ACCEL_THRESHOLD)
    #
    # def write_shaking_accel_threshold(self, value):
    #    return self.write(SHAKE_NV_REG_SHAKING_ACCEL_THRESHOLD, value)
    #
    # def read_shaking_holdoff_time(self):
    #    return self.read(SHAKE_NV_REG_SHAKING_HOLDOFF_TIME)
    #
    # def write_shaking_holdoff_time(self, value):
    #    return self.write(SHAKE_NV_REG_SHAKING_HOLDOFF_TIME, value)
    #
    # def read_shaking_vibration_profile(self):
    #    return self.read(SHAKE_NV_REG_SHAKING_VIBRATION_PROFILE)
    #
    # def write_shaking_vibration_profile(self, value):
    #    return self.write(SHAKE_NV_REG_SHAKING_VIBRATION_PROFILE, value)
    #
    # def read_shaking_hpf_constant(self):
    #    return self.read(SHAKE_NV_REG_SHAKING_HPF_CONSTANT)
    #
    # def write_shaking_hpf_constant(self, value):
    #    return self.write(SHAKE_NV_REG_SHAKING_HPF_CONSTANT, value)
    #
    # def read_shaking_lpf_constant(self):
    #    return self.read(SHAKE_NV_REG_SHAKING_LPF_CONSTANT)
    #
    # def write_shaking_lpf_constant(self, value):
    #    return self.write(SHAKE_NV_REG_SHAKING_LPF_CONSTANT, value)
    #
    # def reset_shaking_detection(self):
    #    return self.write(SHAKE_NV_REG_SHAKING_CONFIG, 0xFF)
    #    return self.write(SHAKE_NV_REG_SHAKING_ACCEL_THRESHOLD, 0x0A)
    #    return self.write(SHAKE_NV_REG_SHAKING_HOLDOFF_TIME, 0x06)
    #    return self.write(SHAKE_NV_REG_SHAKING_VIBRATION_PROFILE, 0x00)
    #    return self.write(SHAKE_NV_REG_SHAKING_HPF_CONSTANT, 0x20)
    #    return self.write(SHAKE_NV_REG_SHAKING_LPF_CONSTANT, 0x06)

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
        """
        Read the temperature sensor register on the device.

        :returns: -1.0 on error, otherwise a floating point value in the range
            0-64 (in degrees Celsius)
        """
        ret = self.read(SHAKE_VO_REG_TEMPERATURE)
        if ret[0] == SHAKE_ERROR:
            return -1.0

        return ret[1] / 4.0

    def write_packet_request(self, value):
        """
        Write to the 'packet request' register on the device. This allows you
        to request that the device immediately send a single set of data 
        from one or more sensors. This is probably only useful if you don't 
        want to be continually streaming data from these sensors. The returned
        data can be accessed using the the standard functions.

        :param value: a logical OR of one or more SHAKE_REQ_<sensor> constants.
            For example, SHAKE_REQ_ACC | SHAKE_REQ_GYRO would request a set of data
            from both the accelerometer and gyroscope. 
        :returns: SHAKE_SUCCESS or SHAKE_ERROR
        """
        return self.write(SHAKE_VO_REG_PKTREQ, value)

    def write_data_request(self, value):
        return self.write(SHAKE_VO_REG_DATAREQ, value)

    def read_battery_level(self):
        """
        Retrieve the current battery level of the device.

        :returns: SHAKE_ERROR or battery level remaining (0-255).
        """
        (result, bat_level) = self.read(SHAKE_VO_REG_BATTERY)
        if result == SHAKE_ERROR:
            return SHAKE_ERROR
        return bat_level

    def read_power_status(self):
        """
        Retrieve the current power status of the device. This allows you to 
        check if the battery is full, if it is charging and if external power
        is connected. 

        :returns: SHAKE_ERROR if an error occurred. Otherwise the value 
            indicates the power status as follows:

            status & SHAKE_EXT_POWER != 0 # external power connected
            status & SHAKE_BAT_CHARGING != 0 # battery is charging
            status & SHAKE_BAT_FULL != 0 # battery is fully charged
        """
        (result, status) = self.read(SHAKE_VO_REG_PWRSTA)
        if result == SHAKE_ERROR:
            return SHAKE_ERROR
        return status

    def get_ack_timeout_ms(self):
        """
        Get the current time in milliseconds the driver will wait for responses
        to read/write commands. 

        :returns: the current timeout in milliseconds
        """
        return self.ack_timeout_ms

    def set_ack_timeout_ms(self, new_timeout_ms):
        """
        Set the time in milliseconds the driver will wait for responses to 
        read/write commands. Try increasing this if read/write commands seem
        to return errors for no reason, especially if you're streaming a lot of
        sensor data from the device. 

        :param new_timeout_ms: the new timeout in milliseconds
        """
        self.ack_timeout_ms = new_timeout_ms

    def playvib(self, channel, profile):
        """
        Plays a stored vibration profile on a selected channel. 

        :param channel: the channel to play the vibration on. For most standard
            SHAKEs, this should be SHAKE_VIB_MAIN. If your SHAKE has the 
            ForceReactor module installed, you can also use the channel
            SHAKE_VIB_FORCEREACTOR to play vibrations through it. If your SHAKE
            has an external vibration module attached (SK7-VO1), its two 
            channels are identified by SHAKE_VIB_LEFT and SHAKE_VIB_RIGHT.
        :param profile: the number of the vibration profile to activate
        :returns: SHAKE_SUCCESS or SHAKE_ERROR
        """
        if profile < 0 or profile > SHAKE_VIB_PROFILE_MAX:
            return SHAKE_ERROR

        if channel < SHAKE_VIB_MAIN or channel > SHAKE_VIB_EXT_ACTUATOR:
            return SHAKE_ERROR

        if self.device_type == SHAKE_SK6:
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

    def sk6_playvib_continuous(self, channel, amplitude, time):
        """
        (SK6 only) Play a continuous vibration profile on a selected channel. 
        This functionality requires the SK6-VO1 external module and a firmware
        version of 2.60 or later. 

        :param channel: must be either SHAKE_VIB_LEFT or SHAKE_VIB_RIGHT, 
            corresponding to the two channels of the external module.
        :param amplitude: amplitude of the vibration. This must be either 0, 33,
            66 or 100 (percentages of maximum available amplitude). 
        :param time: length of the vibration profile in units of 26ms, allowable
            range is 0-64 (ie 0-1.66s)
        :returns: SHAKE_SUCCESS or SHAKE_ERROR
        """
        if self.device_type != SHAKE_SK6:
            return SHAKE_ERROR

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
            vibbyte = 0x40
        elif amplitude == 66:
            vibbyte = 0x80
        elif amplitude == 100:
            vibbyte = 0xc0

        vibbyte += time

        vibaddr = SHAKE_VO_REG_VIB_LEFT_CONTINUOUS
        if(channel == SHAKE_VIB_RIGHT):
            vibaddr = SHAKE_VO_REG_VIB_RIGHT_CONTINUOUS
        return self.write(vibaddr, vibbyte)

    def sk6_upload_vib_sample(self, profile, samples):
        """
        (SK6 only) Uploads vibration samples to a selected profile slot.

        :param profile: the profile slot number. This should be a number in the
            range SHAKE_VIB_PROFILE_MIN to SHAKE_VIB_PROFILE_MAX (inclusive). 
        :param samples: a list of integers containing the sample data. The list
            should contain successive pairs of amplitude/time values. 
            For example: samples = [amp1, time1, amp2, time2]
            The amplitudes should be values in the range 0-SHAKE_VIB_SPEED_MAX
            (inclusive), while the time values are in units of 10 milliseconds
            and should be in the range 0-SHAKE_VIB_TIME_MAX (inclusive). 
            The maximum number of amplitude/time pairs that can be used is 
            given by SHAKE_VIB_SAMPLE_MAX_LENGTH.
        :returns: SHAKE_SUCCESS or SHAKE_ERROR
        """
        if self.device_type != SHAKE_SK6:
            return SHAKE_ERROR

        return self.upload_vib_sample_extended(profile, samples, 0, 0, 0)


    def upload_vib_sample_extended(self, profile, samples, mode, freq, duty):
        """
        (SK6 with SK6-V01 module, SK7) Uploads extended vibration samples to
        a selected profile slot.

        :param profile: the profile slot number. This should be a number in the
            range SHAKE_VIB_PROFILE_MIN to SHAKE_VIB_PROFILE_MAX (inclusive).
        :param samples: a list of integers containing the sample data. The list
            should contain successive pairs of amplitude/time values. 
            For example: samples = [amp1, time1, amp2, time2]
            The amplitudes should be values in the range 0-SHAKE_VIB_SPEED_MAX
            (inclusive), while the time values are in units of 10 milliseconds
            and should be in the range 0-SHAKE_VIB_TIME_MAX (inclusive). 
            The maximum number of amplitude/time pairs that can be used is 
            given by SHAKE_VIB_SAMPLE_MAX_LENGTH.
        :param mode: the mode to be used when the vibration is played back. This
            should be set to 2 for pulse drive, otherwise 0. 
        :param freq: frequency of the pulsed waveform, 0 if not using pulsed
            vibration playback.
        :param duty: duty ratio for pulsed playback, 1-9 (10-90%) inclusive. Set
            to 0 if not using pulsed vibration playback.
        :returns: SHAKE_SUCCESS or SHAKE_ERROR
        """
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

        timeout = self.ack_timeout_ms
        while self.waiting_for_ack and timeout > 0:
            sleep(0.01)
            timeout -= 10 

        self.waiting_for_ack = False

        if not self.lastack or self.lastaddr == -1:
            return SHAKE_ERROR

        return SHAKE_SUCCESS

