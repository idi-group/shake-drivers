package com.ugla.shake.sk7;

import com.ugla.shake.ShakeConstants;
import com.ugla.shake.ShakeDevice;
import com.ugla.shake.ShakeDevicePrivate;
import com.ugla.shake.ShakeEvents;
import com.ugla.shake.common.Shake;
import com.ugla.shake.common.ShakeUtils;
import com.ugla.shake.sk7.SK7Parsing;

public class ShakeSk7 extends Shake {

	public ShakeSk7(ShakeDevice sd, ShakeDevicePrivate sdp) {
		super(sd, sdp);
	}

	@Override
	public int parse_ascii_packet(int packet_type, byte[] packetbuf, int packetlen, boolean playback) {
		if(packet_type != SK7Packets.SK7_ACK_ACK && packet_type != SK7Packets.SK7_ACK_NEG) {
			extract_ascii_packet(packet_type, packetbuf, playback);
		} else {
			if(!devpriv.waiting_for_ack)
				return SK7Parsing.SK7_ASCII_READ_ERROR;
			
			if(packet_type == SK7Packets.SK7_ACK_ACK)
				devpriv.lastack = true;
			else
				devpriv.lastack = false;
			
			parse_ack_packet(packetbuf);
			
			devpriv.waiting_for_ack_signal = false;
		}
		return SK7Parsing.SK7_ASCII_READ_OK;
	}

	@Override
	public int read_ascii_packet(int packet_type, byte[] packetbuf) {
		int packet_size = 0, bytes_left, bytes_read = 0;
		boolean playback = false;
		
		if(packet_type == SK7Packets.SK7_DATA_TIMESTAMP) {
			// TODO full timestamp packet support
			devpriv.readBytesWrapper(packetbuf, 
					SK7Packets.sk7_packet_lengths[SK7Packets.SK7_DATA_TIMESTAMP] - SK7Packets.SK7_HEADER_LEN, 
					SK7Packets.SK7_HEADER_LEN);
			
			// parse timestamp header here
			playback = true;
			devpriv.readBytesWrapper(packetbuf, SK7Packets.SK7_HEADER_LEN, 0);
			packet_type = classify_packet_header(packetbuf, SK7Packets.SK7_HEADER_LEN, true);
		} else if(packet_type == SK7Packets.SK7_DATA_PLAYBACK_COMPLETE) {
			devpriv.readBytesWrapper(packetbuf, 
					SK7Packets.sk7_packet_lengths[packet_type] - SK7Packets.SK7_HEADER_LEN, 
					SK7Packets.SK7_HEADER_LEN);
			
			if(callback != null) {
				devpriv.lastevent = ShakeEvents.SHAKE_PLAYBACK_COMPLETE;
				callback.ShakeEvent(devpriv.lastevent);
			}
			return SK7Parsing.SK7_ASCII_READ_CONTINUE;
		} else if (packet_type == SK7Packets.SK7_DATA_RFID_TID) {
			devpriv.readBytesWrapper(packetbuf, 
					SK7Packets.sk7_packet_lengths[packet_type] - SK7Packets.SK7_HEADER_LEN, 
					SK7Packets.SK7_HEADER_LEN);
			String tmp = new String(packetbuf);
			devpriv.lasttid = tmp.substring(SK7Packets.SK7_HEADER_LEN+1, 16);
			
			if(callback != null) {
				devpriv.lastevent = ShakeEvents.SHAKE_RFID_TID_EVENT;
				callback.ShakeEvent(devpriv.lastevent);
			}
			return SK7Parsing.SK7_ASCII_READ_CONTINUE;
		} else if (packet_type == SK7Packets.SK7_STARTUP_INFO) {
			read_device_info();
			return SK7Parsing.SK7_ASCII_READ_CONTINUE;
		}
		
		bytes_left = SK7Packets.sk7_packet_lengths[packet_type] - SK7Packets.SK7_HEADER_LEN;
		
		if(playback)
			bytes_left -= 3;
		
		bytes_read = devpriv.readBytesWrapper(packetbuf, bytes_left, SK7Packets.SK7_HEADER_LEN);
		if(bytes_read != bytes_left)
			return SK7Parsing.SK7_ASCII_READ_ERROR;
		
		if(playback) {
			int offset = bytes_read + SK7Packets.SK7_HEADER_LEN - 2;
			packetbuf[offset] = ',';
			packetbuf[offset+1] = '0';
			packetbuf[offset+2] = '0';
			packetbuf[offset+3] = '\r';
			packetbuf[offset+4] = '\n';
			bytes_read += 3;
		}
		
		if(SK7Packets.sk7_packet_has_checksum[packet_type] == 1
		        && packetbuf[bytes_read + SK7Packets.SK7_HEADER_LEN - 1] != 0xA) {
			devpriv.checksum = true;
			devpriv.readBytesWrapper(packetbuf, SK7Packets.CHECKSUM_LENGTH, SK7Packets.SK7_HEADER_LEN + bytes_read);
			bytes_read += 3;
		} else if(SK7Packets.sk7_packet_has_checksum[packet_type] == 1 
				&& packetbuf[bytes_read + SK7Packets.SK7_HEADER_LEN - 1] == 0xA
				&& devpriv.checksum) {
			devpriv.checksum = false;
		}
		
		packet_size = bytes_read + SK7Packets.SK7_HEADER_LEN;
		
		if(devpriv.closed)
			return SK7Parsing.SK7_ASCII_READ_ERROR;
		
		return parse_ascii_packet(packet_type, packetbuf, packet_size, playback);
	}

