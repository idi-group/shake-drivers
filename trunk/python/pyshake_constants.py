# 	This file contains most of the constant definitions used by the Python
# 	SHAKE code. The first set of constants are those that can be passed 
# 	to various driver functions, the second set are usually internally
# 	and can be ignored.

## Global success code for SHAKE functions.
SHAKE_SUCCESS = 1
## Global error code for SHAKE functions.
SHAKE_ERROR = -1

## Event produced when nav switch moved up
SHAKE_NAV_UP = 1

## Event produced when nav switch moved down
SHAKE_NAV_DOWN = 2

## Event produced when nav switch pressed
SHAKE_NAV_CENTRE = 3

## Event produced when nav switch released
SHAKE_NAV_NORMAL = 4

## Event produced when cap switch 0 upper threshold reached
SHAKE_CS0_UPPER = 5

## Event produced when cap switch 0 lower threshold reached
SHAKE_CS0_LOWER = 6

## Event produced when cap switch 1 upper threshold reached
SHAKE_CS1_UPPER = 7

## Event produced when cap switch 1 lower threshold reached
SHAKE_CS1_LOWER = 8

## Event produced when data logging playback is completed
SHAKE_PLAYBACK_COMPLETE = 9

## Event produced when a shaking event detected
SHAKE_SHAKING_EVENT = 10

## Event produced when a heart rate event is triggered
SHAKE_HEART_RATE_EVENT = 11

## Event produced when an RFID tag is detected
SHAKE_RFID_TID_EVENT = 12

# 	Parameters for the read/write_power_state functions
SHAKE_POWER_CAP 		= 0x01 		# capacitive sensor
SHAKE_POWER_ACC 		= 0x02 		# accelerometer
SHAKE_POWER_GYRO 		= 0x04 		# gyroscope
SHAKE_POWER_VIB 		= 0x08		# vibrotactile
SHAKE_POWER_MAG 		= 0x10 		# magnetometer
SHAKE_POWER_TEMP 		= 0x20 		# temperature
SHAKE_POWER_ANALOG 		= 0x40 	 	# analog
SHAKE_POWER_NAV 		= 0x80 		# nav button

# 	Parameters for the read/write_power_state_2 functions
SHAKE_POWER_CS0_INC 	= 0x01
SHAKE_POWER_CS0_DEC 	= 0x02
SHAKE_POWER_CS1_INC 	= 0x04
SHAKE_POWER_CS1_DEC 	= 0x08
SHAKE_POWER_CS_EVENTS 	= 0x10

# 	Parameters for the read/write_sample_rate functions.
# 	Corresponding sensors: 
# 		accelerometer, gyroscope, magnetometer,
# 		heading, capacitive sensor 0, capacitive sensor 1,
# 		analog input 0, analog input 1
(SHAKE_SENSOR_ACC, SHAKE_SENSOR_GYRO, SHAKE_SENSOR_MAG,
SHAKE_SENSOR_HEADING, SHAKE_SENSOR_CAP0, SHAKE_SENSOR_CAP1,
SHAKE_SENSOR_ANA0, SHAKE_SENSOR_ANA1) 	= range(8)

# 	Sample rate limits for each sensor (use with read/write_sample_rate functions)
SHAKE_ACC_MAX_RATE 		= 0xFF 		# accelerometer
SHAKE_GYRO_MAX_RATE 	= 0xFF 		# gyroscope
SHAKE_MAG_MAX_RATE 		= 0xFF 		# magnetometer
SHAKE_HEADING_MAX_RATE 	= 0xFF 		# heading
SHAKE_CAP0_MAX_RATE 	= 0xFF 		# capacitive sensor 0
SHAKE_CAP1_MAX_RATE 	= 0xFF 		# capacitive sensor 1 
SHAKE_ANA0_MAX_RATE 	= 0xFF 		# analog input 0
SHAKE_ANA1_MAX_RATE 	= 0xFF 		# analog input  1

# 	Parameters for the write_data_request function
SHAKE_REQ_ACC 			= 0x01 		# accelerometer
SHAKE_REQ_GYRO 			= 0x02 		# gyroscope
SHAKE_REQ_MAG 			= 0x04 		# magnetometer
SHAKE_REQ_HEADING 		= 0x08 		# heading
SHAKE_REQ_CAP0 			= 0x10 		# capacitive sensor 0 
SHAKE_REQ_CAP1 			= 0x20 		# capacitive sensor 1
SHAKE_REQ_ANA0 			= 0x40 		# analog input 0
SHAKE_REQ_ANA1 			= 0x80 		# analog input 1

