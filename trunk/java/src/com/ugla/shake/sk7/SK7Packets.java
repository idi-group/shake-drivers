package com.ugla.shake.sk7;

public class SK7Packets {
	public static final int SK7_DEFAULT_CHECKSUM = 0x00;

	public static final int SK7_MAX_PACKET_SIZE = 64;
	public static final int SK7_NUM_PACKET_TYPES =	61;

	public static final int SK7_HEADER_LEN	= 4;	// "$pid," == 4 chars
	public static final int SK7_RAW_HEADER_LEN = 3;
	public static final int SK7_PACKET_ID_LEN	= 3;


	/* packet types */
	/* ASCII packets */
	public static final int SK7_DATA_ACC = 0;					// accelerometer
	public static final int SK7_DATA_GYRO = 1;				// angular rate (ie gyro readings)
	public static final int SK7_DATA_MAG = 2; 					// magnetometer
	public static final int SK7_DATA_HEADING = 3;					// compass heading
	public static final int SK7_DATA_CAP = 4;					// capacitive sensors
	public static final int SK7_DATA_CAP_B = 5;	
	public static final int SK7_DATA_CAP_C = 6;
	public static final int SK7_DATA_ANA0 = 7;					// analog input 0
	public static final int SK7_DATA_ANA1 = 8;					// analog input 1
	public static final int SK7_DATA_RPH = 9;					// roll-pitch-heading

	public static final int SK7_DATA_NVU = 10;						// nav switch up
	public static final int SK7_DATA_NVD = 11;						// nav switch down
	public static final int SK7_DATA_NVC = 12;						// nav switch centre
	public static final int SK7_DATA_NVN = 13;						// nav switch release

	public static final int SK7_DATA_CU0 = 14;						// cap switch 0 upper threshold trigger
	public static final int SK7_DATA_CL0 = 15;						// cap switch 0 lower threshold trigger
	public static final int SK7_DATA_CU1 = 16;						// cap switch 1 upper threshold trigger
	public static final int SK7_DATA_CL1 = 17;						// cap switch 1 lower threshold trigger
	public static final int SK7_DATA_CU2 = 18;						// cap switch 2 upper threshold trigger
	public static final int SK7_DATA_CL2 = 19;						// cap switch 2 lower threshold trigger
	public static final int SK7_DATA_CU3 = 20;						// cap switch 3 upper threshold trigger
	public static final int SK7_DATA_CL3 = 21;						// cap switch 3 lower threshold trigger
	public static final int SK7_DATA_CU4 = 22;						// cap switch 3 upper threshold trigger
	public static final int SK7_DATA_CL4 = 23;						// cap switch 4 lower threshold trigger
	public static final int SK7_DATA_CU5 = 24;						// cap switch 5 upper threshold trigger
	public static final int SK7_DATA_CL5 = 25;						// cap switch 5 lower threshold trigger
	public static final int SK7_DATA_CU6 = 26;						// cap switch 6 upper threshold trigger
	public static final int SK7_DATA_CL6 = 27;						// cap switch 6 lower threshold trigger
	public static final int SK7_DATA_CU7 = 28;						// cap switch 7 upper threshold trigger
	public static final int SK7_DATA_CL7 = 29;						// cap switch 7 lower threshold trigger
	public static final int SK7_DATA_CU8 = 30;						// cap switch 8 upper threshold trigger
	public static final int SK7_DATA_CL8 = 31;						// cap switch 8 lower threshold trigger
	public static final int SK7_DATA_CU9 = 32;						// cap switch 9 upper threshold trigger
	public static final int SK7_DATA_CL9 = 33;						// cap switch 9 lower threshold trigger
	public static final int SK7_DATA_CUA = 34;						// cap switch 10 upper threshold trigger
	public static final int SK7_DATA_CLA = 35;						// cap switch 10 lower threshold trigger
	public static final int SK7_DATA_CUB = 36;						// cap switch 11 upper threshold trigger
	public static final int SK7_DATA_CLB = 37;						// cap switch 11 lower threshold trigger

	public static final int SK7_DATA_TIMESTAMP = 38;				// timestamped logged data 
	public static final int SK7_DATA_PLAYBACK_COMPLETE = 39;		// playback completed (special packet format)

	public static final int SK7_DATA_RFID_TID = 40;				// RFID TID packet

	public static final int SK7_DATA_SHAKING = 41;					// events from shaking detection algorithm
	public static final int SK7_DATA_HEART_RATE = 42;				// events from heart rate monitor
	
	/* command packets */
	public static final int SK7_CMD_READ = 43; 					// read command
	public static final int SK7_CMD_WRITE = 44; 					// write command
	
	/* acknowledgement packets */
	public static final int SK7_ACK_ACK = 45;						// successful ACK
	public static final int SK7_ACK_NEG = 46; 						// negative ACK (ie an error occurred)

	public static final int SK7_STARTUP_INFO = 47;