	@Override
	public int parse_raw_packet(int packet_type, byte[] packetbuf, int packetlen, boolean has_seq) {
		extract_raw_packet(packet_type, packetbuf, has_seq);
		// TODO?
		//if(packet_type == SK7Packets.SK7_RAW_DATA_AUDIO_HEADER) {}
		return SK7Parsing.SK7_RAW_READ_OK;
	}

	@Override
	public int read_raw_packet(int packet_type, byte[] packetbuf) {
		int bytes_left, bytes_read = 0;
		bytes_left = SK7Packets.sk7_packet_lengths[packet_type] - SK7Packets.SK7_RAW_HEADER_LEN;
		bytes_read = devpriv.readBytesWrapper(packetbuf, bytes_left, SK7Packets.SK7_RAW_HEADER_LEN);
		
		if(bytes_left - bytes_read > 1) {
			return SK7Parsing.SK7_RAW_READ_ERROR;
		}
		
		boolean has_seq = false;
		devpriv.peek_flag = false;
		
		if(bytes_left == bytes_read) {
			int packet_len = SK7Packets.sk7_packet_lengths[packet_type];
			int trailing_byte = packetbuf[packet_len - 1];
			
			if(trailing_byte == 0x7F) {
				devpriv.peek = 0x7F;
				devpriv.peek_flag = true;
			} else if(trailing_byte == '$' || trailing_byte == '\n') {
				int adjtype = packet_type - SK7Packets.SK7_RAW_DATA_ACC;
				if(trailing_byte == data.internal_timestamps[adjtype] + 1
						|| (trailing_byte == 0 && data.internal_timestamps[adjtype] == 255)) {
					has_seq = true;
				} else {
					devpriv.peek = (short) trailing_byte;
					devpriv.peek_flag = true;
				}
			} else {
				has_seq = false;
			}
		} else {
			
		}
		
		return parse_raw_packet(packet_type, packetbuf, bytes_read, has_seq);
	}