# 	Possible values for the playvib function channel parameter
SHAKE_VIB_MAIN, SHAKE_VIB_LEFT, SHAKE_VIB_RIGHT 	= range(3)

#  	Min/Max value for the playvib function value parameter
SHAKE_VIB_PROFILE_MAX 	= 0xFF
SHAKE_VIB_SAMPLE_MAX_LENGTH = 32

SHAKE_BYPASS_ACC = 0x01
SHAKE_BYPASS_GYRO = 0x02
SHAKE_BYPASS_MAG = 0x04
SHAKE_BYPASS_CAP0 = 0x10
SHAKE_BYPASS_CAP1 = 0x20
SHAKE_BYPASS_ANA0 = 0x40
SHAKE_BYPASS_ANA1 = 0x80

SHAKE_CX_BYPASS_ACC = 0x01
SHAKE_CX_BYPASS_GYRO = 0x02
SHAKE_CX_BYPASS_MAG = 0x04

SHAKE_STREAM_ACC = 0x01
SHAKE_STREAM_GYRO = 0x02
SHAKE_STREAM_MAG = 0x04
SHAKE_STREAM_HEADING = 0x08
SHAKE_STREAM_CAP0 = 0x10
SHAKE_STREAM_CAP1 = 0x20
SHAKE_STREAM_ANA0 = 0x40
SHAKE_STREAM_ANA1 = 0x80

## The contents of the NV_REG_SHAKING_CONFIG register should be a combination of zero or more of these values
SHAKE_SHAKING_DETECTION = 0x01
SHAKE_SHAKING_DIRECTION_DETECTION = 0x02
SHAKE_SHAKING_VIBRATION = 0x04

SHAKE_HEART_RATE_MONITORING = 0x01
SHAKE_HEART_RATE_VISUAL_PULSE = 0x02
SHAKE_HEART_RATE_VIB_PULSE = 0x04


## When writing to the VO_REG_LOGGING_CTRL register, use one of these values as the parameter
SHAKE_LOGGING_PLAY	=	0x01
SHAKE_LOGGING_PAUSE	=	0x02
SHAKE_LOGGING_STOP	=	0x03
SHAKE_LOGGING_RECORD =	0x04
SHAKE_LOGGING_RESET	=	0x05

## The contents of the VO_REG_LOGGING_STATUS register should be a combination of these values 
SHAKE_LOGGING_RECORDING	=	0x01
SHAKE_LOGGING_PLAYING	=	0x02
SHAKE_LOGGING_MEMORYFULL =	0x04

## RFID TID tag length
SHAKE_RFID_TAG_LENGTH = 32

## The contents of the NV_REG_RFID_CONFIG register should be a combination of zero or more of these values
SHAKE_RFID_POWER_ON 	= 0x01
SHAKE_RFID_VISUAL_FEEDBACK = 0x02
SHAKE_RFID_AUTO_SCAN 	= 0x04


## The contents of the NV_REG_AUDIO_CONFIG register should be a combination of zero or more of these values
SHAKE_AUDIO_SAMPLING = 0x01
SHAKE_AUDIO_PLAYBACK = 0x02
SHAKE_AUDIO_EXTERNAL_MIC = 0x04
SHAKE_AUDIO_WAVE_PLAYBACK = 0x08
SHAKE_AUDIO_SAMPLE_PLAYBACK = 0x10

## Number of samples in a SHAKE audio data packet
SHAKE_AUDIO_DATA_LEN =  0x20

## SHAKE audio sampling rate in Hz
SHAKE_AUDIO_SAMPLE_RATE = 7200

## (decoded) SHAKE audio sample size in bits
SHAKE_AUDIO_SAMPLE_SIZE = 16

## Writing this value into VO_REG_MIDI_AMPLITUDE mutes the synthesiser
SHAKE_MIDI_AMPLITUDE_MUTE = 0x00

## Minimum and maximum values for the synthesiser volume
SHAKE_MIDI_AMPLITUDE_MIN = 0x01
SHAKE_MIDI_AMPLITUDE_MAX = 0xFF

## Minimum and maximum values for the VO_REG_MIDI_NOTE register
SHAKE_MIDI_NOTE_MIN = 0x21
SHAKE_MIDI_NOTE_MAX = 0x69

## If writing into the VO_REG_MIDI_WAVEFORM register, the value to write should be one of the following 
SHAKE_MIDI_WAVEFORM_SINE = 0x00
SHAKE_MIDI_WAVEFORM_SQUARE = 0x01
SHAKE_MIDI_WAVEFORM_TRIANGLE = 0x02
SHAKE_MIDI_WAVEFORM_SAWTOOTH = 0x03

