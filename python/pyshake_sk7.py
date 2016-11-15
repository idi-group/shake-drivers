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


import pyshake_sk_common
import string, re
from pyshake_constants import *

SK7_NUM_INFO_LINES = 11
SK7_ASCII_READ_OK = 1
SK7_ASCII_READ_CONTINUE = 2
SK7_ASCII_READ_ERROR = -1
SK7_RAW_READ_OK = 1
SK7_RAW_READ_CONTINUE = 2
SK7_RAW_READ_ERROR = -1

# enum for identifying the lines in the device info message
(
    SK7_HEADER,
    SK7_COPYRIGHT,
    SK7_HARDWARE_REV,
    SK7_FIRMWARE_REV,
    SK7_SERIAL_NUMBER,
    SK7_SLOT0,
    SK7_SLOT1,
    SK7_SLOT2,
    SK7_SLOT3,
    SK7_BLUETOOTH_FIRMWARE,
    SK7_SLOT4
) = range(SK7_NUM_INFO_LINES)

# TODO
SK7_modules = \
    [
        "No option module",
    ]

class SK7(pyshake_sk_common.SHAKE):
    def __init__(self, shakedev, devtype):
        pyshake_sk_common.SHAKE.__init__(self, shakedev, devtype)
        self.__shake = shakedev

    def parse_ascii_packet(self, packet_type, packetbuf, playback, timestamp_packet):
        if packet_type != SK7_ACK_ACK and packet_type != SK7_ACK_NEG:
            self.extract_ascii_packet(packet_type, packetbuf, playback, timestamp_packet)
        else:
            if not self.__shake.waiting_for_ack:
                self.__shake.waiting_for_ack_signal = False
                return SK7_ASCII_READ_ERROR
            
            if packet_type == SK7_ACK_ACK:
                self.__shake.lastack = True
            else:
                self.__shake.lastack = False

            self.parse_ack_packet(packetbuf)

            self.__shake.waiting_for_ack_signal = False

        return SK7_ASCII_READ_OK

    def read_ascii_packet(self, packet_type, packetbuf):
        packet_size, bytes_left, bytes_read = 0, 0, 0
        playback = False
        timestamp = None

        if packet_type == SK7_DATA_TIMESTAMP:
            # this type of packet is only encountered when streaming logged
            # data from the device to the host. it consists of a header, a
            # timestamp, and then a normal packet body. 
            # just extract the timestamp here and then handle the rest as if
            # it was a normal packet (the data is written to file elsewhere)
            
            # read the remainder of the timestamp packet (still have to read
            # the content of the packet afterwards)
            packetbuf += self.read_data(SK7_PACKETS[SK7_DATA_TIMESTAMP].len_bytes - SK7_HEADER_LEN)

            # the format of the timestamp packet is "$TIM,1234567890," where the
            # numbers are a 10-digit timestamp in seconds, with an implicit 
            # decimal point between the 7th and 8th digits (so the above would
            # be 1234567.890 seconds)
            timestamp = float(packetbuf[5:12] + "." + packetbuf[12:15])

            playback = True

            packetbuf = self.read_data(SK7_HEADER_LEN)
            packet_type = self.classify_packet_header(packetbuf, True)
        elif packet_type == SK7_DATA_PLAYBACK_COMPLETE:
            packetbuf += self.read_data(SK7_PACKETS[packet_type].len_bytes - SK7_HEADER_LEN)
            playback = False
            if self.__shake.logfp:
                self.__shake.logfp.close()

            if self.__shake.navcb is not None:
                self.__shake.lastevent = SHAKE_PLAYBACK_COMPLETE
                self.__shake.navcb(self.__shake.lastevent)

            return SK7_ASCII_READ_CONTINUE
        elif packet_type == SK7_DATA_RFID_TID:
            packetbuf += self.read_data(SK7_PACKETS[packet_type].len_bytes - SK7_HEADER_LEN)
            self.__shake.lastrfid = packetbuf[SK7_HEADER_LEN+1:]

            if self.__shake.navcv is not None:
                self.__shake.lastevent = SHAKE_RFID_TID_EVENT
                self.__shake.navcb(self.__shake.lastevent)

            return SK7_ASCII_READ_CONTINUE
        elif packet_type == SK7_STARTUP_INFO:
            self.read_device_info()
            return SK7_ASCII_READ_CONTINUE

        bytes_left = SK7_PACKETS[packet_type].len_bytes - SK7_HEADER_LEN

        if playback:
            bytes_left -= 3

        tmp = self.read_data(bytes_left)
        bytes_read = len(tmp)
        packetbuf += tmp
        if bytes_read != bytes_left:
            return SK7_ASCII_READ_ERROR

        if playback:
            # bit of a workaround to make recorded packets conform to the 
            # format of normal packets
            offset = bytes_read + SK7_HEADER_LEN - 2
            packetbuf = packetbuf[:offset] + ',00\r\n'
            bytes_read += 3

        if SK7_PACKETS[packet_type].has_checksum and ord(packetbuf[bytes_read + SK7_HEADER_LEN - 1]) != 0xA:
            if not self.__shake.checksum:
                self.__shake.checksum = True

            packetbuf += self.read_data(SHAKE_CHECKSUM_LENGTH)
            bytes_read += 3
        elif SK7_PACKETS[packet_type].has_checksum and ord(packetbuf[bytes_read + SK7_HEADER_LEN - 1]) == 0xA and self.__shake.checksum:
            self.__shake.checksum = False

        return self.parse_ascii_packet(packet_type, packetbuf, playback, timestamp)

    def parse_raw_packet(self, packet_type, packetbuf, has_seq):
        self.extract_raw_packet(packet_type, packetbuf, has_seq)

        return SK7_RAW_READ_OK

    def read_raw_packet(self, packet_type, packetbuf):
        packet_size, bytes_left, bytes_read = 0, 0, 0

        bytes_left = SK7_PACKETS[packet_type].len_bytes - SK7_RAW_HEADER_LEN
        tmp = self.read_data(bytes_left)
        bytes_read = len(tmp)
        packetbuf += tmp

        if bytes_left != bytes_read:
            return SHAKE_ERROR

        self.synced = True

        has_seq = False
        self.__shake.peek_flag = False

        if bytes_left == bytes_read:
            trailing_byte = packetbuf[len(packetbuf)-1]

            if ord(trailing_byte) == 0x7F:
                self.__shake.peek = 0x7F
                self.__shake.peek_flag = True
            elif trailing_byte == '$' or trailing_byte == '\n':
                adjtype = packet_type - SK7_RAW_DATA_ACC

                if (ord(trailing_byte) == self.data.internal_timestamps[adjtype] + 1) or (ord(trailing_byte) == 0 and self.data.internal_timestamps[adjtype] == 255):
                    has_seq = True
                else:
                    self.__shake.peek_flag = True
                    self.__shake.peek = ord(trailing_byte)
            else:
                has_seq = True
        else:
            pass

        return self.parse_raw_packet(packet_type, packetbuf, has_seq)

    def get_next_packet(self):
        packetbuf = ""
        bytes_read, packet_type = 0, SHAKE_BAD_PACKET
        tmp = self.read_data(3)
        bytes_read = len(tmp)
        packetbuf += tmp

        if bytes_read == 3:
            if ord(packetbuf[0]) == 0x7F and ord(packetbuf[1]) == 0x7F:
                packet_type = self.classify_packet_header(packetbuf, False)
            elif packetbuf[0] == '$' or packetbuf[0] == '\n':
                packetbuf += self.read_data(1)
                packet_type = self.classify_packet_header(packetbuf, True)
            elif packetbuf.startswith("Log"):
                packetbuf += self.read_data(1)
                packet_type = self.classify_packet_header(packetbuf, True)

        if packet_type == SHAKE_BAD_PACKET:
            if len(packetbuf) == 0:
                # this can occasionally happen when an app using the driver
                # is killed / interrupted
                packetbuf = ""
                c = ""
            elif ord(packetbuf[-1]) != 0x7F and packetbuf[-1] != '$':
                read_count = 50
                c = " "
                while read_count >= 0 and len(c) == 1 and (c != '$' and ord(c) != 0x7F):
                    read_count -= 1
                    c = self.read_data(1)
                packetbuf = c
            else:
                c = packetbuf[-1]
                packetbuf = ""
                
            if c == '$':
                packetbuf = c + self.read_data(SK7_HEADER_LEN - 1)
                packet_type = self.classify_packet_header(packetbuf, True)
            elif len(c) != 0 and ord(c) == 0x7F:
                packetbuf = c + self.read_data(SK7_RAW_HEADER_LEN - 1)
                packet_type = self.classify_packet_header(packetbuf, False)

        return (packet_type, packetbuf)

    def parse_packet(self, packetbuf, packet_type):
        if self.is_ascii_packet(packet_type):
            self.read_ascii_packet(packet_type, packetbuf)
        else:
            self.read_raw_packet(packet_type, packetbuf)

        return 1

    def is_ascii_packet(self, packet_type):
        if packet_type >= SK7_DATA_ACC and packet_type < SK7_RAW_DATA_ACC:
            return True
        return False

    def parse_ack_packet(self, packetbuf):
        try:
            self.__shake.lastaddr = string.atoi(packetbuf[5:9], 16)
        except:
            self.__shake.lastaddr = -1
        self.__shake.lastval = string.atoi(packetbuf[10:12], 16)

    # TODO checksums?
    def extract_ascii_packet(self, packet_type, packetbuf, playback, timestamp):
        if packet_type == SK7_DATA_ACC:
            # fmt: $ACC,+dddd,+dddd,+dddd,ss*CS\r\n
            self.data.accx = int(packetbuf[5:10])
            self.data.accy = int(packetbuf[11:16])
            self.data.accz = int(packetbuf[17:22])
            self.data.internal_timestamps[SHAKE_SENSOR_ACC] = int(packetbuf[23:25])
            if self.__shake.data_callback:
                self.__shake.data_callback(SHAKE_SENSOR_ACC, [self.data.accx, self.data.accy, self.data.accz], self.data.internal_timestamps[SHAKE_SENSOR_ACC])

            if playback and self.__shake.logfp:
                self.__shake.logfp.write('%.3f,ACC,%d,%d,%d,%d\n' % (timestamp, SHAKE_SENSOR_ACC, self.data.accx, self.data.accy, self.data.accz))
        elif packet_type == SK7_DATA_GYRO:
            # fmt: $ARS,+dddd,+dddd,+dddd,ss*CS\r\n
            self.data.gyrx = int(packetbuf[5:10])
            self.data.gyry = int(packetbuf[11:16])
            self.data.gyrz = int(packetbuf[17:22])
            self.data.internal_timestamps[SHAKE_SENSOR_GYRO] = int(packetbuf[23:25])
            if self.__shake.data_callback:
                self.__shake.data_callback(SHAKE_SENSOR_GYRO, [self.data.gyrx, self.data.gyry, self.data.gyrz], self.data.internal_timestamps[SHAKE_SENSOR_GYRO])

            if playback and self.__shake.logfp:
                self.__shake.logfp.write('%.3f,GYR,%d,%d,%d,%d\n' % (timestamp, SHAKE_SENSOR_GYRO, self.data.gyrx, self.data.gyry, self.data.gyrz))
        elif packet_type == SK7_DATA_MAG:
            # fmt: $MAG,+dddd,+dddd,+dddd,ss*CS\r\n
            self.data.magx = int(packetbuf[5:10])
            self.data.magy = int(packetbuf[11:16])
            self.data.magz = int(packetbuf[17:22])
            self.data.internal_timestamps[SHAKE_SENSOR_MAG] = int(packetbuf[23:25])
            if self.__shake.data_callback:
                self.__shake.data_callback(SHAKE_SENSOR_MAG, [self.data.magx, self.data.magy, self.data.magz], self.data.internal_timestamps[SHAKE_SENSOR_MAG])

            if playback and self.__shake.logfp:
                self.__shake.logfp.write('%.3f,MAG,%d,%d,%d,%d\n' % (timestamp, SHAKE_SENSOR_MAG, self.data.magx, self.data.magy, self.data.magz))
        elif packet_type == SK7_DATA_HEADING:
            # $HED,dddd,dd*CS
            self.data.heading = int(packetbuf[5:9])
            self.data.internal_timestamps[SHAKE_SENSOR_HEADING] = int(packetbuf[10:12])
            if self.__shake.data_callback:
                self.__shake.data_callback(SHAKE_SENSOR_HEADING, [self.data.heading], self.data.internal_timestamps[SHAKE_SENSOR_HEADING])

            if playback and self.__shake.logfp:
                self.__shake.logfp.write('%.3f,HED,%d,%d\n' % (timestamp, SHAKE_SENSOR_HEADING, self.data.heading))
        elif packet_type == SK7_DATA_CAP:
            # $CSA,aa,bb,cc,dd,ee,ff,gg,hh,ii,jj,kk,ll,dd*CS[CR][LF] 
            for i in range(12):
                offset = i * 3
                self.data.cap_sk7[0][i] = int(packetbuf[5+offset:7+offset], 16)
            try:
                self.data.internal_timestamps[SHAKE_SENSOR_CAP] = int(packetbuf[42:44])
            except ValueError:
                # can happen with standalone cap sensing boards
                self.data.internal_timestamps[SHAKE_SENSOR_CAP] = 0

            if self.__shake.data_callback:
                self.__shake.data_callback(SHAKE_SENSOR_CAP, [self.data.cap_sk7[0], 0], self.data.internal_timestamps[SHAKE_SENSOR_CAP])

            if playback and self.__shake.logfp:
                s = '%.3f,CAP,%d,\n' % (timestamp, SHAKE_SENSOR_CAP)
                s += '%d' * 12 % tuple(self.data.cap_sk7[0])
                self.__shake.logfp.write(s + '\n')
        elif packet_type == SK7_DATA_CAP_B:
            # $CSB,aa,bb,cc,dd,ee,ff,gg,hh,ii,jj,kk,ll,dd*CS[CR][LF] 
            for i in range(12):
                offset = i * 3
                self.data.cap_sk7[1][i] = int(packetbuf[5+offset:7+offset], 16)
            try:
                self.data.internal_timestamps[SHAKE_SENSOR_CAP] = int(packetbuf[42:44])
            except ValueError:
                # can happen with standalone cap sensing boards
                self.data.internal_timestamps[SHAKE_SENSOR_CAP] = 0

            if self.__shake.data_callback:
                self.__shake.data_callback(SHAKE_SENSOR_CAP, [self.data.cap_sk7[1], 1], self.data.internal_timestamps[SHAKE_SENSOR_CAP])
        elif packet_type == SK7_DATA_CAP_C:
            # $CSC,aa,bb,cc,dd,ee,ff,gg,hh,ii,jj,kk,ll,dd*CS[CR][LF] 
            for i in range(12):
                offset = i * 3
                self.data.cap_sk7[2][i] = int(packetbuf[5+offset:7+offset], 16)
            try:
                self.data.internal_timestamps[SHAKE_SENSOR_CAP] = int(packetbuf[42:44])
            except ValueError:
                # can happen with standalone cap sensing boards
                self.data.internal_timestamps[SHAKE_SENSOR_CAP] = 0

            if self.__shake.data_callback:
                self.__shake.data_callback(SHAKE_SENSOR_CAP, [self.data.cap_sk7[2], 2], self.data.internal_timestamps[SHAKE_SENSOR_CAP])
        elif packet_type == SK7_DATA_ANA0:
            # $AI0,dddd,dd*CS
            self.data.ana0 = int(packetbuf[5:9])
            self.data.internal_timestamps[SHAKE_SENSOR_ANA0] = int(packetbuf[10:12])
            if self.__shake.data_callback:
                self.__shake.data_callback(SHAKE_SENSOR_ANA0, [self.data.ana0], self.data.internal_timestamps[SHAKE_SENSOR_ANA0])

            if playback and self.__shake.logfp:
                self.__shake.logfp.write('%.3f,AI0,%d,%d\n' % (timestamp, SHAKE_SENSOR_ANA0, self.data.ana0))
        elif packet_type == SK7_DATA_ANA1:
            # $AI1,dddd,dd*CS
            self.data.ana1 = int(packetbuf[5:9])
            self.data.internal_timestamps[SHAKE_SENSOR_ANA1] = int(packetbuf[10:12])
            if self.__shake.data_callback:
                self.__shake.data_callback(SHAKE_SENSOR_ANA1, [self.data.ana1], self.data.internal_timestamps[SHAKE_SENSOR_ANA1])

            if playback and self.__shake.logfp:
                self.__shake.logfp.write('%.3f,AI1,%d,%d\n' % (timestamp, SHAKE_SENSOR_ANA1, self.data.ana1))
        elif packet_type == SK7_DATA_RPH:
            self.data.rph[0] = int(packetbuf[5:10])
            self.data.rph[1] = int(packetbuf[11:16])
            self.data.rph[2] = int(packetbuf[17:22])

            if playback and self.__shake.logfp:
                self.__shake.logfp.write('%.3f,RPH,%d,%d,%d,%d\n' % (timestamp, SHAKE_SENSOR_HEADING, self.data.rph[0], self.data.rph[1], self.data.rph[2]))
        elif packet_type == SK7_DATA_RPH_QUATERNION:
            self.data.rphq[0] = float(packetbuf[5:13])
            self.data.rphq[1] = float(packetbuf[14:22])
            self.data.rphq[2] = float(packetbuf[23:31])
            self.data.rphq[3] = float(packetbuf[32:40])

            if playback and self.__shake.logfp:
                self.__shake.logfp.write('%.3f,QTN,%d,%f,%f,%f,%f\n' % (timestamp, SHAKE_SENSOR_HEADING, self.data.rphq[0], self.data.rphq[1], self.data.rphq[2], self.data.rphq[3]))
        elif packet_type == SK7_DATA_GYRO_TEMP:
            self.data.temps[0] = int(packetbuf[5:10])
            self.data.temps[1] = int(packetbuf[11:16])
            self.data.temps[2] = int(packetbuf[17:22])
    
            if playback and self.__shake.logfp:
                self.__shake.logfp.write('%.3f,GOT,%d,%f,%f,%f\n' % (timestamp, SHAKE_SENSOR_GYRO_TEMPS, self.data.temps[0], self.data.temps[1], self.data.temps[2]))

        elif packet_type >= SK7_DATA_NVU and packet_type <= SK7_DATA_NVN:
            if self.__shake.navcb is not None:
                event = -1
                if packetbuf[3] == 'U':
                    event = SHAKE_NAV_UP
                elif packetbuf[3] == 'D':
                    event = SHAKE_NAV_DOWN
                elif packetbuf[3] == 'C':
                    event = SHAKE_NAV_CENTRE
                elif packetbuf[3] == 'N':
                    event = SHAKE_NAV_NORMAL
                self.__shake.navcb(event)
        elif packet_type >= SK7_DATA_CU0 and packet_type <= SK7_DATA_CLB:
            if self.__shake.navcb is not None:
                event = SK7_CS0_UPPER + (packet_type - SK7_DATA_CU0)
                self.__shake.navcb(event)
        elif packet_type == SK7_DATA_SHAKING:
            self.data.shaking_peakaccel = int(packetbuf[5:10])
            self.data.shaking_direction = int(packetbuf[11:16])
            self.data.shaking_timestamp = int(packetbuf[17:22])
            if self.__shake.navcb is not None:
                self.__shake.navcb(SHAKE_SHAKING_EVENT)
        elif packet_type == SK7_DATA_HEART_RATE:
            self.data.hr_bpm = int(packetbuf[5:9])
            if self.__shake.navcb is not None:
                self.__shake.navcb(SHAKE_HEART_RATE_EVENT)
        elif packet_type == SK7_DATA_RFID_TID:
            self.data.tid = packetbuf[5:21]
            if self.__shake.navcb is not None:
                self.__shake.navcb(SHAKE_RFID_TID_EVENT)
        else:
            return SHAKE_ERROR

        return SHAKE_SUCCESS

    def extract_raw_packet(self, packet_type, packetbuf, has_seq):
        if packet_type == SK7_RAW_DATA_ACC:
            self.data.accx = pyshake_sk_common.convert_raw_data_value(packetbuf[3:5])
            self.data.accy = pyshake_sk_common.convert_raw_data_value(packetbuf[5:7])
            self.data.accz = pyshake_sk_common.convert_raw_data_value(packetbuf[7:9])
            if has_seq:
                self.data.internal_timestamps[SHAKE_SENSOR_ACC] = ord(packetbuf[9:10])
            if self.__shake.data_callback:
                self.__shake.data_callback(SHAKE_SENSOR_ACC, [self.data.accx, self.data.accy, self.data.accz], self.data.internal_timestamps[SHAKE_SENSOR_ACC])
        elif packet_type == SK7_RAW_DATA_GYRO:
            self.data.gyrx = pyshake_sk_common.convert_raw_data_value(packetbuf[3:5])
            self.data.gyry = pyshake_sk_common.convert_raw_data_value(packetbuf[5:7])
            self.data.gyrz = pyshake_sk_common.convert_raw_data_value(packetbuf[7:9])
            if has_seq:
                self.data.internal_timestamps[SHAKE_SENSOR_GYRO] = ord(packetbuf[9:10])
            if self.__shake.data_callback:
                self.__shake.data_callback(SHAKE_SENSOR_GYRO, [self.data.gyrx, self.data.gyry, self.data.gyrz], self.data.internal_timestamps[SHAKE_SENSOR_GYRO])
        elif packet_type == SK7_RAW_DATA_MAG:
            self.data.magx = pyshake_sk_common.convert_raw_data_value(packetbuf[3:5])
            self.data.magy = pyshake_sk_common.convert_raw_data_value(packetbuf[5:7])
            self.data.magz = pyshake_sk_common.convert_raw_data_value(packetbuf[7:9])
            if has_seq:
                self.data.internal_timestamps[SHAKE_SENSOR_MAG] = ord(packetbuf[9:10])
            if self.__shake.data_callback:
                self.__shake.data_callback(SHAKE_SENSOR_MAG, [self.data.magx, self.data.magy, self.data.magz], self.data.internal_timestamps[SHAKE_SENSOR_MAG])
        elif packet_type == SK7_RAW_DATA_HEADING:
            self.data.heading = pyshake_sk_common.convert_raw_data_value(packetbuf[3:5])
            if has_seq:
                self.data.internal_timestamps[SHAKE_SENSOR_HEADING] = ord(packetbuf[5:6])
            if self.__shake.data_callback:
                self.__shake.data_callback(SHAKE_SENSOR_HEADING, [self.data.heading], self.data.internal_timestamps[SHAKE_SENSOR_HEADING])
        elif packet_type == SK7_RAW_DATA_CAP:
            for i in range(12):
                self.data.cap_sk7[0][i] = ord(packetbuf[SK7_RAW_HEADER_LEN+i])
            if self.__shake.data_callback:
                self.__shake.data_callback(SHAKE_SENSOR_CAP, [self.data.cap_sk7[0], 0], self.data.internal_timestamps[SHAKE_SENSOR_CAP])
        elif packet_type == SK7_RAW_DATA_CAP_B:
            for i in range(12):
                self.data.cap_sk7[1][i] = ord(packetbuf[SK7_RAW_HEADER_LEN+i])
            if self.__shake.data_callback:
                self.__shake.data_callback(SHAKE_SENSOR_CAP, [self.data.cap_sk7[1], 1], self.data.internal_timestamps[SHAKE_SENSOR_CAP])
        elif packet_type == SK7_RAW_DATA_CAP_C:
            for i in range(12):
                self.data.cap_sk7[2][i] = ord(packetbuf[SK7_RAW_HEADER_LEN+i])
            if self.__shake.data_callback:
                self.__shake.data_callback(SHAKE_SENSOR_CAP, [self.data.cap_sk7[2], 2], self.data.internal_timestamps[SHAKE_SENSOR_CAP])
        elif packet_type == SK7_RAW_DATA_ANALOG0:
            self.data.ana0 = pyshake_sk_common.convert_raw_data_value(packetbuf[3:5])
            if has_seq:
                self.data.internal_timestamps[SHAKE_SENSOR_ANA0] = ord(packetbuf[5:6])
            if self.__shake.data_callback:
                self.__shake.data_callback(SHAKE_SENSOR_ANA0, [self.data.ana0], self.data.internal_timestamps[SHAKE_SENSOR_ANA0])
        elif packet_type == SK7_RAW_DATA_ANALOG1:
            self.data.ana1 = pyshake_sk_common.convert_raw_data_value(packetbuf[3:5])
            if has_seq:
                self.data.internal_timestamps[SHAKE_SENSOR_ANA1] = ord(packetbuf[5:6])
            if self.__shake.data_callback:
                self.__shake.data_callback(SHAKE_SENSOR_ANA1, [self.data.ana1], self.data.internal_timestamps[SHAKE_SENSOR_ANA1])
        elif packet_type == SK7_RAW_DATA_RPH:
            self.data.rph[0] = pyshake_sk_common.convert_raw_data_value(packetbuf[3:5])
            self.data.rph[1] = pyshake_sk_common.convert_raw_data_value(packetbuf[5:7])
            self.data.rph[2] = pyshake_sk_common.convert_raw_data_value(packetbuf[7:9])
        elif packet_type == SK7_RAW_DATA_RPH_QUATERNION:
            self.data.rphq[0] = pyshake_sk_common.convert_raw_data_value(packetbuf[3:5])
            self.data.rphq[1] = pyshake_sk_common.convert_raw_data_value(packetbuf[5:7])
            self.data.rphq[2] = pyshake_sk_common.convert_raw_data_value(packetbuf[7:9])
            self.data.rphq[3] = pyshake_sk_common.convert_raw_data_value(packetbuf[9:11])
        elif packet_type == SK7_RAW_DATA_GYRO_TEMP:
            # pitch, roll, yaw axes + acc/mag sensor temperature
            self.data.gyro_temps[0] = pyshake_sk_common.convert_raw_data_value(packetbuf[3:5])
            self.data.gyro_temps[1] = pyshake_sk_common.convert_raw_data_value(packetbuf[5:7])
            self.data.gyro_temps[2] = pyshake_sk_common.convert_raw_data_value(packetbuf[7:9])
            self.data.gyro_temps[3] = pyshake_sk_common.convert_raw_data_value(packetbuf[9:11])
        elif packet_type == SK7_RAW_DATA_EVENT:
            if self.__shake.navcb is not None:
                event = -1
                val = pyshake_sk_common.convert_raw_data_value(packetbuf[3:5])
                if val == 1:
                    event = SHAKE_NAV_UP
                elif val == 2:
                    event = SHAKE_NAV_DOWN
                elif val == 3:
                    event = SHAKE_NAV_CENTRE
                elif val == 4:
                    event = SHAKE_NAV_NORMAL
                else:
                    event = SK7_CS0_UPPER + (val - SK7_DATA_CU0)
                self.__shake.navcb(event)
        elif packet_type == SK7_RAW_DATA_SHAKING:
            if self.__shake.navcb is not None:
                self.__shake.navcb(SHAKE_SHAKING_EVENT)
        elif packet_type >= SK7_RAW_DATA_IMU0 and packet_type <= SK7_RAW_DATA_IMU4:
            imu = packet_type - SK7_RAW_DATA_IMU0
            self.data.imudata[imu].acc = [pyshake_sk_common.convert_raw_data_value(packetbuf[3+(i*2):5+(i*2)]) for i in range(3)]
            self.data.imudata[imu].gyro = [pyshake_sk_common.convert_raw_data_value(packetbuf[9+(i*2):11+(i*2)]) for i in range(3)]
            self.data.imudata[imu].mag = [pyshake_sk_common.convert_raw_data_value(packetbuf[15+(i*2):17+(i*2)]) for i in range(3)]
            self.data.imudata[imu].temp = pyshake_sk_common.convert_raw_data_value(packetbuf[21:23])
            self.data.imudata[imu].seq = ord(packetbuf[23])
            self.data.internal_timestamps[SHAKE_SENSOR_IMU0 + imu] = self.data.imudata[imu].seq
            if self.__shake.data_callback:
                self.__shake.data_callback(SHAKE_SENSOR_IMU0 + imu, self.data.imudata[imu], self.data.internal_timestamps[SHAKE_SENSOR_IMU0 + imu]) 
        else:
            return SHAKE_ERROR

        return SHAKE_SUCCESS

    def classify_packet_header(self, packetbuf, ascii_packet):
        type = SHAKE_BAD_PACKET

        if ascii_packet and (packetbuf is None or len(packetbuf) != SK7_HEADER_LEN):
            return type
        if not ascii_packet and (packetbuf is None or len(packetbuf) != SK7_RAW_HEADER_LEN or ord(packetbuf[0]) != 0x7F or ord(packetbuf[1]) != 0x7F):
            return type

        i = 0
        if not ascii_packet:
            i = SK7_RAW_DATA_ACC

        while i < SK7_NUM_PACKET_TYPES:
            if ascii_packet:
                if SK7_PACKETS[i].match_header(packetbuf[:SK7_HEADER_LEN]):
                    return SK7_PACKETS[i].typeid
                if i >= SK7_STARTUP_INFO: # last non-raw packet type
                    return type
            else:
                if SK7_PACKETS[i].match_header(packetbuf[2]):
                    return SK7_PACKETS[i].typeid
                
            i += 1
        return type

    def read_device_info(self):
        for i in range(SK7_NUM_INFO_LINES):
            line = self.read_info_line()
            if line is None:
                return False
            
            if i == SK7_FIRMWARE_REV:   
                # eg: Firmware Revision 02.37
                res = re.search("\d", line)
                if res:
                    self.__shake.fwrev = float(line[res.start():res.start()+5])
            elif i == SK7_HARDWARE_REV:     
                # eg: Hardware Revision 01.00
                res = re.search("\d", line)
                if res:
                    self.__shake.hwrev = float(line[res.start():res.start()+5])
            elif i == SK7_SERIAL_NUMBER:    
                # eg: Serial Number 0077

                res = re.search("\d", line)
                if res:
                    self.__shake.serial = line[res.start():res.start()+4]
            elif i >= SK7_SLOT0 and i <= SK7_SLOT3:
                # TODO just store names for now, ideally map these to an enum 
                # instead to make testing for presence of a specific module 
                # easier to do
                self.__shake.modules[i - SK7_SLOT0] = line.strip()
            elif i == SK7_BLUETOOTH_FIRMWARE:
                # eg: Bluetooth Firmware Revision 00.02
                res = re.search("\d", line)
                if res:
                    self.__shake.bluetoothfwrev = float(line[res.start():res.start()+5])
            elif i == SK7_SLOT4:
                self.__shake.modules[4] = line.strip()
    
        tmp = self.read_data(1)
        self.synced = True
        return True

