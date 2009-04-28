package SHAKE;

import java.nio.ByteBuffer;

public class SHAKEPackets {
	
	public static final int SHAKE_BAD_PACKET = -1;
	
	public static final int SHAKE_NUM_PACKET_TYPES = 38;
	public static final int SHAKE_HEADER_LEN = 4;
	public static final int SHAKE_RAW_HEADER_LEN = 3;
	public static final int SHAKE_PACKET_ID_LEN = 3;

	public static final int SHAKE_DATA_ACC = 0;					// accelerometer
	public static final int SHAKE_DATA_GYRO = 1;					// angular rate (ie gyro readings)
	public static final int SHAKE_DATA_MAG = 2;					// magnetometer
	public static final int SHAKE_DATA_HEADING = 3;					// compass heading
	public static final int SHAKE_DATA_CAP0 = 4;					// capacitive sensor 0
	public static final int SHAKE_DATA_CAP1 = 5;					// capacitive sensor 1
	public static final int SHAKE_DATA_ANA0 = 6;					// analog input 0
	public static final int SHAKE_DATA_ANA1 = 7;					// analog input 1

	public static final int SHAKE_DATA_NVU = 8;				// nav switch up
	public static final int SHAKE_DATA_NVD = 9;				// nav switch down
	public static final int SHAKE_DATA_NVC = 10;				// nav switch centre
	public static final int SHAKE_DATA_NVN = 11;				// nav switch release

	public static final int SHAKE_DATA_CU0 = 12;				// cap switch 0 upper threshold trigger
	public static final int SHAKE_DATA_CL0 = 13;					// cap switch 0 lower threshold trigger
	public static final int SHAKE_DATA_CU1 = 14;				// cap switch 1 upper threshold trigger
	public static final int SHAKE_DATA_CL1 = 15;				// cap switch 1 lower threshold trigger

	public static final int SHAKE_DATA_TIMESTAMP = 16;				// timestamped logged data 
	public static final int SHAKE_DATA_PLAYBACK_COMPLETE = 17;		// playback completed (special packet format)

	public static final int SHAKE_DATA_RFID_TID = 18;				// RFID TID packet

	public static final int SHAKE_DATA_SHAKING = 19;			// events from shaking detection algorithm
	public static final int SHAKE_DATA_HEART_RATE = 20;				// events from heart rate monitor
		
	public static final int SHAKE_CMD_READ = 21; 					// read command
	public static final int SHAKE_CMD_WRITE = 22; 					// write command
		
	public static final int SHAKE_ACK_ACK = 23;						// successful ACK
	public static final int SHAKE_ACK_NEG = 24;						// negative ACK (ie an error occurred)
	
	public static final int SHAKE_STARTUP_INFO = 25;
	
	public static final int SHAKE_RAW_DATA_ACC = 26;
	public static final int SHAKE_RAW_DATA_GYRO = 27;
	public static final int SHAKE_RAW_DATA_MAG = 28;
	public static final int SHAKE_RAW_DATA_HEADING = 29;
	public static final int SHAKE_RAW_DATA_CAP0 = 30;	
	public static final int SHAKE_RAW_DATA_CAP1 = 31;
	public static final int SHAKE_RAW_DATA_ANALOG0 = 32;
	public static final int SHAKE_RAW_DATA_ANALOG1 = 33;
	public static final int SHAKE_RAW_DATA_EVENT = 34;
	public static final int SHAKE_RAW_DATA_SHAKING = 35;
	public static final int SHAKE_RAW_DATA_AUDIO_EXP = 36;
	public static final int SHAKE_RAW_DATA_AUDIO_HEADER = 37;
	public static final int SHAKE_RAW_DATA_AUDIO = 38;
	
	public static int[] shake_packet_types = { 
		0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 12, 14, 15,
		16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 
		31, 32, 33, 34, 35, 36, 37, 38
	};
	
	public static byte[][] shake_byte_packet_headers = new byte[39][4];
	private static boolean headers_initialised = false;
	
	public static void initialiseHeaders() {
		if(headers_initialised)
			return;
		
		for(int i=0;i<shake_packet_headers.length;i++) {
			shake_byte_packet_headers[i] = shake_packet_headers[i].getBytes();
		}
		headers_initialised = true;
	}

	public static String shake_packet_headers[] = {
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
		
		"\nSHA",
	};