	@Override
	public int get_next_packet(byte[] packetbuf, int bufsize) {
		int bytes_read = 0, packet_type = ShakeConstants.SHAKE_BAD_PACKET;
		
		bytes_read = devpriv.readBytesWrapper(packetbuf, 3, 0);
		
		if(bytes_read == 3) {
			if(packetbuf[0] == 0x7F && packetbuf[1] == 0x7F) {
				packet_type = classify_packet_header(packetbuf, SK7Packets.SK7_RAW_HEADER_LEN, false);
			} else if(packetbuf[0] == '$' || packetbuf[0] == '\n') {
				devpriv.readBytesWrapper(packetbuf, 1, 3);
				packet_type = classify_packet_header(packetbuf, SK7Packets.SK7_HEADER_LEN, true);
			} else if(ShakeUtils.memcmp("Log".getBytes(), packetbuf)) {
				devpriv.readBytesWrapper(packetbuf, 1, 3);
				packet_type = classify_packet_header(packetbuf, SK7Packets.SK7_HEADER_LEN, true);
			}
		}
		
		if(packet_type == ShakeConstants.SHAKE_BAD_PACKET) {
			System.out.println("PARSE ERROR");
			int i;
			byte[] c=  new byte[1];
			
			int read_count = 0;
			while(read_count++ < 50 && (c[0] != '$' && c[0] != 0x7F)) {
				devpriv.readBytesWrapper(c, 1, 0);
			}
			
			packetbuf[0] = c[0];
			
			if(c[0] == '$') {
				devpriv.readBytesWrapper(packetbuf, SK7Packets.SK7_HEADER_LEN - 1, 1);
				packet_type = classify_packet_header(packetbuf, SK7Packets.SK7_HEADER_LEN, true);
			} else if(c[0] == 0x7F) {
				devpriv.readBytesWrapper(packetbuf, SK7Packets.SK7_RAW_HEADER_LEN - 1, 1);
				packet_type = classify_packet_header(packetbuf, SK7Packets.SK7_RAW_HEADER_LEN, false);
			}
		}
		
		return packet_type;
	}
	
	@Override
	public int parse_packet(byte[] packetbuf, int packet_type) {
		if(is_ascii_packet(packet_type)) {
			read_ascii_packet(packet_type, packetbuf);
		} else {
			read_raw_packet(packet_type, packetbuf);
		}
		return 1;
	}

	@Override
	public boolean is_ascii_packet(int packet_type) {
		if(packet_type >= SK7Packets.SK7_DATA_ACC && packet_type < SK7Packets.SK7_RAW_DATA_ACC)
			return true;
		return false;
	}

	@Override
	public int parse_ack_packet(byte[] packetbuf) {
		devpriv.lastaddr = ShakeUtils.convertHexASCIIToInt(packetbuf, 5, 4, 4);
		devpriv.lastval = ShakeUtils.convertHexASCIIToInt(packetbuf, 10, 2, 2);
		return ShakeConstants.SHAKE_SUCCESS;
	}

