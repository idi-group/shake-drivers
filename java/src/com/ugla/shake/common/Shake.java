package com.ugla.shake.common;

import com.ugla.shake.ShakeDevice;
import com.ugla.shake.ShakeDevicePrivate;
import com.ugla.shake.ShakeListener;

/** 	Defines common functionality and interface for the SK6 and SK7 */
public abstract class Shake {
	public Shake(ShakeDevice sd, ShakeDevicePrivate sdp) {
		dev = sd;
		devpriv = sdp;
		for(int i=0;i<8;i++)
			data.timestamps[i] = 0;
	}
	
	public int device_type;
	public SkSensorData data = new SkSensorData();
	public ShakeDevice dev = null;
	public ShakeDevicePrivate devpriv = null;
	public ShakeListener callback = null;

	abstract public int parse_ascii_packet(int packet_type, byte[] packetbuf, int packetlen, boolean playback);
	abstract public int read_ascii_packet(int packet_type, byte[] packetbuf);
	abstract public int parse_raw_packet(int packet_type, byte[] packetbuf, int packetlen, boolean has_seq);
	abstract public int read_raw_packet(int packet_type, byte[] packetbuf);
	abstract public int get_next_packet(byte[] packetbuf, int bufsize);
	abstract public int parse_packet(byte[] packetbuf, int packet_type);
	abstract public boolean is_ascii_packet(int packet_type);
	abstract public int parse_ack_packet(byte[] packetbuf);
	abstract public int extract_ascii_packet(int packet_type, byte[] rawpacket, boolean playback);
	abstract public int extract_raw_packet(int packet_type, byte[] rawpacket, boolean has_seq);
	abstract public int classify_packet_header(byte[] packetbuf, int header_length, boolean ascii_packet);
	abstract public int read_device_info();
}