	static char shake_raw_packet_headers[] = { 
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

	public static int shake_packet_lengths[] = {
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
		
		-1, 		// 

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

	public static int shake_packet_has_checksum[] = {
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
	
	public static boolean isASCIIHeader(byte[] b, int len) {
		if(b == null)
			return false;
		
		if(len != SHAKE_HEADER_LEN)
			return false;
		
		if(b[0] != '$')
			return false;
		
		return true;
	}
	
	public static boolean isRawHeader(byte[] b, int len) {
		if(b == null) 
			return false;
		
		if(len != SHAKE_RAW_HEADER_LEN)
			return false;
		
		if(b[0] != 0x7F || b[1] != 0x7F)
			return false;
		
		return true;
	}
	
	public static boolean hasChecksum(int packet_type) {
		if(shake_packet_has_checksum[packet_type] == 0)
			return false;
		return true;
	}
	
	public static int classifyHeader(byte[] b, int len, boolean ascii_mode) {
		int type = SHAKE_BAD_PACKET;
		
		if(ascii_mode && (len != SHAKE_HEADER_LEN || b == null))
			return type;
		if(!ascii_mode && (len != SHAKE_RAW_HEADER_LEN || b == null || b[0] != 0x7F || b[1] != 0x7F))
			return type;
		
		int i = 0;
		
		if(ascii_mode) {
			int limit = SHAKE_RAW_DATA_ACC;
			while(i < limit) {
				if(SHAKEUtils.memcmp(shake_byte_packet_headers[i], b, 0, 4))
					return i;
				++i;
			}
		} else {
			i = SHAKE_RAW_DATA_ACC;
			int limit = shake_packet_types.length;
			while(i < limit) {
				if(b[2] == shake_raw_packet_headers[i - SHAKE_RAW_DATA_ACC])
					return i;
				++i;
			}
		}
		
		return type;
	}
	
	public static int parseACK(internalSHAKE is, byte[] rawpacket) {
		is.lastaddr = SHAKEUtils.convertHexASCIIToInt(rawpacket, 5, 4, 4);
		is.lastval = SHAKEUtils.convertHexASCIIToInt(rawpacket, 10, 2, 2);
		return SHAKEConstants.SHAKE_SUCCESS;
	}
	
	public static boolean parseASCIIPacket(internalSHAKE is, int pkttype, byte[] rawpacket, boolean checksum) {
		int ev = -1;
		
		switch(pkttype) {
			case SHAKE_DATA_ACC: 
				is.acc[0] = SHAKEUtils.convertASCIIToInt(rawpacket, 5, 5, 4);
				is.acc[1] = SHAKEUtils.convertASCIIToInt(rawpacket, 11, 5, 4);
				is.acc[2] = SHAKEUtils.convertASCIIToInt(rawpacket, 17, 5, 4);
				is.current_timestamps[SHAKEConstants.SHAKE_SENSOR_ACC] = SHAKEUtils.convertASCIIToInt(rawpacket, 23, 2, 2);
				break;
			case SHAKE_DATA_GYRO: 
				is.gyro[0] = SHAKEUtils.convertASCIIToInt(rawpacket, 5, 5, 4);
				is.gyro[1] = SHAKEUtils.convertASCIIToInt(rawpacket, 11, 5, 4);
				is.gyro[2] = SHAKEUtils.convertASCIIToInt(rawpacket, 17, 5, 4);
				is.current_timestamps[SHAKEConstants.SHAKE_SENSOR_GYRO] = SHAKEUtils.convertASCIIToInt(rawpacket, 23, 2, 2);
				break;
			case SHAKE_DATA_MAG: 
				is.mag[0] = SHAKEUtils.convertASCIIToInt(rawpacket, 5, 5, 4);
				is.mag[1] = SHAKEUtils.convertASCIIToInt(rawpacket, 11, 5, 4);
				is.mag[2] = SHAKEUtils.convertASCIIToInt(rawpacket, 17, 5, 4);
				is.current_timestamps[SHAKEConstants.SHAKE_SENSOR_MAG] = SHAKEUtils.convertASCIIToInt(rawpacket, 23, 2, 2);
				break;
			case SHAKE_DATA_HEADING:
				is.heading = SHAKEUtils.convertASCIIToInt(rawpacket, 5, 4, 4);
				is.current_timestamps[SHAKEConstants.SHAKE_SENSOR_HEADING] = SHAKEUtils.convertASCIIToInt(rawpacket, 10, 2, 2);
				break;
			case SHAKE_DATA_CAP0: 
				is.cap[0] = SHAKEUtils.convertASCIIToInt(rawpacket, 5, 4, 4);
				is.current_timestamps[SHAKEConstants.SHAKE_SENSOR_CAP0] = SHAKEUtils.convertASCIIToInt(rawpacket, 10, 2, 2);
				break;
			case SHAKE_DATA_CAP1: 
				is.cap[1] = SHAKEUtils.convertASCIIToInt(rawpacket, 5, 4, 4);
				is.current_timestamps[SHAKEConstants.SHAKE_SENSOR_CAP1] = SHAKEUtils.convertASCIIToInt(rawpacket, 10, 2, 2);
				break;
			case SHAKE_DATA_ANA0: 
				is.ana[0] = SHAKEUtils.convertASCIIToInt(rawpacket, 5, 4, 4);
				is.current_timestamps[SHAKEConstants.SHAKE_SENSOR_ANA0] = SHAKEUtils.convertASCIIToInt(rawpacket, 10, 2, 2);
				break;
			case SHAKE_DATA_ANA1:
				is.ana[1] = SHAKEUtils.convertASCIIToInt(rawpacket, 5, 4, 4);
				is.current_timestamps[SHAKEConstants.SHAKE_SENSOR_ANA1] = SHAKEUtils.convertASCIIToInt(rawpacket, 10, 2, 2);
				break;
			case SHAKE_DATA_NVU:
			case SHAKE_DATA_NVD:
			case SHAKE_DATA_NVC:
			case SHAKE_DATA_NVN:
				if(is.eventCallback != null) {
					switch(rawpacket[3]) {
						case 'U':
							ev = SHAKEConstants.SHAKE_NAV_UP;
							break;
						case 'D':
							ev = SHAKEConstants.SHAKE_NAV_DOWN;
							break;
						case 'C':
							ev = SHAKEConstants.SHAKE_NAV_CENTRE;
							break;
						case 'N':
							ev = SHAKEConstants.SHAKE_NAV_NORMAL;
							break;
					}
					is.eventCallback.SHAKE_Event(ev);
				}
				break;
			case SHAKE_DATA_CU0:
				if (is.eventCallback != null)
					is.eventCallback.SHAKE_Event(SHAKEConstants.SHAKE_CS0_UPPER);
				break;
			case SHAKE_DATA_CL0:
				if (is.eventCallback != null)
					is.eventCallback.SHAKE_Event(SHAKEConstants.SHAKE_CS0_LOWER);
				break;
			case SHAKE_DATA_CU1:
				if (is.eventCallback != null)
					is.eventCallback.SHAKE_Event(SHAKEConstants.SHAKE_CS1_UPPER);
				break;
			case SHAKE_DATA_CL1:
				if (is.eventCallback != null)
					is.eventCallback.SHAKE_Event(SHAKEConstants.SHAKE_CS1_LOWER);
				break;
			case SHAKE_DATA_SHAKING: 
				if(is.eventCallback != null) {
					is.shaking_peak_accel = SHAKEUtils.convertASCIIToInt(rawpacket, 5, 5, 4);
					is.shaking_direction = SHAKEUtils.convertASCIIToInt(rawpacket, 11, 5, 4);
					is.shaking_timestamp = SHAKEUtils.convertASCIIToInt(rawpacket, 17, 5, 4);
					is.eventCallback.SHAKE_Event(SHAKEConstants.SHAKE_SHAKING_EVENT);
				}
				break;
			case SHAKE_DATA_HEART_RATE: 
				if(is.eventCallback != null) {
					is.hr_bpm = SHAKEUtils.convertASCIIToInt(rawpacket, 5, 4, 4);
				}
				break;
			
		}
		return true;
	}
	
	public static short fixBytes(int b1, int b2) {
		return (short)((b1 & 0x00ff) + ((b2 << 8)));
	}

	public static boolean parseRawPacket(internalSHAKE is, int pkttype, byte[] pkt, boolean has_seq) {
		switch(pkttype) {
			case SHAKEPackets.SHAKE_RAW_DATA_ACC:
				is.acc[0] = fixBytes(pkt[3], pkt[4]);
				is.acc[1] = fixBytes(pkt[5], pkt[6]);
				is.acc[2] = fixBytes(pkt[7], pkt[8]);
                if(is.dbg != null) is.dbg.debug("parseACC (raw): " + is.acc[0] + ", " + is.acc[1] + ", " + is.acc[2]);
                                
				if(has_seq) is.current_timestamps[SHAKEConstants.SHAKE_SENSOR_ACC] = 0xFF & pkt[9];
				break;
			case SHAKEPackets.SHAKE_RAW_DATA_GYRO:
				is.gyro[0] = fixBytes(pkt[3], pkt[4]);
				is.gyro[1] = fixBytes(pkt[5], pkt[6]);
				is.gyro[2] = fixBytes(pkt[7], pkt[8]);
				if(has_seq) is.current_timestamps[SHAKEConstants.SHAKE_SENSOR_GYRO] = 0xFF & pkt[9];
				break;
			case SHAKEPackets.SHAKE_RAW_DATA_MAG:
				is.mag[0] = fixBytes(pkt[3], pkt[4]);
				is.mag[1] = fixBytes(pkt[5], pkt[6]);
				is.mag[2] = fixBytes(pkt[7], pkt[8]);
				if(has_seq) is.current_timestamps[SHAKEConstants.SHAKE_SENSOR_MAG] = 0xFF & pkt[9];
				break;
			case SHAKEPackets.SHAKE_RAW_DATA_HEADING:
				is.heading = fixBytes(pkt[3], pkt[4]);
				if(has_seq) is.current_timestamps[SHAKEConstants.SHAKE_SENSOR_HEADING] = 0xFF & pkt[5];
				break;
			case SHAKEPackets.SHAKE_RAW_DATA_CAP0:
				is.cap[0] = fixBytes(pkt[3], pkt[4]);
				if(has_seq) is.current_timestamps[SHAKEConstants.SHAKE_SENSOR_CAP0] = 0xFF & pkt[5];
				break;
			case SHAKEPackets.SHAKE_RAW_DATA_CAP1:
				is.cap[1] = fixBytes(pkt[3], pkt[4]);
				if(has_seq) is.current_timestamps[SHAKEConstants.SHAKE_SENSOR_CAP1] = 0xFF & pkt[5];
				break;
			case SHAKEPackets.SHAKE_RAW_DATA_ANALOG0:
				is.ana[0] = fixBytes(pkt[3], pkt[4]);
				if(has_seq) is.current_timestamps[SHAKEConstants.SHAKE_SENSOR_ANA0] = 0xFF & pkt[5];
				break;
			case SHAKEPackets.SHAKE_RAW_DATA_ANALOG1:
				is.ana[1] = fixBytes(pkt[3], pkt[4]);
				if(has_seq) is.current_timestamps[SHAKEConstants.SHAKE_SENSOR_ANA1] = 0xFF & pkt[5];
				break;
			case SHAKEPackets.SHAKE_RAW_DATA_EVENT:
				if(is.eventCallback != null) {
					int t = -1;
					switch(pkt[3]) {
						case 1:
							t = SHAKEConstants.SHAKE_NAV_NORMAL;
							break;
						case 2:
							t = SHAKEConstants.SHAKE_NAV_UP;
							break;
						case 3:
							t = SHAKEConstants.SHAKE_NAV_DOWN;
							break;
						case 4:
							t = SHAKEConstants.SHAKE_NAV_CENTRE;
							break;
						case 5:
							t = SHAKEConstants.SHAKE_CS0_UPPER;
							break;
						case 6:
							t = SHAKEConstants.SHAKE_CS0_LOWER;
							break;
						case 7:
							t = SHAKEConstants.SHAKE_CS1_UPPER;
							break;
						case 8:
							t = SHAKEConstants.SHAKE_CS1_LOWER;
							break;
					}
					is.lastevent = t;
					is.eventCallback.SHAKE_Event(t);
				}
				break;
			case SHAKEPackets.SHAKE_RAW_DATA_SHAKING:
				is.shaking_peak_accel = fixBytes(pkt[3], pkt[4]);
				is.shaking_direction = fixBytes(pkt[5], pkt[6]);
				is.shaking_timestamp = fixBytes(pkt[7], pkt[8]);
				if(is.eventCallback != null) {
					is.lastevent = SHAKEConstants.SHAKE_SHAKING_EVENT;
					is.eventCallback.SHAKE_Event(is.lastevent);
				}
				break;
			case SHAKEPackets.SHAKE_RAW_DATA_AUDIO:
				// TODO
				break;
			case SHAKEPackets.SHAKE_RAW_DATA_AUDIO_EXP:
				// TODO
				break;
			case SHAKEPackets.SHAKE_RAW_DATA_AUDIO_HEADER:
				// TODO
				break;
		}
		
		return true;
	}
}
