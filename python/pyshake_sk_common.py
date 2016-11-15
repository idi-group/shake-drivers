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


from pyshake_constants import *
from pyshake_sk7_constants import *

def convert_raw_data_value(bytes):
    lsb = ord(bytes[0])
    msb = ord(bytes[1])

    if msb & 0x80:
        return (lsb + (msb << 8)) - 65536
    else:
        return lsb + (msb << 8)

class sk_imu_data:
    def __init__(self):
        self.acc = [0, 0, 0]
        self.gyro = [0, 0, 0]
        self.mag = [0, 0, 0]
        self.temp = 0
        self.seq = 0

    def raw(self):
        return self.acc + self.gyro + self.mag + [self.temp, self.seq]

    def __repr__(self):
        return 'Acc: {}, Gyro: {}, Mag: {}, Temp: {}, Seq: {}'.format(self.acc, self.gyro, self.mag, self.temp, self.seq)

class sk_sensor_data:
    def __init__(self):
        self.accx, self.accy, self.accz = 0, 0, 0
        self.gyrx, self.gyry, self.gyrz = 0, 0, 0
        self.magx, self.magy, self.magz = 0, 0, 0
        self.heading = 0
        self.cap_sk6 = [0 for x in range(2)]
        self.cap_sk7 = [[0 for x in range(12)], [0 for x in range(12)], [0 for x in range(12)]]
        self.ana0, self.ana1 = 0, 0
        self.shaking_peak_accel, self.shaking_direction, self.shaking_timestamp = 0, 0, 0
        self.hr_bpm = 0.0
        self.rph = [0, 0, 0]
        self.rphq = [0, 0, 0, 0]
        self.temps = [0, 0, 0]
        self.timestamps = [0 for x in range(8)]
        self.internal_timestamps = [0 for x in range(1 + (SK7_RAW_DATA_IMU4 - SK7_RAW_DATA_ACC))]
        self.sk6seq = 0
        self.sk7seq = 0
        self.hrseq = 0
        self.imudata = [sk_imu_data() for x in range(5)]
        self.gyro_temps = [0 for x in range(4)]

class SHAKE:
    def __init__(self, shakedev, devtype):
        self.__shake = shakedev
        self.device_type = devtype
        self.data = sk_sensor_data()
        self.peek_flag = False
        self.peek = 0
        self.synced = False

    def parse_ascii_packet(self, packet_type, packetbuf, playback, timestamp_packet):
        pass

    def read_ascii_packet(self, packet_type, packetbuf):
        pass

    def parse_raw_packet(self, packet_type, packetbuf, has_seq):
        pass

    def read_raw_packet(self, packet_type, packetbuf):
        pass

    def get_next_packet(self):
        pass

    def parse_packet(self, packetbuf, packet_type):
        pass

    def is_ascii_packet(self, packet_type):
        pass

    def parse_ack_packet(self, packetbuf, addr, val):
        pass

    def extract_ascii_packet(self, packet_type, packetbuf, playback, timestamp_packet):
        pass

    def extract_raw_packet(self, packet_type, packetbuf, has_seq):
        pass

    def classify_packet_header(self, packetbuf, ascii_packet):
        pass

    def read_device_info(self):
        pass

    def read_data(self, num_bytes):
        if self.__shake.port is None:
            return ""

        bytes = ""
        if self.peek_flag:
            bytes += chr(self.peek)
            num_bytes -= 1
            self.peek_flag = False

            if num_bytes == 0:
                return bytes

        allbytes = bytes + self.__shake.port.read(num_bytes)
        return allbytes

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
                    if (ord(buf[i]) == 0xA and ord(buf[i + 1]) == 0xD) or (ord(buf[i]) == 0xD and ord(buf[i + 1]) == 0xA):
                        return buf

            pos += 1

        return None
