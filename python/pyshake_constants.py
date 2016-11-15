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


#       This file contains most of the constant definitions used by the Python
#       SHAKE code. The first set of constants are those that can be passed 
#       to various driver functions, the second set are usually internally
#       and can be ignored.

# Global success code for SHAKE functions.
SHAKE_SUCCESS = 1
# Global error code for SHAKE functions.
SHAKE_ERROR = -1

SHAKE_BAD_PACKET = -1
SHAKE_CHECKSUM_LENGTH = 3

# Event produced when nav switch moved up
SHAKE_NAV_UP = 1

# Event produced when nav switch moved down
SHAKE_NAV_DOWN = 2

# Event produced when nav switch pressed
SHAKE_NAV_CENTRE = 3

# Event produced when nav switch released
SHAKE_NAV_NORMAL = 4

# Event produced when cap switch 0 upper threshold reached
SK6_CS0_UPPER = 5

# Event produced when cap switch 0 lower threshold reached
SK6_CS0_LOWER = 6

# Event produced when cap switch 1 upper threshold reached
SK6_CS1_UPPER = 7

# Event produced when cap switch 1 lower threshold reached
SK6_CS1_LOWER = 8

# Event produced when data logging playback is completed
SHAKE_PLAYBACK_COMPLETE = 9

# Event produced when a shaking event detected
SHAKE_SHAKING_EVENT = 10

# Event produced when a heart rate event is triggered
SHAKE_HEART_RATE_EVENT = 11

# Event produced when an RFID tag is detected
SHAKE_RFID_TID_EVENT = 12

SK7_CS0_UPPER = 13
SK7_CS0_LOWER = 14
SK7_CS1_UPPER = 15
SK7_CS1_LOWER = 16
SK7_CS2_UPPER = 17
SK7_CS2_LOWER = 18
SK7_CS3_UPPER = 19
SK7_CS3_LOWER = 20
SK7_CS4_UPPER = 21
SK7_CS4_LOWER = 22
SK7_CS5_UPPER = 23
SK7_CS5_LOWER = 24
SK7_CS6_UPPER = 25
SK7_CS6_LOWER = 26
SK7_CS7_UPPER = 27
SK7_CS7_LOWER = 28
SK7_CS8_UPPER = 29
SK7_CS8_LOWER = 30
SK7_CS9_UPPER = 31
SK7_CS9_LOWER = 32
SK7_CSA_UPPER = 33
SK7_CSA_LOWER = 34
SK7_CSB_UPPER = 35
SK7_CSB_LOWER = 36

# Device types
SHAKE_SK6 = 0
SHAKE_SK7 = 1

# SK6 option modules
# slot 1
SK6_MODULE_NONE = 0
SK6_MODULE_GYRO1 = 1
SK6_MODULE_GYRO2 = 2
SK6_MODULE_RFID = 3
# slot 2
SK6_MODULE_AUDIO = 4
SK6_MODULE_EXTVIB = 5
SK6_MODULE_USB = 6
SK6_MODULE_EXPANSION = 7
SK6_MODULE_LAST = 8

# SK7 option modules 
# TODO
SK7_MODULE_NONE = 9
SK7_MODULE_LAST = 10


# Values passed to write_power_state function should be an OR of these values
SHAKE_POWER_CAP         = 0x01      # capacitive sensor (bit 0)
SHAKE_POWER_ACC         = 0x02      # accelerometer
SHAKE_POWER_GYRO        = 0x04      # gyroscope
SHAKE_POWER_VIB         = 0x08      # vibrotactile
SHAKE_POWER_MAG         = 0x10      # magnetometer
SHAKE_POWER_TEMP        = 0x20      # temperature
SHAKE_POWER_ANALOG          = 0x40      # analog
SHAKE_POWER_NAV         = 0x80      # nav button (bit 7)

#       Parameters for the read/write_sample_rate functions.
#       Corresponding sensors: 
#           accelerometer, gyroscope, magnetometer,
#           heading, capacitive sensor 0 (SK6), capacitive sensor 1 (SK6),
#           analog input 0, analog input 1, capacitive sensors (SK7)
SHAKE_SENSOR_ACC    = 0
SHAKE_SENSOR_GYRO       = 1 
SHAKE_SENSOR_MAG    = 2
SHAKE_SENSOR_HEADING    = 3
SHAKE_SENSOR_CAP0       = 4
SHAKE_SENSOR_CAP1       = 5
SHAKE_SENSOR_ANA0       = 6 
SHAKE_SENSOR_ANA1       = 7
SHAKE_SENSOR_CAP    = 4

SHAKE_SENSOR_IMU0 = 10
SHAKE_SENSOR_IMU1 = 11
SHAKE_SENSOR_IMU2 = 12
SHAKE_SENSOR_IMU3 = 13
SHAKE_SENSOR_IMU4 = 14

SHAKE_REQ_ACC       = 1
SHAKE_REQ_GYRO      = 2
SHAKE_REQ_MAG       = 4
SHAKE_REQ_HEADING   = 8
SHAKE_REQ_CAP       = 16
SHAKE_REQ_ANA0      = 64
SHAKE_REQ_ANA1      = 128

