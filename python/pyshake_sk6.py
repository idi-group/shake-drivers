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
import string
from pyshake_constants import *

SK6_NUM_INFO_LINES = 7
SK6_ASCII_READ_OK = 1
SK6_ASCII_READ_CONTINUE = 2
SK6_ASCII_READ_ERROR = -1
SK6_RAW_READ_OK = 1
SK6_RAW_READ_CONTINUE = 2
SK6_RAW_READ_ERROR = -1

(
    SK6_HEADER,
    SK6_COPYRIGHT,
    SK6_FIRMWARE_REV,
    SK6_HARDWARE_REV,
    SK6_SERIAL_NUMBER,
    SK6_EXPANSION_SLOT1,
    SK6_EXPANSION_SLOT2
) = range(SK6_NUM_INFO_LINES)

SK6_modules =   [
                    "No option module",
                    "Gyro Module SK6-G01",
                    "Gyro Module SK6-G02",
                    "Gyro Module SK6-R01",
                    "No option module",
                    "Audio module SK6-A01",
                    "External vibration module SK6-V01",
                    "Polar Heart Rate Receiver Module SK6-H01",
                    "USB Module SK6-U01",
                    "Expansion Module SK6-E01"
                ]

class SK6(pyshake_sk_common.SHAKE):
    def __init__(self, shakedev, devtype):
        pyshake_sk_common.SHAKE.__init__(self, shakedev, devtype)
        self.__shake = shakedev

    def parse_ascii_packet(self, packet_type, packetbuf, playback, timestamp_packet):
        if packet_type != SK6_ACK_ACK and packet_type != SK6_ACK_NEG:
            self.extract_ascii_packet(packet_type, packetbuf, playback, timestamp_packet)
        else:
            if not self.__shake.waiting_for_ack:
                self.__shake.waiting_for_ack_signal = False
                return SK6_ASCII_READ_ERROR
            
            if packet_type == SK6_ACK_ACK:
                self.__shake.lastack = True
            else:
                self.__shake.lastack = False

            self.parse_ack_packet(packetbuf)

            self.__shake.waiting_for_ack_signal = False

        return SK6_ASCII_READ_OK

    def read_ascii_packet(self, packet_type, packetbuf):
        packet_size, bytes_left, bytes_read = 0,0,0
        playback = False
        timestamp_packet = None

        if packet_type == SK6_DATA_TIMESTAMP:
            # TODO
            return SK6_ASCII_READ_ERROR
        elif packet_type == SK6_DATA_PLAYBACK_COMPLETE:
            packetbuf += self.read_data(sk6_packet_lengths[packet_type] - SK6_HEADER_LEN)
            
            if self.__shake.navcb != None:
                self.__shake.lastevent = SHAKE_PLAYBACK_COMPLETE
                self.__shake.navcb(self.__shake.lastevent)

            return SK6_ASCII_READ_CONTINUE
        elif packet_type == SK6_DATA_RFID_TID:
            packetbuf += self.read_data(sk6_packet_lengths[packet_type] - SK6_HEADER_LEN)
            self.__shake.lastrfid = packetbuf[SK6_HEADER_LEN+1:]

            if self.__shake.navcv != None:
                self.__shake.lastevent = SHAKE_RFID_TID_EVENT
                self.__shake.navcb(self.__shake.lastevent)

            return SK6_ASCII_READ_CONTINUE
        elif packet_type == SK6_STARTUP_INFO:
            self.read_device_info()
            return SK6_ASCII_READ_CONTINUE

        bytes_left = sk6_packet_lengths[packet_type] - SK6_HEADER_LEN

        if playback:
            bytes_left -= 3

        tmp = self.read_data(bytes_left)
        bytes_read = len(tmp)
        packetbuf += tmp
        if bytes_read != bytes_left:
            return SK6_ASCII_READ_ERROR

        if playback:
            offset = bytes_read + SK6_HEADER_LEN - 2
            # TODO
            bytes_read += 3

        if sk6_packet_has_checksum[packet_type] and ord(packetbuf[bytes_read + SK6_HEADER_LEN - 1]) != 0xA:
            if not self.__shake.checksum:
                self.__shake.checksum = True

            packetbuf += self.read_data(SHAKE_CHECKSUM_LENGTH)
            bytes_read += 3
        elif sk6_packet_has_checksum[packet_type] and ord(packetbuf[bytes_read + SK6_HEADER_LEN - 1]) == 0xA and self.__shake.checksum:
            self.__shake.checksum = False

        return self.parse_ascii_packet(packet_type, packetbuf, playback, timestamp_packet)

    def parse_raw_packet(self, packet_type, packetbuf, has_seq):
        self.extract_raw_packet(packet_type, packetbuf, has_seq)
        if packet_type == SK6_RAW_DATA_AUDIO_HEADER:
            # TODO compress and send audio
            pass

        return SK6_RAW_READ_OK

    def read_raw_packet(self, packet_type, packetbuf):
        packet_size, bytes_left, bytes_read = 0,0,0

        bytes_left = sk6_packet_lengths[packet_type] - SK6_RAW_HEADER_LEN
        tmp = self.read_data(bytes_left)
        bytes_read = len(tmp)
        packetbuf += tmp

        if bytes_left != bytes_read:
            return SK6_RAW_READ_ERROR

        has_seq = False
        self.__shake.peek_flag = False

        if bytes_left == bytes_read:
            packet_len = sk6_packet_lengths[packet_type]
            trailing_byte = packetbuf[len(packetbuf)-1]

            if ord(trailing_byte) == 0x7F:
                self.__shake.peek = 0x7F
                self.__shake.peek_flag = True
            elif trailing_byte == '$' or trailing_byte == '\n':
                adjtype = packet_type - SK6_RAW_DATA_ACC

                if (trailing_byte == self.data.internal_timestamps[adjtype] + 1) or (trailing_byte == 0 and self.data.internal_timestamps[adjtype] == 255):
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

        if packet_type == SHAKE_BAD_PACKET:
            read_count = 50
            c = " "
            while read_count < 50 and len(c) == 1 and (c != '$' and ord(c) != 0x7F):
                read_count -= 1
                c = self.read_data(1)

            packetbuf = c
            if c == '$':
                packetbuf += self.read_data( SK6_HEADER_LEN - 1)
                packet_type = self.classify_packet_header(packetbuf, True)
            elif len(c) != 0 and ord(c) == 0x7F:
                packetbuf += self.read_data(SK6_RAW_HEADER_LEN -1)
                packet_type = self.classify_packet_header(packetbuf, False)

        return (packet_type, packetbuf)

    def parse_packet(self, packetbuf, packet_type):
        if self.is_ascii_packet(packet_type):
            self.read_ascii_packet(packet_type, packetbuf)
        else:
            self.read_raw_packet(packet_type, packetbuf)

        return 1

    def is_ascii_packet(self, packet_type):
        if packet_type >= SK6_DATA_ACC and packet_type < SK6_RAW_DATA_ACC:
            return True
        return False

    def parse_ack_packet(self, packetbuf):
        try:
            self.__shake.lastaddr = string.atoi(packetbuf[5:9], 16)
        except:
            self.__shake.lastaddr = -1
        self.__shake.lastval = string.atoi(packetbuf[10:12], 16)

    # TODO checksums?
    def extract_ascii_packet(self, packet_type, packetbuf, playback, timestamp_packet):
        if packet_type == SK6_DATA_ACC:
            # fmt: $ACC,+dddd,+dddd,+dddd,ss*CS\r\n
            self.data.accx = int(packetbuf[5:10])
            self.data.accy = int(packetbuf[11:16])
            self.data.accz = int(packetbuf[17:22])
            self.data.internal_timestamps[SHAKE_SENSOR_ACC] = int(packetbuf[23:25])
            if self.__shake.data_callback:
                self.__shake.data_callback(SHAKE_SENSOR_ACC, [self.data.accx, self.data.accy, self.data.accz], self.data.internal_timestamps[SHAKE_SENSOR_ACC])
        elif packet_type == SK6_DATA_GYRO:
            # fmt: $ARS,+dddd,+dddd,+dddd,ss*CS\r\n
            self.data.gyrx = int(packetbuf[5:10])
            self.data.gyry = int(packetbuf[11:16])
            self.data.gyrz = int(packetbuf[17:22])
            self.data.internal_timestamps[SHAKE_SENSOR_GYRO] = int(packetbuf[23:25])
            if self.__shake.data_callback:
                self.__shake.data_callback(SHAKE_SENSOR_GYRO, [self.data.gyrx, self.data.gyry, self.data.gyrz], self.data.internal_timestamps[SHAKE_SENSOR_GYRO])
        elif packet_type == SK6_DATA_MAG:
            # fmt: $MAG,+dddd,+dddd,+dddd,ss*CS\r\n
            self.data.magx = int(packetbuf[5:10])
            self.data.magy = int(packetbuf[11:16])
            self.data.magz = int(packetbuf[17:22])
            self.data.internal_timestamps[SHAKE_SENSOR_MAG] = int(packetbuf[23:25])
            if self.__shake.data_callback:
                self.__shake.data_callback(SHAKE_SENSOR_MAG, [self.data.magx, self.data.magy, self.data.magz], self.data.internal_timestamps[SHAKE_SENSOR_MAG])
        elif packet_type == SK6_DATA_HEADING:
            # $HED,dddd,dd*CS
            self.data.heading = int(packetbuf[5:9])
            self.data.internal_timestamps[SHAKE_SENSOR_HEADING] = int(packetbuf[10:12])
            if self.__shake.data_callback:
                self.__shake.data_callback(SHAKE_SENSOR_HEADING, [self.data.heading], self.data.internal_timestamps[SHAKE_SENSOR_HEADING])
        elif packet_type == SK6_DATA_CAP0:
            # $CS0,dddd,dd*CS[CR][LF]
            self.data.cap_sk6[0] = int(packetbuf[5:9])
            self.data.internal_timestamps[SHAKE_SENSOR_CAP0] = int(packetbuf[10:12])
            if self.__shake.data_callback:
                self.__shake.data_callback(SHAKE_SENSOR_CAP0, [self.data.cap_sk6[0]], self.data.internal_timestamps[SHAKE_SENSOR_CAP0])
        elif packet_type == SK6_DATA_CAP1:
            # $CS1,dddd,dd*CS[CR][LF]
            self.data.cap_sk6[1] = int(packetbuf[5:9])
            self.data.internal_timestamps[SHAKE_SENSOR_CAP1] = int(packetbuf[10:12])
            if self.__shake.data_callback:
                self.__shake.data_callback(SHAKE_SENSOR_CAP1, [self.data.cap_sk6[1]], self.data.internal_timestamps[SHAKE_SENSOR_CAP1])
        elif packet_type == SK6_DATA_ANA0:
            # $AI0,dddd,dd*CS
            self.ana0 = int(packetbuf[5:9])
            self.data.internal_timestamps[SHAKE_SENSOR_ANA0] = int(packetbuf[10:12])
            if self.__shake.data_callback:
                self.__shake.data_callback(SHAKE_SENSOR_ANA0, [self.data.ana0], self.data.internal_timestamps[SHAKE_SENSOR_ANA0])
        elif packet_type == SK6_DATA_ANA1:
            # $AI1,dddd,dd*CS
            self.ana1 = int(packetbuf[5:9])
            self.data.internal_timestamps[SHAKE_SENSOR_ANA1] = int(packetbuf[10:12])
            if self.__shake.data_callback:
                self.__shake.data_callback(SHAKE_SENSOR_ANA1, [self.data.ana1], self.data.internal_timestamps[SHAKE_SENSOR_ANA1])
        elif packet_type >= SK6_DATA_NVU and packet_type <= SK6_DATA_NVN:
            if self.__shake.navcb != None:
                event = -1
                if packet[3] == 'U':
                    event = SHAKE_NAV_UP
                elif packet[3] == 'D':
                    event = SHAKE_NAV_DOWN
                elif packet[3] == 'C':
                    event = SHAKE_NAV_CENTRE
                elif packet[3] == 'N':
                    event = SHAKE_NAV_NORMAL
                self.__shake.navcb(event)
        elif packet_type == SK6_DATA_CU0:
            if self.__shake.navcb != None:
                self.__shake.navcb(SHAKE_CS0_UPPER)
        elif packet_type == SK6_DATA_CL0:
            if self.__shake.navcb != None:
                self.__shake.navcb(SHAKE_CS0_LOWER)
        elif packet_type == SK6_DATA_CU1:
            if self.__shake.navcb != None:
                self.__shake.navcb(SHAKE_CS1_UPPER)
        elif packet_type == SK6_DATA_CL1:
            if self.__shake.navcb != None:
                self.__shake.navcb(SHAKE_CS1_LOWER)
        elif packet_type == SK6_DATA_SHAKING:
            self.data.shaking_peakaccel = int(packetbuf[5:10])
            self.data.shaking_direction = int(packetbuf[11:16])
            self.data.shaking_timestamp = int(packetbuf[17:22])
            if self.__shake.navcb != None:
                self.__shake.navcb(SHAKE_SHAKING_EVENT)
        elif packet_type == SK6_DATA_HEART_RATE:
            self.data.hr_bpm = int(packetbuf[5:9])
            if self.__shake.navcb != None:
                self.__shake.navcb(SHAKE_HEART_RATE_EVENT)
        elif packet_type == SK6_DATA_RFID_TID:
            self.data.tid = packetbuf[5:21]
            if self.__shake.navcb != None:
                self.__shake.navcb(SHAKE_RFID_TID_EVENT)
        else:
            return SHAKE_ERROR

        return SHAKE_SUCCESS

    def extract_raw_packet(self, packet_type, packetbuf, has_seq):
        if packet_type == SK6_RAW_DATA_ACC:
            self.data.accx = pyshake_sk_common.convert_raw_data_value(packetbuf[3:5])
            self.data.accy = pyshake_sk_common.convert_raw_data_value(packetbuf[5:7])
            self.data.accz = pyshake_sk_common.convert_raw_data_value(packetbuf[7:9])
            if has_seq:
                self.data.internal_timestamps[SHAKE_SENSOR_ACC] = ord(packetbuf[9:10])
            if self.__shake.data_callback:
                self.__shake.data_callback(SHAKE_SENSOR_ACC, [self.data.accx, self.data.accy, self.data.accz], self.data.internal_timestamps[SHAKE_SENSOR_ACC])
        elif packet_type == SK6_RAW_DATA_GYRO:
            self.data.gyrx = pyshake_sk_common.convert_raw_data_value(packetbuf[3:5])
            self.data.gyry = pyshake_sk_common.convert_raw_data_value(packetbuf[5:7])
            self.data.gyrz = pyshake_sk_common.convert_raw_data_value(packetbuf[7:9])
            if has_seq:
                self.data.internal_timestamps[SHAKE_SENSOR_GYRO] = ord(packet[9:10])
            if self.__shake.data_callback:
                self.__shake.data_callback(SHAKE_SENSOR_GYRO, [self.data.gyrx, self.data.gyry, self.data.gyrz], self.data.internal_timestamps[SHAKE_SENSOR_GYRO])
        elif packet_type == SK6_RAW_DATA_MAG:
            self.data.magx = pyshake_sk_common.convert_raw_data_value(packetbuf[3:5])
            self.data.magy = pyshake_sk_common.convert_raw_data_value(packetbuf[5:7])
            self.data.magz = pyshake_sk_common.convert_raw_data_value(packetbuf[7:9])
            if has_seq:
                self.data.internal_timestamps[SHAKE_SENSOR_MAG] = ord(packetbuf[9:10])
            if self.__shake.data_callback:
                self.__shake.data_callback(SHAKE_SENSOR_MAG, [self.data.magx, self.data.magy, self.data.magz], self.data.internal_timestamps[SHAKE_SENSOR_MAG])
        elif packet_type == SK6_RAW_DATA_HEADING:
            self.data.heading = pyshake_sk_common.convert_raw_data_value(packetbuf[3:5])
            if has_seq:
                self.data.internal_timestamps[SHAKE_SENSOR_HEADING] = ord(packetbuf[5:6])
            if self.__shake.data_callback:
                self.__shake.data_callback(SHAKE_SENSOR_HEADING, [self.data.heading], self.data.internal_timestamps[SHAKE_SENSOR_HEADING])
        elif packet_type == SK6_RAW_DATA_CAP0:
            self.data.cap_sk6[0] = pyshake_sk_common.convert_raw_data_value(packetbuf[3:5])
            if has_seq:
                self.data.internal_timestamps[SHAKE_SENSOR_CAP0] = ord(packetbuf[5:6])
            if self.__shake.data_callback:
                self.__shake.data_callback(SHAKE_SENSOR_CAP0, [self.data.cap_sk6[0]], self.data.internal_timestamps[SHAKE_SENSOR_CAP0])
        elif packet_type == SK6_RAW_DATA_CAP1:
            self.data.cap_sk6[1] = pyshake_sk_common.convert_raw_data_value(packetbuf[3:5])
            if has_seq:
                self.data.internal_timestamps[SHAKE_SENSOR_CAP1] = ord(packetbuf[5:6])
            if self.__shake.data_callback:
                self.__shake.data_callback(SHAKE_SENSOR_CAP1, [self.data.cap_sk6[1]], self.data.internal_timestamps[SHAKE_SENSOR_CAP1])
        elif packet_type == SK6_RAW_DATA_ANALOG0:
            self.data.ana0 = pyshake_sk_common.convert_raw_data_value(packetbuf[3:5])
            if has_seq:
                self.data.internal_timestamps[SHAKE_SENSOR_ANA0] = ord(packetbuf[5:6])
            if self.__shake.data_callback:
                self.__shake.data_callback(SHAKE_SENSOR_ANA0, [self.data.ana0], self.data.internal_timestamps[SHAKE_SENSOR_ANA0])
        elif packet_type == SK6_RAW_DATA_ANALOG1:
            self.data.ana1 = pyshake_sk_common.convert_raw_data_value(packetbuf[3:5])
            if has_seq:
                self.data.internal_timestamps[SHAKE_SENSOR_ANA1] = ord(packetbuf[5:6])
            if self.__shake.data_callback:
                self.__shake.data_callback(SHAKE_SENSOR_ANA1, [self.data.ana1], self.data.internal_timestamps[SHAKE_SENSOR_ANA1])
        elif packet_type == SK6_RAW_DATA_EVENT:
            if self.__shake.navcb != None:
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
                self.__shake.navcb(event)
        elif packet_type == SK6_RAW_DATA_SHAKING:
            if self.__shake.navcb != None:
                self.__shake.navcb(SHAKE_SHAKING_EVENT)
        else:
            return SHAKE_ERROR

        return SHAKE_SUCCESS

    def classify_packet_header(self, packetbuf, ascii_packet):
        type = SHAKE_BAD_PACKET

        if ascii_packet and (packetbuf == None or len(packetbuf) != SK6_HEADER_LEN):
            return type
        if not ascii_packet and (packetbuf == None or len(packetbuf) != SK6_RAW_HEADER_LEN or ord(packetbuf[0]) != 0x7F or ord(packetbuf[1]) != 0x7F):
            return type

        i = 0
        if not ascii_packet:
            i = SK6_RAW_DATA_ACC

        while i < SK6_NUM_PACKET_TYPES:
            if ascii_packet:
                if packetbuf[:SK6_HEADER_LEN] == sk6_packet_headers[i]:
                    type = i
                    return type
                if i >= SK6_STARTUP_INFO:
                    return type
            else:
                if ord(packetbuf[2]) == sk6_raw_packet_headers[i - SK6_RAW_DATA_ACC]:
                    type = i
                    return type

            i += 1
        return type

    def read_device_info(self):
        for i in range(7):
            line = self.read_info_line()
            if line == None:
                return False
            
            if i == SK6_FIRMWARE_REV:       # firmware revision line
                # find start of revision number and get the value
                pos = 0
                while (ord(line[pos]) != 0xA and ord(line[pos]) != 0xD) and (line[pos] < '0' or line[pos] > '9'):
                    pos += 1

                try:
                    self.__shake.fwrev = float((line[pos:pos+4]))
                except ValueError:
                    self.__shake.fwrev = None
            elif i == SK6_HARDWARE_REV:     # hardware revision line
                # find start of revision number and get the value
                pos = 0
                while (ord(line[pos]) != 0xA and ord(line[pos]) != 0xD) and (line[pos] < '0' or line[pos] > '9'):
                    pos += 1

                try:
                    self.__shake.hwrev = float((line[pos:pos+4]))
                except ValueError:
                    self.__shake.hwrev = None
            elif i == SK6_SERIAL_NUMBER:    # serial number
                # find start of serial number and read remainder of string
                pos = 0
                spacecount = 0
                while spacecount < 2:
                    if line[pos] == ' ':
                        spacecount += 1
                    pos += 1

                self.__shake.serial = line[pos:]
                pos = 0
                while pos < len(self.__shake.serial):
                    if ord(self.__shake.serial[pos]) == 0xA:
                        break
                    pos += 1

                self.__shake.serial = self.__shake.serial[:pos]
            elif i == SK6_EXPANSION_SLOT1:
                module = line
                for i in range(len(SK6_modules)):
                    if module[:len(SK6_modules[i])] == SK6_modules[i]:
                        self.__shake.modules[0] = i
                        break
            elif i == SK6_EXPANSION_SLOT2:
                module = line
                for i in range(len(SK6_modules)):
                    if module[:len(SK6_modules[i])] == SK6_modules[i]:
                        self.__shake.modules[1] = i
                        break
    
        self.read_data(1)
        self.synced = True
        return True
