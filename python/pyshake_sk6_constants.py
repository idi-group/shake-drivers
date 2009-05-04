# SK6 specific constants

SK6_DEFAULT_CHECKSUM 	= 0x00
SK6_MAX_PACKET_SIZE = 64
SK6_NUM_PACKET_TYPES = 39
SK6_HEADER_LEN = 4
SK6_RAW_HEADER_LEN = 3
SK6_PACKET_IID_LEN = 3

# 	Packet types enumeration
(SK6_DATA_ACC, 
SK6_DATA_GYRO, 
SK6_DATA_MAG, 
SK6_DATA_HEADING, 
SK6_DATA_CAP0, 
SK6_DATA_CAP1,
SK6_DATA_ANA0, 
SK6_DATA_ANA1, 

SK6_DATA_NVU, 
SK6_DATA_NVD, 
SK6_DATA_NVC, 
SK6_DATA_NVN,

SK6_DATA_CU0, 
SK6_DATA_CL0, 
SK6_DATA_CU1, 
SK6_DATA_CL1,

SK6_DATA_TIMESTAMP, 
SK6_DATA_PLAYBACK_COMPLETE,

SK6_DATA_RFID_TID,

SK6_DATA_SHAKING,
SK6_DATA_HEART_RATE,

SK6_CMD_READ, 
SK6_CMD_WRITE, 

SK6_ACK_ACK, 
SK6_ACK_NEG, 

SK6_STARTUP_INFO,

SK6_RAW_DATA_ACC, 
SK6_RAW_DATA_GYRO,
SK6_RAW_DATA_MAG, 
SK6_RAW_DATA_HEADING, 
SK6_RAW_DATA_CAP0,
SK6_RAW_DATA_CAP1, 
SK6_RAW_DATA_ANALOG0, 
SK6_RAW_DATA_ANALOG1,
SK6_RAW_DATA_EVENT, 
SK6_RAW_DATA_SHAKING, 
SK6_RAW_DATA_AUDIO_EXP,
SK6_RAW_DATA_AUDIO_HEADER,
SK6_RAW_DATA_AUDIO
) = range(SK6_NUM_PACKET_TYPES)

# variables

sk6_packet_headers = [ 	"$ACC", 
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

sk6_raw_packet_headers = [ 	126, 
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

sk6_packet_lengths = [ 
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

sk6_packet_has_checksum = [ 	1, 1, 1, 1, 1, 1, 1, 1,
							 	0, 0, 0, 0, 0, 0, 0, 0,
								0, 0,
								0,
								1,
								1,
								0, 0,
								1, 1,
								0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 ]