	@Override
	public int extract_ascii_packet(int packet_type, byte[] rawpacket, boolean playback) {
		int ev = -1;
		
		switch(packet_type) {
			case SK7Packets.SK7_DATA_ACC:
				data.acc[0] = ShakeUtils.convertASCIIToInt(rawpacket, 5, 5, 4);
				data.acc[1] = ShakeUtils.convertASCIIToInt(rawpacket, 11, 5, 4);
				data.acc[2] = ShakeUtils.convertASCIIToInt(rawpacket, 17, 5, 4);
				data.internal_timestamps[ShakeConstants.SHAKE_SENSOR_ACC] = ShakeUtils.convertASCIIToInt(rawpacket, 23, 2, 2);
				break;
			case SK7Packets.SK7_DATA_GYRO:
				data.gyro[0] = ShakeUtils.convertASCIIToInt(rawpacket, 5, 5, 4);
				data.gyro[1] = ShakeUtils.convertASCIIToInt(rawpacket, 11, 5, 4);
				data.gyro[2] = ShakeUtils.convertASCIIToInt(rawpacket, 17, 5, 4);
				data.internal_timestamps[ShakeConstants.SHAKE_SENSOR_GYRO] = ShakeUtils.convertASCIIToInt(rawpacket, 23, 2, 2);
				break;
			case SK7Packets.SK7_DATA_MAG:
				data.mag[0] = ShakeUtils.convertASCIIToInt(rawpacket, 5, 5, 4);
				data.mag[1] = ShakeUtils.convertASCIIToInt(rawpacket, 11, 5, 4);
				data.mag[2] = ShakeUtils.convertASCIIToInt(rawpacket, 17, 5, 4);
				data.internal_timestamps[ShakeConstants.SHAKE_SENSOR_MAG] = ShakeUtils.convertASCIIToInt(rawpacket, 23, 2, 2);
				break;
			case SK7Packets.SK7_DATA_HEADING:
				data.heading = ShakeUtils.convertASCIIToInt(rawpacket, 5, 4, 4);
				data.internal_timestamps[ShakeConstants.SHAKE_SENSOR_HEADING] = ShakeUtils.convertASCIIToInt(rawpacket, 10, 2, 2);
				break;
			case SK7Packets.SK7_DATA_CAP: {
				int offset = 5;
				for(int i=0;i<12;i++) {
					data.cap_sk7[0][i] = ShakeUtils.convertHexASCIIToInt(rawpacket, offset, 2, 2);
					offset += 3;
				}
				data.internal_timestamps[ShakeConstants.SHAKE_SENSOR_SK7_CAP] = ShakeUtils.convertASCIIToInt(rawpacket, offset, 2, 2); 
				break;
			}
			case SK7Packets.SK7_DATA_CAP_B: {
				int offset = 5;
				for(int i=0;i<12;i++) {
					data.cap_sk7[1][i] = ShakeUtils.convertHexASCIIToInt(rawpacket, offset, 2, 2);
					offset += 3;
				}
				data.internal_timestamps[ShakeConstants.SHAKE_SENSOR_SK7_CAP] = ShakeUtils.convertASCIIToInt(rawpacket, offset, 2, 2); 
				break;
			}
			case SK7Packets.SK7_DATA_CAP_C: {
				int offset = 5;
				for(int i=0;i<12;i++) {
					data.cap_sk7[2][i] = ShakeUtils.convertHexASCIIToInt(rawpacket, offset, 2, 2);
					offset += 3;
				}
				data.internal_timestamps[ShakeConstants.SHAKE_SENSOR_SK7_CAP] = ShakeUtils.convertASCIIToInt(rawpacket, offset, 2, 2); 
				break;
			}
			case SK7Packets.SK7_DATA_ANA0:
				data.ana[0] = ShakeUtils.convertASCIIToInt(rawpacket, 5, 4, 4);
				data.internal_timestamps[ShakeConstants.SHAKE_SENSOR_ANA0] = ShakeUtils.convertASCIIToInt(rawpacket, 10, 2, 2);
				break;
			case SK7Packets.SK7_DATA_ANA1:
				data.ana[1] = ShakeUtils.convertASCIIToInt(rawpacket, 5, 4, 4);
				data.internal_timestamps[ShakeConstants.SHAKE_SENSOR_ANA1] = ShakeUtils.convertASCIIToInt(rawpacket, 10, 2, 2);
				break;
			case SK7Packets.SK7_DATA_RPH:
				data.rph[0] = ShakeUtils.convertASCIIToInt(rawpacket, 5, 5, 4);
				data.rph[1] = ShakeUtils.convertASCIIToInt(rawpacket, 11, 5, 4);
				data.rph[2] = ShakeUtils.convertASCIIToInt(rawpacket, 17, 5, 4);
				data.heading = data.rph[2];
				data.internal_timestamps[ShakeConstants.SHAKE_SENSOR_HEADING] = ShakeUtils.convertASCIIToInt(rawpacket, 23, 2, 2);
				break;
			case SK7Packets.SK7_DATA_NVU:
			case SK7Packets.SK7_DATA_NVD:
			case SK7Packets.SK7_DATA_NVC:
			case SK7Packets.SK7_DATA_NVN:
				if(callback != null) {
					switch(rawpacket[3]) {
						case 'U':
							ev = ShakeEvents.SHAKE_NAV_UP;
							break;
						case 'D':
							ev = ShakeEvents.SHAKE_NAV_DOWN;
							break;
						case 'C':
							ev = ShakeEvents.SHAKE_NAV_CENTRE;
							break;
						case 'N':
							ev = ShakeEvents.SHAKE_NAV_NORMAL;
							break;
					}
					callback.ShakeEvent(ev);
				}
				break;
			case SK7Packets.SK7_DATA_CU0: case SK7Packets.SK7_DATA_CL0: case SK7Packets.SK7_DATA_CU1: case SK7Packets.SK7_DATA_CL1:
			case SK7Packets.SK7_DATA_CU2: case SK7Packets.SK7_DATA_CL2: case SK7Packets.SK7_DATA_CU3: case SK7Packets.SK7_DATA_CL3:
			case SK7Packets.SK7_DATA_CU4: case SK7Packets.SK7_DATA_CL4: case SK7Packets.SK7_DATA_CU5: case SK7Packets.SK7_DATA_CL5:
			case SK7Packets.SK7_DATA_CU6: case SK7Packets.SK7_DATA_CL6: case SK7Packets.SK7_DATA_CU7: case SK7Packets.SK7_DATA_CL7:
			case SK7Packets.SK7_DATA_CU8: case SK7Packets.SK7_DATA_CL8: case SK7Packets.SK7_DATA_CU9: case SK7Packets.SK7_DATA_CL9:
			case SK7Packets.SK7_DATA_CUA: case SK7Packets.SK7_DATA_CLA: case SK7Packets.SK7_DATA_CUB: case SK7Packets.SK7_DATA_CLB:
				ev = ShakeEvents.SK7_CS0_UPPER + (packet_type - SK7Packets.SK7_DATA_CU0);
				if(callback != null) {
					callback.ShakeEvent(ev);
				}
				break;
			case SK7Packets.SK7_DATA_SHAKING:
				if(callback != null) {
					data.shaking_peak_accel = ShakeUtils.convertASCIIToInt(rawpacket, 5, 5, 4);
					data.shaking_direction = ShakeUtils.convertASCIIToInt(rawpacket, 11, 5, 4);
					data.shaking_timestamp = ShakeUtils.convertASCIIToInt(rawpacket, 17, 5, 4);
					callback.ShakeEvent(ShakeEvents.SHAKE_SHAKING_EVENT);
				}
				break;
			case SK7Packets.SK7_DATA_HEART_RATE:
				if(callback != null) {
					data.hr_bpm = ShakeUtils.convertASCIIToInt(rawpacket, 5, 4, 4);
					callback.ShakeEvent(ShakeEvents.SHAKE_HEART_RATE_EVENT);
				}
				break;
			default:
				return -1;
		}
		return 1;
	}
	