#       Maximum sensor output rate (Hz)
SHAKE_MAX_OUTPUT_RATE =     0xFF

#       Possible values for the playvib function channel parameter
SHAKE_VIB_MAIN, SHAKE_VIB_LEFT, SHAKE_VIB_RIGHT, SHAKE_VIB_FORCEREACTOR, SHAKE_VIB_EXT_ACTUATOR     = range(5)

#       Maximum number of speed/time pairs in a vibration profile
SHAKE_VIB_SAMPLE_MAX_LENGTH = 32

#       Max values for vibration profile speeds/times
SHAKE_VIB_SPEED_MAX = 0xFF
SHAKE_VIB_TIME_MAX = 0xFF

#       Min/Max value for the playvib function value parameter
SHAKE_VIB_PROFILE_MIN   = 0x01
SHAKE_VIB_PROFILE_MAX   = 0xFF

#       Return value from the read_power_status function is a bitwise OR 
#       of these values.
SHAKE_EXT_POWER         = 0x01      # external power on
SHAKE_BAT_CHARGING          = 0x02      # battery charging
SHAKE_BAT_FULL          = 0x04      # battery fully charged

# The contents of the SHAKE_NV_REG_RFID_CONFIG register should be a combination of zero or more of these values
SHAKE_RFID_POWER_ON     = 0x01
SHAKE_RFID_VISUAL_FEEDBACK = 0x02
SHAKE_RFID_AUTO_SCAN    = 0x04

# If writing into the SHAKE_VO_REG_MIDI_WAVEFORM register, the value to write should be one of the following 
SHAKE_MIDI_WAVEFORM_SINE = 0x00
SHAKE_MIDI_WAVEFORM_SQUARE = 0x01
SHAKE_MIDI_WAVEFORM_TRIANGLE = 0x02
SHAKE_MIDI_WAVEFORM_SAWTOOTH = 0x03

# When writing to the SHAKE_VO_REG_LOGGING_CTRL register, use one of these values as the parameter
SHAKE_LOGGING_PLAY      =       0x01
SHAKE_LOGGING_PAUSE     =       0x02
SHAKE_LOGGING_STOP      =       0x03
SHAKE_LOGGING_RECORD =  0x04
SHAKE_LOGGING_RESET     =       0x05

# The contents of the SHAKE_VO_REG_LOGGING_STATUS register should be a combination of these values 
SHAKE_LOGGING_RECORDING =       0x01
SHAKE_LOGGING_PLAYING   =       0x02
SHAKE_LOGGING_MEMORYFULL =      0x04

# RFID TID tag length
SHAKE_RFID_TAG_LENGTH = 32

#       Max value returned by the read_battery_level function
SHAKE_BATTERY_MAX           = 0xFF

SHAKE_TEMP_MIN          = 0x00
SHAKE_TEMP_MAX          = 0x40

# Number of samples in a SHAKE audio data packet
SHAKE_AUDIO_DATA_LEN =  0x20

# SHAKE audio sampling rate in Hz
SHAKE_AUDIO_SAMPLE_RATE = 7200

# (decoded) SHAKE audio sample size in bits
SHAKE_AUDIO_SAMPLE_SIZE = 16

# Writing this value into SHAKE_VO_REG_MIDI_AMPLITUDE mutes the synthesiser
SHAKE_MIDI_AMPLITUDE_MUTE = 0x00

# Minimum and maximum values for the synthesiser volume
SHAKE_MIDI_AMPLITUDE_MIN = 0x01
SHAKE_MIDI_AMPLITUDE_MAX = 0xFF

# Minimum and maximum values for the SHAKE_VO_REG_MIDI_NOTE register
SHAKE_MIDI_NOTE_MIN = 0x21
SHAKE_MIDI_NOTE_MAX = 0x69

# Audio sample upload page size (1 page = 1056 samples)
SHAKE_UPLOAD_PAGE_SIZE = 1056

# Audio sample minimum amplitude 
SHAKE_AUDIO_AMPLITUDE_MIN = 0

# Audio sample maximum amplitude
SHAKE_AUDIO_AMPLITUDE_MAX = 255

# Audio sample upload minimum address 
SHAKE_UPLOAD_MIN_PAGE = 0

# Audio sample upload maximum address 
SHAKE_UPLOAD_MAX_PAGE = 7999

# Vibration sample minimum amplitude for ext module 
SHAKE_VIB_AMPLITUDE_MIN = 0

# Vibration sample maximum amplitude for ext module 
SHAKE_VIB_AMPLITUDE_MAX = 255

SHAKE_UPLOAD_CHUNK_SIZE = 64
SHAKE_UPLOAD_DELAY      = 30

#       register address listing (only required if you want to use the
#       basic read/write() functions

