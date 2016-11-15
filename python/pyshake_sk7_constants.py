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


# SK7 specific constants

SK7_DEFAULT_CHECKSUM = 0x00
SK7_MAX_PACKET_SIZE = 64
SK7_NUM_PACKET_TYPES = 69
SK7_HEADER_LEN = 4
SK7_RAW_HEADER_LEN = 3
SK7_PACKET_IID_LEN = 3

SK7_DATA_ASCII = 0x00
SK7_DATA_RAW = 0x02
SK7_DATA_RAW_SEQ = 0x06

SK7_NV_REG_RPH_CONFIG = 0x0046

#       Packet types enumeration
(
    SK7_DATA_ACC,
    SK7_DATA_GYRO,
    SK7_DATA_MAG,
    SK7_DATA_HEADING,
    SK7_DATA_CAP,
    SK7_DATA_CAP_B,
    SK7_DATA_CAP_C,
    SK7_DATA_ANA0,
    SK7_DATA_ANA1,
    SK7_DATA_RPH,

    SK7_DATA_NVU,
    SK7_DATA_NVD,
    SK7_DATA_NVC,
    SK7_DATA_NVN,

    SK7_DATA_CU0,
    SK7_DATA_CL0,
    SK7_DATA_CU1,
    SK7_DATA_CL1,
    SK7_DATA_CU2,
    SK7_DATA_CL2,
    SK7_DATA_CU3,
    SK7_DATA_CL3,
    SK7_DATA_CU4,
    SK7_DATA_CL4,
    SK7_DATA_CU5,
    SK7_DATA_CL5,
    SK7_DATA_CU6,
    SK7_DATA_CL6,
    SK7_DATA_CU7,
    SK7_DATA_CL7,
    SK7_DATA_CU8,
    SK7_DATA_CL8,
    SK7_DATA_CU9,
    SK7_DATA_CL9,
    SK7_DATA_CUA,
    SK7_DATA_CLA,
    SK7_DATA_CUB,
    SK7_DATA_CLB,

    SK7_DATA_RPH_QUATERNION,
    SK7_DATA_GYRO_TEMP,

    SK7_DATA_TIMESTAMP,
    SK7_DATA_PLAYBACK_COMPLETE,

    SK7_DATA_RFID_TID,

    SK7_DATA_SHAKING,
    SK7_DATA_HEART_RATE,

    SK7_CMD_READ,
    SK7_CMD_WRITE,

    SK7_ACK_ACK,
    SK7_ACK_NEG,

    SK7_STARTUP_INFO,

    SK7_RAW_DATA_ACC,
    SK7_RAW_DATA_GYRO,
    SK7_RAW_DATA_MAG,
    SK7_RAW_DATA_HEADING,
    SK7_RAW_DATA_CAP,
    SK7_RAW_DATA_CAP_B,
    SK7_RAW_DATA_CAP_C,
    SK7_RAW_DATA_ANALOG0,
    SK7_RAW_DATA_ANALOG1,
    SK7_RAW_DATA_EVENT,
    SK7_RAW_DATA_SHAKING,
    SK7_RAW_DATA_RPH,
    SK7_RAW_DATA_RPH_QUATERNION,
    SK7_RAW_DATA_GYRO_TEMP,

    SK7_RAW_DATA_IMU0,
    SK7_RAW_DATA_IMU1,
    SK7_RAW_DATA_IMU2,
    SK7_RAW_DATA_IMU3,
    SK7_RAW_DATA_IMU4,
) = range(SK7_NUM_PACKET_TYPES)

class SK7PacketInfo(object):

    def __init__(self, name, typeid, header, is_raw, len_bytes, has_checksum):
        self.name = name
        self.typeid = typeid
        self.is_raw = is_raw
        if is_raw:
            self.header = int(header)
        else:
            self.header = header
        self.len_bytes = len_bytes
        self.has_checksum = has_checksum

    def match_header(self, packet_header):
        if self.is_raw:
            return ord(packet_header) == self.header

        return packet_header == self.header