	public static short fixBytes(int b1, int b2) {
		return (short)((b1 & 0x00ff) + ((b2 << 8)));
	}

	@Override
	public int extract_raw_packet(int packet_type, byte[] rawpacket, boolean has_seq) {
		switch(packet_type) {
			case SK7Packets.SK7_RAW_DATA_ACC:
				data.acc[0] = fixBytes(rawpacket[3], rawpacket[4]);
				data.acc[1] = fixBytes(rawpacket[5], rawpacket[6]);
				data.acc[2] = fixBytes(rawpacket[7], rawpacket[8]);            
				if(has_seq) data.internal_timestamps[ShakeConstants.SHAKE_SENSOR_ACC] = 0xFF & rawpacket[9];
				break;
			case SK7Packets.SK7_RAW_DATA_GYRO:
				data.gyro[0] = fixBytes(rawpacket[3], rawpacket[4]);
				data.gyro[1] = fixBytes(rawpacket[5], rawpacket[6]);
				data.gyro[2] = fixBytes(rawpacket[7], rawpacket[8]);            
				if(has_seq) data.internal_timestamps[ShakeConstants.SHAKE_SENSOR_GYRO] = 0xFF & rawpacket[9];
				break;
			case SK7Packets.SK7_RAW_DATA_MAG:
				data.mag[0] = fixBytes(rawpacket[3], rawpacket[4]);
				data.mag[1] = fixBytes(rawpacket[5], rawpacket[6]);
				data.mag[2] = fixBytes(rawpacket[7], rawpacket[8]);            
				if(has_seq) data.internal_timestamps[ShakeConstants.SHAKE_SENSOR_MAG] = 0xFF & rawpacket[9];
				break;
			case SK7Packets.SK7_RAW_DATA_HEADING:
				data.heading = fixBytes(rawpacket[3], rawpacket[4]);
				if(has_seq) data.internal_timestamps[ShakeConstants.SHAKE_SENSOR_HEADING] = 0xFF & rawpacket[5];
				break;
			case SK7Packets.SK7_RAW_DATA_CAP:
				for(int i=0;i<12;i++) {
					data.cap_sk7[0][i] = rawpacket[3+i] & 0x00ff;
				}
				break;
			case SK7Packets.SK7_RAW_DATA_CAP_B:
				for(int i=0;i<12;i++) {
					data.cap_sk7[1][i] = rawpacket[3+i] & 0x00ff;
				}
				break;
			case SK7Packets.SK7_RAW_DATA_CAP_C:
				for(int i=0;i<12;i++) {
					data.cap_sk7[2][i] = rawpacket[3+i] & 0x00ff;
				}
				break;
			case SK7Packets.SK7_RAW_DATA_ANALOG0:
				data.ana[0] = fixBytes(rawpacket[3], rawpacket[4]);
				if(has_seq) data.internal_timestamps[ShakeConstants.SHAKE_SENSOR_ANA0] = 0xFF & rawpacket[5];
				break;
			case SK7Packets.SK7_RAW_DATA_ANALOG1:
				data.ana[1] = fixBytes(rawpacket[3], rawpacket[4]);
				if(has_seq) data.internal_timestamps[ShakeConstants.SHAKE_SENSOR_ANA1] = 0xFF & rawpacket[5];
				break;
			case SK7Packets.SK7_RAW_DATA_EVENT:
				if(callback != null) {
					int t = -1;
					switch(rawpacket[3]) {
						case 1:
							t = ShakeEvents.SHAKE_NAV_NORMAL;
							break;
						case 2:
							t = ShakeEvents.SHAKE_NAV_UP;
							break;
						case 3:
							t = ShakeEvents.SHAKE_NAV_DOWN;
							break;
						case 4:
							t = ShakeEvents.SHAKE_NAV_CENTRE;
							break;
						default:
							t = ShakeEvents.SK7_CS0_UPPER + (rawpacket[3] - SK7Packets.SK7_DATA_CU0);
							break;
					}
					devpriv.lastevent = t;
					callback.ShakeEvent(t);
				}
				break;
			case SK7Packets.SK7_RAW_DATA_SHAKING:
				data.shaking_peak_accel = fixBytes(rawpacket[3], rawpacket[4]);
				data.shaking_direction = fixBytes(rawpacket[5], rawpacket[6]);
				data.shaking_timestamp = fixBytes(rawpacket[7], rawpacket[8]);
				if(callback != null) {
					devpriv.lastevent = ShakeEvents.SHAKE_SHAKING_EVENT;
					callback.ShakeEvent(devpriv.lastevent);
				}
				break;
			case SK7Packets.SK7_RAW_DATA_RPH:
				data.rph[0] = fixBytes(rawpacket[3], rawpacket[4]);
				data.rph[1] = fixBytes(rawpacket[5], rawpacket[6]);
				data.rph[2] = fixBytes(rawpacket[7], rawpacket[8]);
				data.heading = data.rph[2];
				if(has_seq) data.internal_timestamps[ShakeConstants.SHAKE_SENSOR_HEADING] = 0xFF & rawpacket[9];
				break;
			case SK7Packets.SK7_RAW_DATA_RPH_QUATERNION:
				data.rphq[0] = fixBytes(rawpacket[3], rawpacket[4]);
				data.rphq[1] = fixBytes(rawpacket[5], rawpacket[6]);
				data.rphq[2] = fixBytes(rawpacket[7], rawpacket[8]);
				data.rphq[3] = fixBytes(rawpacket[9], rawpacket[10]);
				if(has_seq) data.internal_timestamps[ShakeConstants.SHAKE_SENSOR_HEADING] = 0xFF & rawpacket[11];
			default:
				return -1;
		}
		return 1;
	}