## Audio sample upload page size (1 page = 1056 samples)
SHAKE_UPLOAD_PAGE_SIZE = 1056

## Audio sample minimum amplitude 
SHAKE_AUDIO_AMPLITUDE_MIN = 0

## Audio sample maximum amplitude
SHAKE_AUDIO_AMPLITUDE_MAX = 255

## Audio sample upload minimum address 
SHAKE_UPLOAD_MIN_PAGE = 0

## Audio sample upload maximum address 
SHAKE_UPLOAD_MAX_PAGE = 7999


# 	Max value returned by the read_battery_level function
SHAKE_BATTERY_MAX 		= 0xFF

# 	Return value from the read_power_status function is a bitwise OR 
# 	of these values.
SHAKE_EXT_POWER 		= 0x01 		# external power on
SHAKE_BAT_CHARGING 		= 0x02 		# battery charging
SHAKE_BAT_FULL 			= 0x04 		# battery fully charged

SHAKE_TEMP_MIN 			= 0x00
SHAKE_TEMP_MAX 			= 0x40

# 	register address listing (only required if you want to use the
# 	basic read/write() functions

## Non volatile register addresses
NV_REG_POWER1 = 0x0000
NV_REG_POWER2 = 0x0001
NV_REG_DATAFMT = 0x0002
NV_REG_ACC_CONFIG = 0x0004
NV_REG_CALIB_BYPASS = 0x0006
NV_REG_CX_CALIB_BYPASS = 0x0007
NV_REG_ACCOUT = 0x0008
NV_REG_GYROUT = 0x0009
NV_REG_MAGOUT = 0x000A
NV_REG_HEDOUT = 0x000B
NV_REG_CAP0OUT = 0x000C
NV_REG_CAP1OUT = 0x000D
NV_REG_ANA0OUT = 0x000E
NV_REG_ANA1OUT = 0x000F
NV_REG_TEMP_COMPENSATION = 0x0010
NV_REG_STREAM_DISABLE = 0x0012
NV_REG_DIGFIL_ACC = 0x0014
NV_REG_DIGFIL_GYR = 0x0015
NV_REG_DIGFIL_MAG = 0x0016
NV_REG_DIGFIL_HED = 0x0017
NV_REG_DIGFIL_CAP0 = 0x0018
NV_REG_DIGFIL_CAP1 = 0x0019
NV_REG_DIGFIL_ANA0 = 0x001A
NV_REG_DIGFIL_ANA1 = 0x001B
NV_REG_AUDIO_CONFIG = 0x001E
NV_REG_EXPANSION_CONFIG = 0x026
NV_REG_CS0_INC = 0x0028
NV_REG_CS0_DEC = 0x0029
NV_REG_CS0_INC_PROFILE = 0x002A
NV_REG_CS0_DEC_PROFILE = 0x002B
NV_REG_CS1_INC = 0x002C
NV_REG_CS1_DEC = 0x002D
NV_REG_CS1_INC_PROFILE = 0x002E
NV_REG_CS1_DEC_PROFILE = 0x002F
NV_REG_SHAKING_CONFIG = 0x0030
NV_REG_SHAKING_ACCEL_THRESHOLD = 0x0032
NV_REG_SHAKING_HOLDOFF_TIME = 0x0033
NV_REG_SHAKING_VIBRATION_PROFILE = 0x0034
NV_REG_SHAKING_HPF_CONSTANT = 0x0035
NV_REG_SHAKING_LPF_CONSTANT = 0x0036
NV_REG_LOGGING_PKT_LSB = 0x0038
NV_REG_LOGGING_PKT_MSB = 0x0039
NV_REG_HEART_RATE_CONFIG = 0x003A
NV_REG_RFID_CONFIG = 0x003C
NV_REG_RFID_FREQUENCY = 0x003D

## Volatile register addresses
VO_REG_PKTREQ = 0x0100
VO_REG_DATAREQ = 0x0101
VO_REG_VIB_MAIN = 0x0102
VO_REG_VIB_LEFT = 0x0103
VO_REG_VIB_RIGHT = 0x0104
VO_REG_BATTERY = 0x0105
VO_REG_PWRSTA = 0x0106
VO_REG_TEMPERATURE = 0x0108
VO_REG_CALIBRATION = 0x010A
VO_REG_LOGGING_CTRL = 0x010D
VO_REG_LOGGING_STATUS = 0x010E
VO_REG_LOGGING_MEM = 0x0110
VO_REG_MIDI_AMPLITUDE = 0x0111
VO_REG_MIDI_NOTE = 0x0112
VO_REG_MIDI_WAVEFORM = 0x0113
VO_REG_RFID_SCAN = 0x011A
VO_REG_EXP_PWM1	= 0x0120
VO_REG_EXP_PWM2	= 0x0121
VO_REG_EXP_PWM3	= 0x0122
VO_REG_EXP_PWM4	= 0x0123
VO_REG_EXP_PWM5	= 0x0124
VO_REG_EXP_PWM6	= 0x0125


