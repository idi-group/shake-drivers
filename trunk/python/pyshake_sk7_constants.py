# Copyright (c) 2006-2009, University of Glasgow
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without modification, are
# permitted provided that the following conditions are met:
#
#    * Redistributions of source code must retain the above copyright notice, this list of 
#		conditions and the following disclaimer.
#    * Redistributions in binary form must reproduce the above copyright notice, this list
#		of conditions and the following disclaimer in the documentation and/or other
#	 	materials provided with the distribution.
#    * Neither the name of the University of Glasgow nor the names of its contributors 
#		may be used to endorse or promote products derived from this software without
# 		specific prior written permission.
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

SK7_DEFAULT_CHECKSUM 	= 0x00
SK7_MAX_PACKET_SIZE = 64
SK7_NUM_PACKET_TYPES = 60
SK7_HEADER_LEN = 4
SK7_RAW_HEADER_LEN = 3
SK7_PACKET_IID_LEN = 3

# 	Packet types enumeration
(SK7_DATA_ACC, 
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
SK7_RAW_DATA_RPH
) = range(SK7_NUM_PACKET_TYPES)

# variables

sk7_packet_headers = [ 		"$ACC", 
							"$ARS", 
							"$MAG", 
							"$HED", 
							"$CSA",
							"$CSB",
							"$CSC",
							"$AI0", 
							"$AI1", 
							"$RPH",
							
							"$NVU", 
							"$NVD", 
							"$NVC", 
							"$NVN", 
							
							"$CU0",
							"$CL0", 
							"$CU1", 
							"$CL1", 
							"$CU2",
							"$CL2", 
							"$CU3", 
							"$CL3", 
							"$CU4",
							"$CL4", 
							"$CU5", 
							"$CL5", 
							"$CU6",
							"$CL6", 
							"$CU7", 
							"$CL7", 
							"$CU8",
							"$CL8", 
							"$CU9", 
							"$CL9", 
							"$CUA",
							"$CLA", 
							"$CUB", 
							"$CLB", 
							
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

sk7_raw_packet_headers = [ 		126, 
								125, 
								124, 
								123, 
								122, 
								115,
								114,
								120,
								119, 
								118, 
								117,
								116
							]

sk7_packet_lengths = [ 
		27, 
		27, 
		27, 
		14, 
		45, 
		45,
		45,
		14, 
		14,
		27,
						
		6, 
		6, 
		6, 
		6, 
						
		6, 
		6,
		6, 
		6,
		6, 
		6,
		6, 
		6,
		6, 
		6,
		6, 
		6,
		6, 
		6,
		6, 
		6,
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
		15, 
		15,
		15,
		6,
		6, 
		5, 
		9,
		10
	]

sk7_packet_has_checksum = [ 
	1,1,1,1,1,1,1,1,1,1,	# these are the basic sensor output channels + RPH

	0,0,0,0,				# nav switch events
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,	# cap threshold events
	0,0,					# logger packets
	0,						# RFID packet
	1,						# shaking event
	1,						# heart rate event
	0,0,					# commands
	1,1,					# acks
	0,						# startup info
	0,0,0,0,0,0,0,0,0,0,0,0	# raw data packets
]		