	@Override
	public int classify_packet_header(byte[] packetbuf, int header_length, boolean ascii_packet) {
		int type = ShakeConstants.SHAKE_BAD_PACKET, i;
		if(ascii_packet && (header_length != SK7Packets.SK7_HEADER_LEN))
			return type;
		if(!ascii_packet && (header_length != SK7Packets.SK7_RAW_HEADER_LEN || packetbuf[0] != 0x7F || packetbuf[1] != 0x7F))
			return type;
		
		if(ascii_packet)
			i = 0;
		else
			i = SK7Packets.SK7_RAW_DATA_ACC;
		
		for(;i<SK7Packets.SK7_NUM_PACKET_TYPES;i++) {
			if(ascii_packet) {
				if(ShakeUtils.memcmp(SK7Packets.sk7_packet_headers[i].getBytes(), packetbuf, 0, SK7Packets.SK7_HEADER_LEN)) { 
					type = i;
					break;
				}
				if(i >= SK7Packets.SK7_STARTUP_INFO)
					return ShakeConstants.SHAKE_BAD_PACKET;
			} else {
				if(packetbuf[2] == SK7Packets.sk7_raw_packet_headers[i - SK7Packets.SK7_RAW_DATA_ACC]) {
					type = i;
					break;
				}
			}
		}
		
		return type;
	}