VO_REG_VIB_LEFT_CONTINUOUS = 0x0126
VO_REG_VIB_RIGHT_CONTINUOUS = 0x0127


# 	Internal defs below here

# 	Length of a SHAKE packet header in bytes
SHAKE_HEADER_LEN 		= 4
SHAKE_RAW_HEADER_LEN 	= 3

SHAKE_NUM_PACKET_TYPES = 39

# 	Packet types enumeration
(SHAKE_DATA_ACC, 
SHAKE_DATA_GYRO, 
SHAKE_DATA_MAG, 
SHAKE_DATA_HEADING, 
SHAKE_DATA_CAP0, 
SHAKE_DATA_CAP1,
SHAKE_DATA_ANA0, 
SHAKE_DATA_ANA1, 

SHAKE_DATA_NVU, 
SHAKE_DATA_NVD, 
SHAKE_DATA_NVC, 
SHAKE_DATA_NVN,

SHAKE_DATA_CU0, 
SHAKE_DATA_CL0, 
SHAKE_DATA_CU1, 
SHAKE_DATA_CL1,

SHAKE_DATA_TIMESTAMP, 
SHAKE_DATA_PLAYBACK_COMPLETE,

SHAKE_DATA_RFID_TID,

SHAKE_DATA_SHAKING,
SHAKE_DATA_HEART_RATE,

SHAKE_CMD_READ, 
SHAKE_CMD_WRITE, 

SHAKE_ACK_ACK, 
SHAKE_ACK_NEG, 

SHAKE_STARTUP_INFO,

SHAKE_RAW_DATA_ACC, 
SHAKE_RAW_DATA_GYRO,
SHAKE_RAW_DATA_MAG, 
SHAKE_RAW_DATA_HEADING, 
SHAKE_RAW_DATA_CAP0,
SHAKE_RAW_DATA_CAP1, 
SHAKE_RAW_DATA_ANALOG0, 
SHAKE_RAW_DATA_ANALOG1,
SHAKE_RAW_DATA_EVENT, 
SHAKE_RAW_DATA_SHAKING, 
SHAKE_RAW_DATA_AUDIO_EXP,
SHAKE_RAW_DATA_AUDIO_HEADER,
SHAKE_RAW_DATA_AUDIO
) = range(SHAKE_NUM_PACKET_TYPES)



# 	Indicates an unknown/badly formatted packet
SHAKE_BAD_PKT 			= -1

CHECKSUM_LENGTH 		= 3

# variables

shake_packet_headers = [ 	"$ACC", 
							"$ARS", 
							"$MAG", 
							"$HED", 
							"$CS0",
							"$CS1", 
							"$AI0", 
							"$AI1", 
							
							"$NVU", 
							"$NVD", 
							"$NVC", 
							"$NVN", 
							
							"$CU0",
							"$CL0", 
							"$CU1", 
							"$CL1", 
							
							"$TIM", 
							"Logg", 
							
							"$TID",

							"$SHK",
							"$HR ",

							"$REA", 
							"$WRI",

							"$ACK", 
							"$NAK", 
							
							"\nSHA"
							]

shake_raw_packet_headers = [ 	126, 
								125, 
								124, 
								123, 
								122, 
								121, 
								120,
								119, 
								118, 
								117, 
								114,
								113, 
								112 
							]
#[ '\x7E', '\x7D', '\x7C', '\x7B', '\x7A', '\x79', '\x78', '\x77' ] 

shake_packet_lengths = [ 
		27, 
		27, 
		27, 
		14, 
		14, 
		14, 
		14, 
		14, 
						
		6, 
		6, 
		6, 
		6, 
						
		6, 
		6,
		6, 
		6,
						
		16, 
		31,

		24,

		27,
		14,
		
		14, 
		14, 
		
		14, 
		14,
		
		-1,
						
		10, 
		10, 
		10, 
		6,
		6, 
		6, 
		6,
		6, 
		5, 
		9,
		35,
		3, 
		35 
	]

shake_packet_has_checksum = [ 	1, 1, 1, 1, 1, 1, 1, 1,
							 	0, 0, 0, 0, 0, 0, 0, 0,
								0, 0,
								0,
								1,
								1,
								0, 0,
								1, 1,
								0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 ]