SK7_PACKETS = \
    [
        SK7PacketInfo("Accelerometer (ASCII)", SK7_DATA_ACC, "$ACC", False, 27, True),
        SK7PacketInfo("Gyro (ASCII)", SK7_DATA_GYRO, "$ARS", False, 27, True),
        SK7PacketInfo("Magnetometer (ASCII)", SK7_DATA_MAG, "$MAG", False, 27, True),
        SK7PacketInfo("Compass Heading (ASCII)", SK7_DATA_HEADING, "$HED", False, 14, True),
        SK7PacketInfo("Cap sensor A (ASCII)", SK7_DATA_CAP, "$CSA", False, 45, True),
        SK7PacketInfo("Cap sensor B (ASCII)", SK7_DATA_CAP_B, "$CSB", False, 45, True),
        SK7PacketInfo("Cap sensor C (ASCII)", SK7_DATA_CAP_C, "$CSC", False, 45, True),
        SK7PacketInfo("Analogue input 0 (ASCII)", SK7_DATA_ANA0, "$AI0", False, 14, True),
        SK7PacketInfo("Analogue input 1 (ASCII)", SK7_DATA_ANA1, "$AI1", False, 14, True),
        SK7PacketInfo("Roll pitch heading (ASCII)", SK7_DATA_RPH, "$RPH", False, 27, True),
        SK7PacketInfo("Nav switch up", SK7_DATA_NVU, "$NVU", False, 6, False),
        SK7PacketInfo("Nav switch down", SK7_DATA_NVD, "$NVD", False, 6, False),
        SK7PacketInfo("Nav switch centre", SK7_DATA_NVC, "$NVC", False, 6, False),
        SK7PacketInfo("Nav switch up", SK7_DATA_NVN, "$NVN", False, 6, False),
        SK7PacketInfo("Cap sensor 0 upper threshold", SK7_DATA_CU0, "$CU0", False, 6, False),
        SK7PacketInfo("Cap sensor 0 lower threshold", SK7_DATA_CL0, "$CL0", False, 6, False),
        SK7PacketInfo("Cap sensor 1 upper threshold", SK7_DATA_CU1, "$CU1", False, 6, False),
        SK7PacketInfo("Cap sensor 1 lower threshold", SK7_DATA_CL1, "$CL1", False, 6, False),
        SK7PacketInfo("Cap sensor 2 upper threshold", SK7_DATA_CU2, "$CU2", False, 6, False),
        SK7PacketInfo("Cap sensor 2 lower threshold", SK7_DATA_CL2, "$CL2", False, 6, False),
        SK7PacketInfo("Cap sensor 3 upper threshold", SK7_DATA_CU3, "$CU3", False, 6, False),
        SK7PacketInfo("Cap sensor 3 lower threshold", SK7_DATA_CL3, "$CL3", False, 6, False),
        SK7PacketInfo("Cap sensor 4 upper threshold", SK7_DATA_CU4, "$CU4", False, 6, False),
        SK7PacketInfo("Cap sensor 4 lower threshold", SK7_DATA_CL4, "$CL4", False, 6, False),
        SK7PacketInfo("Cap sensor 5 upper threshold", SK7_DATA_CU5, "$CU5", False, 6, False),
        SK7PacketInfo("Cap sensor 5 lower threshold", SK7_DATA_CL5, "$CL5", False, 6, False),
        SK7PacketInfo("Cap sensor 6 upper threshold", SK7_DATA_CU6, "$CU6", False, 6, False),
        SK7PacketInfo("Cap sensor 6 lower threshold", SK7_DATA_CL6, "$CL6", False, 6, False),
        SK7PacketInfo("Cap sensor 7 upper threshold", SK7_DATA_CU7, "$CU7", False, 6, False),
        SK7PacketInfo("Cap sensor 7 lower threshold", SK7_DATA_CL7, "$CL7", False, 6, False),
        SK7PacketInfo("Cap sensor 8 upper threshold", SK7_DATA_CU8, "$CU8", False, 6, False),
        SK7PacketInfo("Cap sensor 8 lower threshold", SK7_DATA_CL8, "$CL8", False, 6, False),
        SK7PacketInfo("Cap sensor 9 upper threshold", SK7_DATA_CU9, "$CU9", False, 6, False),
        SK7PacketInfo("Cap sensor 9 lower threshold", SK7_DATA_CL9, "$CL9", False, 6, False),
        SK7PacketInfo("Cap sensor 10 upper threshold", SK7_DATA_CUA, "$CUA", False, 6, False),
        SK7PacketInfo("Cap sensor 10 lower threshold", SK7_DATA_CLA, "$CLA", False, 6, False),
        SK7PacketInfo("Cap sensor 11 upper threshold", SK7_DATA_CUB, "$CUB", False, 6, False),
        SK7PacketInfo("Cap sensor 11 lower threshold", SK7_DATA_CLB, "$CLB", False, 6, False),
        SK7PacketInfo("Quaternion (ASCII)", SK7_DATA_RPH_QUATERNION, "$QTN", False, 45, True),
        SK7PacketInfo("Gyro temps", SK7_DATA_GYRO_TEMP, "$GOT", False, 27, True),
        SK7PacketInfo("Logging packet #1", SK7_DATA_TIMESTAMP, "$TIM", False, 16, False),
        SK7PacketInfo("Logging packet #2", SK7_DATA_PLAYBACK_COMPLETE, "Logg", False, 31, False),
        SK7PacketInfo("RFID tag", SK7_DATA_RFID_TID, "$TID", False, 24, False),
        SK7PacketInfo("Shaking event", SK7_DATA_SHAKING, "$SHK", False, 27, True),
        SK7PacketInfo("Heart rate", SK7_DATA_HEART_RATE, "$HR ", False, 14, True),
        SK7PacketInfo("Read command", SK7_CMD_READ, "$REA", False, 14, False),
        SK7PacketInfo("Write command", SK7_CMD_WRITE, "$WRI", False, 14, False),
        SK7PacketInfo("Ack", SK7_ACK_ACK, "$ACK", False, 14, True),
        SK7PacketInfo("Nack", SK7_ACK_NEG, "$NAK", False, 14, True),
        SK7PacketInfo("Device startup info", SK7_STARTUP_INFO, "\nSHA", False, -1, False),
        SK7PacketInfo("Accelerometer (raw)", SK7_RAW_DATA_ACC, "126", True, 10, False),
        SK7PacketInfo("Magnetometer (raw)", SK7_RAW_DATA_GYRO, "125", True, 10, False),
        SK7PacketInfo("Gyro (raw)", SK7_RAW_DATA_MAG, "124", True, 10, False),
        SK7PacketInfo("Compass heading (raw)", SK7_RAW_DATA_HEADING, "123", True, 6, False),
        SK7PacketInfo("Cap sensor A (raw)", SK7_RAW_DATA_CAP, "122", True, 15, False),
        SK7PacketInfo("Cap sensor B (raw)", SK7_RAW_DATA_CAP_B, "115", True, 15, False),
        SK7PacketInfo("Cap sensor C (raw)", SK7_RAW_DATA_CAP_C, "114", True, 15, False),
        SK7PacketInfo("Analogue input 0 (raw)", SK7_RAW_DATA_ANALOG0, "120", True, 6, False),
        SK7PacketInfo("Analogue input 1 (raw)", SK7_RAW_DATA_ANALOG1, "119", True, 6, False),
        SK7PacketInfo("Events", SK7_RAW_DATA_EVENT, "118", True, 5, False),
        SK7PacketInfo("Shaking events", SK7_RAW_DATA_SHAKING, "117", True, 9, False),
        SK7PacketInfo("Roll pitch heading (raw)", SK7_RAW_DATA_RPH, "116", True, 10, False),
        SK7PacketInfo("Roll pitch heading quaternions (raw)", SK7_RAW_DATA_RPH_QUATERNION, "110", True, 12, False),
        SK7PacketInfo("Gyro temps (raw)", SK7_RAW_DATA_GYRO_TEMP, "111", True, 12, False),
        SK7PacketInfo("IMU channel 0", SK7_RAW_DATA_IMU0, "80", True, 24, False),
        SK7PacketInfo("IMU channel 1", SK7_RAW_DATA_IMU1, "81", True, 24, False),
        SK7PacketInfo("IMU channel 2", SK7_RAW_DATA_IMU2, "82", True, 24, False),
        SK7PacketInfo("IMU channel 3", SK7_RAW_DATA_IMU3, "83", True, 24, False),
        SK7PacketInfo("IMU channel 4", SK7_RAW_DATA_IMU4, "84", True, 24, False),
    ]