	@Override
	public int read_device_info() {
		byte[][] buf = new byte[7][200];
		int linecount = 0, len;
		
		for(linecount=0;linecount<7;linecount++) {
			len = devpriv.read_info_line(buf[linecount], 200);
			if(len == -1)
				return ShakeConstants.SHAKE_ERROR;
			
			if(linecount == SK7Parsing.SK7_FIRMWARE_REV) {
				int plen = 0;
				byte[] ptr = buf[linecount];
				while((ptr[plen] != 0xA && ptr[plen] != 0xD) && (ptr[plen] < '0' || ptr[plen] > '9')) {
					plen++;
				}
				try {
					devpriv.fwrev = Float.parseFloat(new String(ptr).substring(plen, plen+4));
                } catch (NumberFormatException nfe) {}
			} else if(linecount == SK7Parsing.SK7_HARDWARE_REV) {
				int plen = 0;
				byte[] ptr = buf[linecount];
				while((ptr[plen] != 0xA && ptr[plen] != 0xD) && (ptr[plen] < '0' || ptr[plen] > '9')) {
					plen++;
				}
				devpriv.hwrev = Float.parseFloat(new String(ptr).substring(plen, plen+4));
			} else if(linecount == SK7Parsing.SK7_SERIAL_NUMBER) {
				byte[] ptr = buf[linecount];
				int spacecount = 0, plen = 0;
				while(spacecount < 2) {
					if(ptr[plen] == ' ') 
						spacecount++;
					plen++;
				}
				
				devpriv.serial = new String(ptr).substring(plen);
				devpriv.serial = devpriv.serial.substring(0, devpriv.serial.indexOf('\r'));
			} else if(linecount >= SK7Parsing.SK7_SLOT0 && linecount <= SK7Parsing.SK7_SLOT3) {
				// TODO
//				int i;
//				String module = new String(buf[linecount]);
//				module = module.substring(0, module.lastIndexOf('\r'));
//				for(i=ShakeConstants.SK6_MODULE_NONE;i<ShakeConstants.SK6_MODULE_LAST;i++) {
//					if(module.indexOf(SK6Parsing.SK6_modules[i]) != -1) {
//						devpriv.slot1 = i;
//						break;
//					}
//				}
			} else if(linecount == SK7Parsing.SK7_BLUETOOTH_FIRMWARE) {
				// TODO
			}
		}
		return 1;
	}



}
