package com.ugla.shake.sk6;

public class SK6Packets {
	public static final int SK6_DEFAULT_CHECKSUM = 0x00;

	public static final int SK6_MAX_PACKET_SIZE  = 64;
	public static final int SK6_NUM_PACKET_TYPES = 39;

	public static final int SK6_HEADER_LEN =	4;	// "$pid," == 4 chars
	public static final int SK6_RAW_HEADER_LEN = 3;
	public static final int SK6_PACKET_ID_LEN =	3;

	/* packet types */
	/* ASCII packets */
	public static final int SK6_DATA_ACC = 0;					// accelerometer
	public static final int SK6_DATA_GYRO = 1;					// angular rate (ie gyro readings)
	public static final int SK6_DATA_MAG = 2; 					// magnetometer
	public static final int SK6_DATA_HEADING = 3;					// compass heading
	public static final int SK6_DATA_CAP0 = 4;					// capacitive sensor 0
	public static final int SK6_DATA_CAP1 = 5;					// capacitive sensor 1
	public static final int SK6_DATA_ANA0 = 6;					// analog input 0
	public static final int SK6_DATA_ANA1 = 7;					// analog input 1

	public static final int SK6_DATA_NVU = 8;						// nav switch up
	public static final int SK6_DATA_NVD = 9;						// nav switch down
	public static final int SK6_DATA_NVC = 10;						// nav switch centre
	public static final int SK6_DATA_NVN = 11;						// nav switch release

	public static final int SK6_DATA_CU0 = 12;						// cap switch 0 upper threshold trigger
	public static final int SK6_DATA_CL0 = 13;						// cap switch 0 lower threshold trigger
	public static final int SK6_DATA_CU1 = 14;					// cap switch 1 upper threshold trigger
	public static final int SK6_DATA_CL1 = 15;						// cap switch 1 lower threshold trigger

	public static final int SK6_DATA_TIMESTAMP = 16;			// timestamped logged data 
	public static final int SK6_DATA_PLAYBACK_COMPLETE = 17;		// playback completed (special packet format)

	public static final int SK6_DATA_RFID_TID = 18;				// RFID TID packet

	public static final int SK6_DATA_SHAKING = 19;					// events from shaking detection algorithm
	public static final int SK6_DATA_HEART_RATE = 20;				// events from heart rate monitor
	
	/* command packets */
	public static final int SK6_CMD_READ = 21; 					// read command
	public static final int SK6_CMD_WRITE = 22; 					// write command
	
	/* acknowledgement packets */
	public static final int SK6_ACK_ACK = 23;						// successful ACK
	public static final int SK6_ACK_NEG = 24; 						// negative ACK (ie an error occurred)

	public static final int SK6_STARTUP_INFO = 25;

	/* Raw packets */
	public static final int SK6_RAW_DATA_ACC = 26;
	public static final int SK6_RAW_DATA_GYRO = 27;
	public static final int SK6_RAW_DATA_MAG = 28;
	public static final int SK6_RAW_DATA_HEADING = 29;
	public static final int SK6_RAW_DATA_CAP0 = 30;
	public static final int SK6_RAW_DATA_CAP1 = 31;
	public static final int SK6_RAW_DATA_ANALOG0 = 32;
	public static final int SK6_RAW_DATA_ANALOG1 = 33;
	public static final int SK6_RAW_DATA_EVENT = 34;
	public static final int SK6_RAW_DATA_SHAKING = 35;
	public static final int SK6_RAW_DATA_AUDIO_EXP = 36; 
	public static final int SK6_RAW_DATA_AUDIO_HEADER = 37;
	public static final int SK6_RAW_DATA_AUDIO = 38;

	static String sk6_packet_type_names[] = {
		"ASCII/Acc",
		"ASCII/Gyro",
		"ASCII/Mag",
		"ASCII/Heading",
		"ASCII/Cap0",
		"ASCII/Cap1",
		"ASCII/Ana0",
		"ASCII/Ana1",
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
		"Binary/AudioSTANE",
		"Binary/AudioHeader",
		"Binary/Audio",
	};