# Non volatile register addresses
SHAKE_NV_REG_POWER1 = 0x0000
SHAKE_NV_REG_POWER2 = 0x0001
SHAKE_NV_REG_DATAFMT = 0x0002
SHAKE_NV_REG_ACC_CONFIG = 0x0004
SHAKE_NV_REG_CALIB_BYPASS = 0x0006
SHAKE_NV_REG_CX_CALIB_BYPASS = 0x0007
SHAKE_NV_REG_ACCOUT = 0x0008
SHAKE_NV_REG_GYROUT = 0x0009
SHAKE_NV_REG_MAGOUT = 0x000A
SHAKE_NV_REG_HEDOUT = 0x000B
SHAKE_NV_REG_CAP0OUT = 0x000C
SHAKE_NV_REG_CAP1OUT = 0x000D
SHAKE_NV_REG_ANA0OUT = 0x000E
SHAKE_NV_REG_ANA1OUT = 0x000F
SHAKE_NV_REG_TEMP_COMPENSATION = 0x0010
SHAKE_NV_REG_STREAM_DISABLE = 0x0012
SHAKE_NV_REG_DIGFIL_ACC = 0x0014
SHAKE_NV_REG_DIGFIL_GYR = 0x0015
SHAKE_NV_REG_DIGFIL_MAG = 0x0016
SHAKE_NV_REG_DIGFIL_HED = 0x0017
SHAKE_NV_REG_DIGFIL_CAP0 = 0x0018
SHAKE_NV_REG_DIGFIL_CAP1 = 0x0019
SHAKE_NV_REG_DIGFIL_ANA0 = 0x001A
SHAKE_NV_REG_DIGFIL_ANA1 = 0x001B
SHAKE_NV_REG_AUDIO_CONFIG = 0x001E
SHAKE_NV_REG_EXPANSION_CONFIG = 0x026

SK6_NV_REG_CS0_INC = 0x0028
SK6_NV_REG_CS0_DEC = 0x0029
SK6_NV_REG_CS0_INC_PROFILE = 0x002A
SK6_NV_REG_CS0_DEC_PROFILE = 0x002B
SK6_NV_REG_CS1_INC = 0x002C
SK6_NV_REG_CS1_DEC = 0x002D
SK6_NV_REG_CS1_INC_PROFILE = 0x002E
SK6_NV_REG_CS1_DEC_PROFILE = 0x002F

SK7_NV_REG_CAP_INC = 0x0028
SK7_NV_REG_CAP_DEC = 0x0029
SK7_NV_REG_CAP_INC_PROFILE = 0x002A
SK7_NV_REG_CAP_DEC_PROFILE = 0x002B

SHAKE_NV_REG_SHAKING_CONFIG = 0x0030
SHAKE_NV_REG_SHAKING_ACCEL_THRESHOLD = 0x0032
SHAKE_NV_REG_SHAKING_HOLDOFF_TIME = 0x0033
SHAKE_NV_REG_SHAKING_VIBRATION_PROFILE = 0x0034
SHAKE_NV_REG_SHAKING_HPF_CONSTANT = 0x0035
SHAKE_NV_REG_SHAKING_LPF_CONSTANT = 0x0036
SHAKE_NV_REG_LOGGING_PKT_LSB = 0x0038
SHAKE_NV_REG_LOGGING_PKT_MSB = 0x0039
SHAKE_NV_REG_HEART_RATE_CONFIG = 0x003A
SHAKE_NV_REG_RFID_CONFIG = 0x003C
SHAKE_NV_REG_RFID_FREQUENCY = 0x003D

# Volatile register addresses
SHAKE_VO_REG_PKTREQ = 0x0100
SHAKE_VO_REG_DATAREQ = 0x0101
SHAKE_VO_REG_VIB_MAIN = 0x0102
SHAKE_VO_REG_VIB_LEFT = 0x0103
SHAKE_VO_REG_VIB_RIGHT = 0x0104
SHAKE_VO_REG_BATTERY = 0x0105
SHAKE_VO_REG_PWRSTA = 0x0106
SHAKE_VO_REG_TEMPERATURE = 0x0108
SHAKE_VO_REG_CALIBRATION = 0x010A
SHAKE_VO_REG_LOGGING_CTRL = 0x010D
SHAKE_VO_REG_LOGGING_STATUS = 0x010E
SHAKE_VO_REG_LOGGING_MEM = 0x0110
SHAKE_VO_REG_MIDI_AMPLITUDE = 0x0111
SHAKE_VO_REG_MIDI_NOTE = 0x0112
SHAKE_VO_REG_MIDI_WAVEFORM = 0x0113
SHAKE_VO_REG_RFID_SCAN = 0x011A
SHAKE_VO_REG_EXP_PWM1   = 0x0120
SHAKE_VO_REG_EXP_PWM2   = 0x0121
SHAKE_VO_REG_EXP_PWM3   = 0x0122
SHAKE_VO_REG_EXP_PWM4   = 0x0123
SHAKE_VO_REG_EXP_PWM5   = 0x0124
SHAKE_VO_REG_EXP_PWM6   = 0x0125

SHAKE_VO_REG_VIB_LEFT_CONTINUOUS = 0x0126
SHAKE_VO_REG_VIB_RIGHT_CONTINUOUS = 0x0127

from pyshake_sk6_constants import *
from pyshake_sk7_constants import *