	/* Raw packets */
	public static final int SK7_RAW_DATA_ACC = 48;
	public static final int SK7_RAW_DATA_GYRO = 49;
	public static final int SK7_RAW_DATA_MAG = 50;
	public static final int SK7_RAW_DATA_HEADING = 51;
	public static final int SK7_RAW_DATA_CAP = 52;
	public static final int SK7_RAW_DATA_CAP_B = 53;
	public static final int SK7_RAW_DATA_CAP_C = 54;
	public static final int SK7_RAW_DATA_ANALOG0 = 55;
	public static final int SK7_RAW_DATA_ANALOG1 = 56;
	public static final int SK7_RAW_DATA_EVENT = 57;
	public static final int SK7_RAW_DATA_SHAKING = 58;
	public static final int SK7_RAW_DATA_RPH = 59;
	public static final int SK7_RAW_DATA_RPH_QUATERNION = 60;
	
	// length of checksum part of packet in bytes
	public static final int CHECKSUM_LENGTH = 3;

	// TODO UPDATE (only for debugging anyway)
	public static String sk7_packet_type_names[] = {
		"ASCII/Acc",
		"ASCII/Gyro",
		"ASCII/Mag",
		"ASCII/Heading",
		"ASCII/Cap0",
		"ASCII/Cap1",
		"ASCII/Ana0",
		"ASCII/Ana1",
		"ASCII/RPH",
		"ASCII/NavUp",
		"ASCII/NavDown",
		"ASCII/NavCentre",
		"ASCII/NavRelease",
		"ASCII/Cap0Upper",
		"ASCII/Cap0Lower",
		"ASCII/Cap1Upper",
		"ASCII/Cap1Lower",
		"ASCII/Timestamp",
		"ASCII/PlaybackComplete",
		"ASCII/RFID",
		"ASCII/Shaking",
		"ASCII/HeartRate",
		"ASCII/CmdRead",
		"ASCII/CmdWrite",
		"ASCII/ACK",
		"ASCII/NAK",
		"ASCII/StartupInfo",
		"Binary/Acc",
		"Binary/Gyro",
		"Binary/Mag",
		"Binary/Heading",
		"Binary/Cap0",
		"Binary/Cap1",
		"Binary/Analog0",
		"Binary/Analog1",
		"Binary/Event",
		"Binary/Shaking",
		"Binary/RPH",
		"Binary/AudioSTANE",
		"Binary/AudioHeader",
		"Binary/Audio",
	};

	/* packet headers (these directly follow the common header above) */
	public static String sk7_packet_headers[] = {
		"$ACC",		// data; accelerometer
		"$ARS",		// data; angular rate (gyro)
		"$MAG", 	// data; magnetometer
		"$HED", 	// data; heading
		"$CSA",		// data; capacitive sensors
		"$CSB",		// data; external capacitive sensor
		"$CSC",		// data; external capacitive sensor
		"$AI0",		// data; analog input 0
		"$AI1",		// data; analog input 1
		"$RPH",		// data; roll-pitch-heading

		"$NVU",		// data; nav switch up
		"$NVD",		// data; nav switch down
		"$NVC",		// data; nav switch centre
		"$NVN",		// data; nav switch released

		"$CU0",		// data; cap switch 0; increasing threshold trigger
		"$CL0",		// data; cap switch 0; decreasing threshold trigger
		"$CU1",		// data; cap switch 1; increasing threshold trigger
		"$CL1",		// data; cap switch 1; decreasing threshold trigger
		"$CU2",		// data; cap switch 2; increasing threshold trigger
		"$CL2",		// data; cap switch 2; decreasing threshold trigger
		"$CU3",		// data; cap switch 3; increasing threshold trigger
		"$CL3",		// data; cap switch 3; decreasing threshold trigger
		"$CU4",		// data; cap switch 4; increasing threshold trigger
		"$CL4",		// data; cap switch 4; decreasing threshold trigger
		"$CU5",		// data; cap switch 5; increasing threshold trigger
		"$CL5",		// data; cap switch 5; decreasing threshold trigger
		"$CU6",		// data; cap switch 6; increasing threshold trigger
		"$CL6",		// data; cap switch 6; decreasing threshold trigger
		"$CU7",		// data; cap switch 7; increasing threshold trigger
		"$CL7",		// data; cap switch 7; increasing threshold trigger
		"$CU8",		// data; cap switch 8; decreasing threshold trigger
		"$CL8",		// data; cap switch 8; increasing threshold trigger
		"$CU9",		// data; cap switch 9; decreasing threshold trigger
		"$CL9",		// data; cap switch 9; increasing threshold trigger
		"$CUA",		// data; cap switch 10; decreasing threshold trigger
		"$CLA",		// data; cap switch 10; increasing threshold trigger
		"$CUB",		// data; cap switch 11; decreasing threshold trigger
		"$CLB",		// data; cap switch 11; decreasing threshold trigger

		"$TIM",		// data; timestamped logged data packet
		"Logg",		// data; playback complete packet ("Logged Data Upload Complete.\r\n")

		"$TID",		// data; RFID TID packet

		"$SHK",		// data; shaking event detected
		"$HR ",		// data; heart rate monitor event

		"$REA",		// command; read
		"$WRI",		// command; write

		"$ACK",		// ack; positive
		"$NAK",		// ack; negative

		"\nSHA",		// SK7 startup info
	};