	/* packet headers (these directly follow the common header above) */
	public static String sk6_packet_headers[] = {
		"$ACC",		// data; accelerometer
		"$ARS",		// data; angular rate (gyro)
		"$MAG", 	// data; magnetometer
		"$HED", 	// data; heading
		"$CS0",		// data; capacitive sensor 0
		"$CS1",		// data; capacitive sensor 1
		"$AI0",		// data; analog input 0
		"$AI1",		// data; analog input 1

		"$NVU",		// data; nav switch up
		"$NVD",		// data; nav switch down
		"$NVC",		// data; nav switch centre
		"$NVN",		// data; nav switch released

		"$CU0",		// data; cap switch 0; increasing threshold trigger
		"$CL0",		// data; cap switch 0; decreasing threshold trigger
		"$CU1",		// data; cap switch 1; increasing threshold trigger
		"$CL1",		// data; cap switch 1; decreasing threshold trigger

		"$TIM",		// data; timestamped logged data packet
		"Logg",		// data; playback complete packet ("Logged Data Upload Complete.\r\n")

		"$TID",		// data; RFID TID packet

		"$SHK",		// data; shaking event detected
		"$HR ",		// data; heart rate monitor event

		"$REA",		// command; read
		"$WRI",		// command; write

		"$ACK",		// ack; positive
		"$NAK",		// ack; negative

		"\nSHA",		// SK6 startup info
	};

	public static char sk6_raw_packet_headers[] = { 
		126,	// raw acc
		125,	// raw gyro
		124,	// raw mag
		123,	// raw compass
		122,	// raw cap0
		121,	// raw cap1
		120,	// raw ana0
		119,	// raw ana1
		118,	// nav events
		117,	// shaking events
		114,	// expansion module audio
		113,	// audio (headers)
		112,	// audio (data packets)
	};

	// length of checksum part of packet in bytes
	public static final int CHECKSUM_LENGTH = 3;

	/* packet lengths (bytes), WITHOUT INCLUDING CHECKSUMS */
	public static int sk6_packet_lengths[] = {
		27, 		// data; accelerometer
		27,			// data; angular rate (gyro)
		27, 		// data; magnetometer
		14, 		// data; heading
		14, 		// data; capacitive sensor 0
		14,			// data; capacitive sensor 1
		14,			// data: analog input 0
		14,			// data; analog input 1

		6,			// data; nav switch up
		6,			// data; nav switch down
		6,			// data; nav switch centre
		6,			// data; nav switch released

		6,			// data; cap switch 0; increasing threshold trigger
		6,			// data; cap switch 0; decreasing threshold trigger
		6,			// data; cap switch 1; increasing threshold trigger
		6,			// data; cap switch 1; decreasing threshold trigger

		16,			// data; timestamped logged data packet (length corresponds to "$TIM,1234567890," prefix)
		31,			// data; playback complete packet ("Logged Data Upload Complete.\r\n")

		24,			// data; RFID TID packet (length corresponds to "$TID,aabbccddeeffgghh)

		27,			// data; shaking event detected
		14,			// data; heart rate monitor event

		14, 		// command; read
		14, 		// command; write

		14,			// ack; positive
		14, 		// ack; negative

		-1,			// special one: SK6 startup info

		10,			// raw data; accelerometer
		10,			// raw data; angular rate (gyro)
		10,			// raw data; magnetometer 
		6,			// raw data; heading
		6,			// raw data; capacitive sensor 0
		6,			// raw data; capacitive sensor 1
		6,			// raw data; analog input 0
		6,			// raw data; analog input 1
		5,			// raw data; nav switch / cap switch events
		9,			// raw data; shaking event detected
		35,			// raw data; audio sample (extension module contact mic)
		3,			// raw data: audio header (for playback flow control)
		35,			// raw data; (audio sample)
	};

	/* indicates which packet types could have checksums */
	public static int sk6_packet_has_checksum[] = {
		1,1,1,1,1,1,1,1,		// these are the 8 basic sensor output channels

		0,0,0,0,0,0,0,0,		// nav switch and cap switch events
		0,0,					// logger packets
		0,						// RFID packet
		1,						// shaking event
		1,						// heart rate event
		0,0,					// commands
		1,1,					// acks
		0,0,0,0,0,0,0,0,0,0,0,0,0	// raw data packets
	};
}