	public static char sk7_raw_packet_headers[] = { 
		126,	// raw acc
		125,	// raw gyro
		124,	// raw mag
		123,	// raw compass
		122,	// raw cap
		115,	// raw cap (ext)
		114,	// raw cap (ext)
		120,	// raw ana0
		119,	// raw ana1
		118,	// nav events
		117,	// shaking events
		116,	// roll-pitch-heading
		110, 	// roll-pitch-heading (quaternion)
	};

	/* packet lengths (bytes), WITHOUT INCLUDING CHECKSUMS */
	public static int sk7_packet_lengths[] = {
		27, 		// data; accelerometer
		27,			// data; angular rate (gyro)
		27, 		// data; magnetometer
		14, 		// data; heading
		45, 		// data; capacitive sensors
		45, 		// data; capacitive sensors (ext)
		45, 		// data; capacitive sensors (ext)
		14,			// data: analog input 0
		14,			// data; analog input 1
		27,			// data; roll-pitch-heading

		6,			// data; nav switch up
		6,			// data; nav switch down
		6,			// data; nav switch centre
		6,			// data; nav switch released

		6,			// data; cap switch 0; increasing threshold trigger
		6,			// data; cap switch 0; decreasing threshold trigger
		6,			// data; cap switch 1; increasing threshold trigger
		6,			// data; cap switch 1; decreasing threshold trigger
		6,			// data; cap switch 2; increasing threshold trigger
		6,			// data; cap switch 2; decreasing threshold trigger
		6,			// data; cap switch 3; increasing threshold trigger
		6,			// data; cap switch 3; increasing threshold trigger
		6,			// data; cap switch 4; decreasing threshold trigger
		6,			// data; cap switch 4; increasing threshold trigger
		6,			// data; cap switch 5; increasing threshold trigger
		6,			// data; cap switch 5; decreasing threshold trigger
		6,			// data; cap switch 6; increasing threshold trigger
		6,			// data; cap switch 6; decreasing threshold trigger
		6,			// data; cap switch 7; increasing threshold trigger
		6,			// data; cap switch 7; decreasing threshold trigger
		6,			// data; cap switch 8; increasing threshold trigger
		6,			// data; cap switch 8; increasing threshold trigger
		6,			// data; cap switch 9; decreasing threshold trigger
		6,			// data; cap switch 9; increasing threshold trigger
		6,			// data; cap switch 10; decreasing threshold trigger
		6,			// data; cap switch 10; decreasing threshold trigger
		6,			// data; cap switch 11; decreasing threshold trigger
		6,			// data; cap switch 11; decreasing threshold trigger

		16,			// data; timestamped logged data packet (length corresponds to "$TIM,1234567890," prefix)
		31,			// data; playback complete packet ("Logged Data Upload Complete.\r\n")

		24,			// data; RFID TID packet (length corresponds to "$TID,aabbccddeeffgghh)

		27,			// data; shaking event detected
		14,			// data; heart rate monitor event

		14, 		// command; read
		14, 		// command; write

		14,			// ack; positive
		14, 		// ack; negative

		-1,			// special one: SK7 startup info

		10,			// raw data; accelerometer
		10,			// raw data; angular rate (gyro)
		10,			// raw data; magnetometer 
		6,			// raw data; heading
		15,			// raw data; capacitive sensors (note no timestamp/packet counter here)
		15,			// raw data; capacitive sensors (note no timestamp/packet counter here)
		15,			// raw data; capacitive sensors (note no timestamp/packet counter here)
		6,			// raw data; analog input 0
		6,			// raw data; analog input 1
		5,			// raw data; nav switch / cap switch events (never has timestamps/packet counter)
		9,			// raw data; shaking event detected
		10,			// raw data; RPH
		12, 		// raw data; RPH (quaternion)
	};

	// TODO UPDATE
	/* indicates which packet types could have checksums */
	public static int sk7_packet_has_checksum[] = {
		1,1,1,1,1,1,1,1,1,1,	// these are the basic sensor output channels + RPH

		0,0,0,0,				// nav switch events
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,	// cap threshold events
		0,0,					// logger packets
		0,						// RFID packet
		1,						// shaking event
		1,						// heart rate event
		0,0,					// commands
		1,1,					// acks
		0,						// startup info
		0,0,0,0,0,0,0,0,0,0,0,0,0	// raw data packets
	};
}
