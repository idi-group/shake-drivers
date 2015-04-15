package com.ugla.shake;

import com.ugla.shake.ShakeConstants.ShakeDeviceType;
import com.ugla.shake.sk6.SK6Parsing;
import com.ugla.shake.sk7.SK7Parsing;

/* This class defines the public SHAKE API */
public class ShakeDevice {
	private ShakeDevicePrivate dev = null;

	public ShakeDevice(ShakeConstants.ShakeDeviceType devtype, ShakeSerialInterface _port) {
		dev = new ShakeDevicePrivate(this, devtype, _port);
	}

	public boolean connect(String btaddress) {
		if(!dev.openConnection(btaddress))
			return false;
		
		return true;
	}

	public boolean connect(int com_port) {
		if(!dev.openConnection(com_port))
			return false;
		
		return true;
	}
	
	public boolean close() {
		register_event_listener(null);
		return dev.closeConnection();
	}
	
	public float info_firmware_revision() {
		if(dev.fwrev == 0.0)
			info_retrieve();
		
		return dev.fwrev;
	}
	
	public float info_hardware_revision() {
		if(dev.hwrev == 0.0)
			info_retrieve();
		
		return dev.hwrev;
	}
	
	public String info_serial_number() {
		if(dev.hwrev == 0.0)
			info_retrieve();
		
		return dev.serial;
	}

	public float info_bluetooth_firmware_revision() {
		if(dev.bluetoothfwrev == 0.0)
			info_retrieve();
		
		return dev.bluetoothfwrev;
	}
	
	public int info_module_slot1() {
		if(dev.hwrev == 0.0)
			info_retrieve();
		return dev.slot1;
	}
	
	public int info_module_slot2() {
		if(dev.hwrev == 0.0)
			info_retrieve();
		return dev.slot2;
	}
	
	public String info_module_name(int module) {
		int i;
		for(i=ShakeConstants.SK6_MODULE_NONE;i<ShakeConstants.SK6_MODULE_LAST;i++) {
			if(module == i)
				return SK6Parsing.SK6_modules[i];
		}
		for(i=ShakeConstants.SK7_MODULE_NONE;i<ShakeConstants.SK7_MODULE_LAST;i++) {
			if(module == i)
				return SK7Parsing.SK7_modules[i];
		}
		return "unknown module";
	}
	
	public boolean info_retrieve() {
		boolean ret = write_data_request(0x01);
		if(ret) {
			int count = 0;
			while(count++ < 1000 && dev.fwrev == 0.0) {
				try { Thread.sleep(1); } catch (InterruptedException ie) {}
			}
		}
		return ret;
	}
	
	public int sk6_factory_reset(int repeat) {
		int i, repeat_count = 1;

		if(repeat > 1)
			repeat_count = repeat;

		wait_for_acks(false);

		for(i=0;i<repeat_count;i++) {
			write(0x0000, 0xFB);	// Pwr Ctrl1
			write(0x0001, 0xFB);	// Pwr Ctrl2
			write(0x0002, 0x08);	// ODF
			write(0x0003, 0x00);	// Comms
			write(0x0004, 0x00);	// Acc flags
			write(0x0005, 0x00);	// Acc flags
			write(0x0006, 0x00);	// Data cal mod
			write(0x0007, 0x00);	// X-Axes en
			write(0x0008, 0x00);	// ACC
			write(0x0009, 0x00);	// GYRO
			write(0x000a, 0x00);	// MAG
			write(0x000b, 0x00);	// CH
			write(0x000c, 0x00);	// CS0
			write(0x000d, 0x00);	// CS1
			write(0x000e, 0x00);	// AIN0
			write(0x000f, 0x00);	// AIN1
			write(0x0010, 0x02);	// Gyro OS TC enable
			write(0x0011, 0x00);	// TC unused
			write(0x0012, 0x00);	// Streaming
			write(0x0013, 0x00);	// Streaming
			write(0x0014, 0x01);	// A Filt
			write(0x0015, 0x01);	// G Filt
			write(0x0016, 0x01);	// M Filt
			write(0x0018, 0x01);	// CS0 Filt
			write(0x0019, 0x01);	// CS1 Filt
			write(0x001a, 0x01);	// AIN0 Filt
			write(0x001b, 0x01);	// AIN1 Filt
			write(0x001e, 0x00);	// AO1 off
			write(0x001f, 0x00);	// AO1 off
			write(0x0026, 0x00);	// E0x off
			write(0x0030, 0x00);	// shake detect off
			write(0x003a, 0x00);	// H01 off
			write(0x003c, 0x00);	// R01 off

			int sample1[] = { 0xFF, 0x03, 0x00, 0x30, 0xFF, 0x03, 0x01, 0x30 };
			sk6_upload_vib_sample(0x01, sample1, 4);

			int sample2[] = { 0xFF, 0x03, 0x00, 0x30, 0xFF, 0x03, 0x01, 0x30, 0xFF, 0x03, 0x00, 0x30, 0xFF, 0x03, 0x01, 0x30, 0xFF, 0x03, 0x00, 0x30, 0xFF, 0x03, 0x01, 0x30 };
			sk6_upload_vib_sample(0x01, sample2, 12);

			int sample3[] = { 0xFF, 0x03, 0x00, 0x30, 0xFF, 0x03, 0x01, 0x30, 0xFF, 0x03, 0x00, 0x30, 0xFF, 0x03, 0x01, 0x30, 0xFF, 0x03, 0x00, 0x30, 0xFF, 0x03, 0x01, 0x30 };
			sk6_upload_vib_sample(0x01, sample3, 12);

			int sample4[] = { 0xFF, 0x03, 0x00, 0x30, 0xFF, 0x03, 0x01, 0x30 };
			sk6_upload_vib_sample(0x01, sample4, 4);
		}

		return ShakeConstants.SHAKE_SUCCESS;
	}
	
	public void wait_for_acks(boolean wait_on) {
		dev.wait_for_acks = wait_on;
	}
	
	public void register_event_listener(ShakeListener sl) {
		dev.setEventListener(sl);
	}
	
	
	// TODO EXP_ FUNCTIONS 
	
	
	public int accx() {
		dev.shake.data.timestamps[ShakeConstants.SHAKE_SENSOR_ACC] = dev.shake.data.internal_timestamps[ShakeConstants.SHAKE_SENSOR_ACC]; 
		return dev.shake.data.acc[0];
	}
	
	public int accy() {
		dev.shake.data.timestamps[ShakeConstants.SHAKE_SENSOR_ACC] = dev.shake.data.internal_timestamps[ShakeConstants.SHAKE_SENSOR_ACC]; 
		return dev.shake.data.acc[1];
	}
	
	public int accz() {
		dev.shake.data.timestamps[ShakeConstants.SHAKE_SENSOR_ACC] = dev.shake.data.internal_timestamps[ShakeConstants.SHAKE_SENSOR_ACC]; 
		return dev.shake.data.acc[2];
	}
	
	public int[] acc() {
		dev.shake.data.timestamps[ShakeConstants.SHAKE_SENSOR_ACC] = dev.shake.data.internal_timestamps[ShakeConstants.SHAKE_SENSOR_ACC]; 
		return dev.shake.data.acc;
	}
	
	public int gyrox() {
		dev.shake.data.timestamps[ShakeConstants.SHAKE_SENSOR_GYRO] = dev.shake.data.internal_timestamps[ShakeConstants.SHAKE_SENSOR_GYRO]; 
		return dev.shake.data.gyro[0];
	}
	
	public int gyroy() {
		dev.shake.data.timestamps[ShakeConstants.SHAKE_SENSOR_GYRO] = dev.shake.data.internal_timestamps[ShakeConstants.SHAKE_SENSOR_GYRO];
		return dev.shake.data.gyro[1];
	}
	
	public int gyroz() {
		dev.shake.data.timestamps[ShakeConstants.SHAKE_SENSOR_GYRO] = dev.shake.data.internal_timestamps[ShakeConstants.SHAKE_SENSOR_GYRO];
		return dev.shake.data.gyro[2];
	}
	
	public int[] gyro() {
		dev.shake.data.timestamps[ShakeConstants.SHAKE_SENSOR_GYRO] = dev.shake.data.internal_timestamps[ShakeConstants.SHAKE_SENSOR_GYRO];
		return dev.shake.data.gyro;
	}
	
	public int magx() {
		dev.shake.data.timestamps[ShakeConstants.SHAKE_SENSOR_MAG] = dev.shake.data.internal_timestamps[ShakeConstants.SHAKE_SENSOR_MAG];
		return dev.shake.data.mag[0];
	}
	
	public int magy() {
		dev.shake.data.timestamps[ShakeConstants.SHAKE_SENSOR_MAG] = dev.shake.data.internal_timestamps[ShakeConstants.SHAKE_SENSOR_MAG];
		return dev.shake.data.mag[1];
	}
	
	public int magz() {
		dev.shake.data.timestamps[ShakeConstants.SHAKE_SENSOR_MAG] = dev.shake.data.internal_timestamps[ShakeConstants.SHAKE_SENSOR_MAG];
		return dev.shake.data.mag[2];
	}
	
	public int[] mag() {
		dev.shake.data.timestamps[ShakeConstants.SHAKE_SENSOR_MAG] = dev.shake.data.internal_timestamps[ShakeConstants.SHAKE_SENSOR_MAG];
		return dev.shake.data.mag;
	}
	
	public int heading() {
		dev.shake.data.timestamps[ShakeConstants.SHAKE_SENSOR_HEADING] = dev.shake.data.internal_timestamps[ShakeConstants.SHAKE_SENSOR_HEADING];
		return dev.shake.data.heading;
	}
	
	public int[] sk7_roll_pitch_heading() {
		return dev.shake.data.rph;
	}
	
	public int[] sk7_roll_pitch_heading_quaternions() {
		return dev.shake.data.rphq;
	}
	
	public int sk6_cap0() {
		dev.shake.data.timestamps[ShakeConstants.SHAKE_SENSOR_SK6_CAP0] = dev.shake.data.internal_timestamps[ShakeConstants.SHAKE_SENSOR_SK6_CAP0];
		return dev.shake.data.cap_sk6[0];
	}
	
	public int sk6_cap1() {
		dev.shake.data.timestamps[ShakeConstants.SHAKE_SENSOR_SK6_CAP1] = dev.shake.data.internal_timestamps[ShakeConstants.SHAKE_SENSOR_SK6_CAP1];
		return dev.shake.data.cap_sk6[1];
	}
	
	public int[] sk6_cap() {
		dev.shake.data.timestamps[ShakeConstants.SHAKE_SENSOR_SK6_CAP0] = dev.shake.data.internal_timestamps[ShakeConstants.SHAKE_SENSOR_SK6_CAP0];
		dev.shake.data.timestamps[ShakeConstants.SHAKE_SENSOR_SK6_CAP1] = dev.shake.data.internal_timestamps[ShakeConstants.SHAKE_SENSOR_SK6_CAP1];
		return dev.shake.data.cap_sk6;
	}
	
	public int[] sk7_cap() {
		dev.shake.data.timestamps[ShakeConstants.SHAKE_SENSOR_SK7_CAP] = dev.shake.data.internal_timestamps[ShakeConstants.SHAKE_SENSOR_SK7_CAP];
		return dev.shake.data.cap_sk7[0];
	}
	
	public int[] sk7_cap_ext(int blocks) {
		switch(blocks) {
			case 0:
				return dev.shake.data.cap_sk7[1];
			case 1:
				return dev.shake.data.cap_sk7[2];
			case 2:
				int[] both = new int[24];
				System.arraycopy(dev.shake.data.cap_sk7[1], 0, both, 0, 12);
				System.arraycopy(dev.shake.data.cap_sk7[2], 0, both, 12, 12);
				return both;
		}
		return null;
	}
	
	public int analog0() {
		dev.shake.data.timestamps[ShakeConstants.SHAKE_SENSOR_ANA0] = dev.shake.data.internal_timestamps[ShakeConstants.SHAKE_SENSOR_ANA0];
		return dev.shake.data.ana[0];
	}
	
	public int analog1() {
		dev.shake.data.timestamps[ShakeConstants.SHAKE_SENSOR_ANA1] = dev.shake.data.internal_timestamps[ShakeConstants.SHAKE_SENSOR_ANA1];
		return dev.shake.data.ana[1];
	}
	
	public int[] analog() {
		dev.shake.data.timestamps[ShakeConstants.SHAKE_SENSOR_ANA0] = dev.shake.data.internal_timestamps[ShakeConstants.SHAKE_SENSOR_ANA0];
		dev.shake.data.timestamps[ShakeConstants.SHAKE_SENSOR_ANA1] = dev.shake.data.internal_timestamps[ShakeConstants.SHAKE_SENSOR_ANA1];
		return dev.shake.data.ana;
	}
	
	public int shaking_peakaccel() {
		return dev.shake.data.shaking_peak_accel;
	}
	
	public int shaking_direction() {
		return dev.shake.data.shaking_direction;
	}
	
	public int shaking_timestamp() {
		return dev.shake.data.shaking_timestamp;
	}
	
	public int heart_rate() {
		return dev.shake.data.hr_bpm;
	}
	
	public int data_timestamp(int sensor) {
		return dev.shake.data.timestamps[sensor];
	}
	
	public void setDebugOut(ShakeDebug dbg) {
		dev.dbg = dbg;
	}
	
	// TODO RFID functions

	// TODO LOGGING API
	
	// TODO shaking API
	
	// TODO various utility functions (capacitive sensing etc)

	public int read_power_state() {
		return dev.readRegister(0);
	}

	public boolean write_power_state(int value) {
		return dev.writeRegister(0, value);
	}

	public int read_power_state_2() {
		return dev.readRegister(1);
	}

	public boolean write_power_state_2(int value) {
		return dev.writeRegister(1, value);
	}

	public int read_data_format() {
		return dev.readRegister(2);
	}

	public boolean write_data_format(int value) {
		return dev.writeRegister(2, value);
	}

	public int read_sample_rate(int sensor) {
		return dev.readRegister(8 + sensor);
	}

	public boolean write_sample_rate(int sensor, int  value) {
		return dev.writeRegister(8 + sensor, value);
	}
	
	public boolean write_packet_request(int value) {
		return dev.writeRegister(0x0100, value);
	}

	public boolean write_data_request(int value) {
		return dev.writeRegister(0x0101, value);
	}
	
	public boolean playvib(int channel, int profile) {
		if(profile < 1)
			return false;
		
		if(dev.shake.device_type == ShakeDeviceType.SHAKE_SK6.ordinal()) {
			int addr = ShakeConstants.VO_REG_VIB_MAIN + channel;
			return write(addr, profile);
		} else {
			String buf = null;
			switch(channel) {
				case ShakeConstants.SHAKE_VIB_MAIN:
					buf = String.format("vm%02X", profile);
					break;
				case ShakeConstants.SHAKE_VIB_LEFT:
					buf = String.format("vl%02X", profile);
					break;
				case ShakeConstants.SHAKE_VIB_RIGHT:
					buf = String.format("vr%02X", profile);
					break;
				case ShakeConstants.SHAKE_VIB_FORCEREACTOR:
					buf = String.format("vf%02X", profile);
					break;
				case ShakeConstants.SHAKE_VIB_ACTUATOR:
					buf = String.format("vd%02X", profile);
					break;
			}
			if(buf == null)
				return false;
			
			dev.port.writeBytes(buf.getBytes(), buf.length(), 0);
		}
		
		return true;
	}
	
	public boolean sk6_playvib_continuous(int channel, int amplitude, int time) {
		if(channel != ShakeConstants.SHAKE_VIB_LEFT || channel != ShakeConstants.SHAKE_VIB_RIGHT)
			return false;
		
		if(amplitude != 0 && amplitude != 33 && amplitude != 66 && amplitude != 100)
			return false;
		
		if (time < 0 || time > 64)
			return false;
		
		int vibbyte = 0;
		switch(amplitude) {
			case 0:
				vibbyte = 0;
				break;
			case 33:
				vibbyte = 0x40;
				break;
			case 66:
				vibbyte = 0x80;
				break;
			case 100:
				vibbyte = 0xC0;
				break;
		}
		
		vibbyte += time;
		
		int vibaddr = ShakeConstants.VO_REG_VIB_LEFT_CONTINUOUS;
		if(channel == ShakeConstants.SHAKE_VIB_RIGHT)
			vibaddr = ShakeConstants.VO_REG_VIB_RIGHT_CONTINUOUS;
		
		return write(vibaddr, vibbyte);
	}
	
	public boolean sk6_upload_vib_sample(int profile, int[] sample, int sample_length) {
		return upload_vib_sample_extended(profile, sample, sample_length, 0, 0, 0);
	}
	
	public boolean upload_vib_sample_extended(int profile, int[] sample, int sample_length, int mode, int freq, int duty) {
		
		if(profile < 1) 
			return false;
		
		if(sample_length <= 0 || sample_length > ShakeConstants.SHAKE_VIB_SAMPLE_MAX_LENGTH)
			return false;
		int cursample = 0;
		int timeout = 250;
		
		String packetbuf = "";
		
		packetbuf = String.format("$VIB,%02X,%02X,%02X,%02X,", profile, mode, freq, duty);

		for(int i=0;i<sample_length-1;i++) {
			packetbuf += String.format("%02X,%02X,", sample[cursample], sample[cursample+1]);
			cursample += 2;
		}
		
		packetbuf += String.format("%02X,%02X~", sample[cursample], sample[cursample+1]);
		
		if(dev.port.writeBytes(packetbuf.getBytes(), packetbuf.length(), 0) != packetbuf.length())
			return false;
		
		if(!dev.wait_for_acks)
			return true;
		
		dev.waiting_for_ack_signal = true;
		dev.waiting_for_ack = true;
		
		while(dev.waiting_for_ack_signal) {
			try { Thread.sleep(1); } catch (InterruptedException ie) {}
			--timeout;
			if(timeout == 0)
				break;
		}
		
		dev.waiting_for_ack = false;
		
		if(!dev.lastack)
			return false;
		return true;
	}
	
	// TODO battery/power functions
	
	// TODO SK7 heading feedback
	
	public boolean sk7_configure_roll_pitch_heading(int value) {
		return write(ShakeConstants.NV_REG_RPH_CONFIG, value);
	}
	
	public int read(int reg_address) {
		return dev.readRegister(reg_address);
	}
	
	public boolean write(int reg, int val) {
		return dev.writeRegister(reg, val);
	}
	
	public boolean calibration_c() {
		// This write c to the output to go to the next step of calibration
		return dev.write_c();
		
	}
	
	
	

	
	public boolean uploadVibrationSample(int address, int[] samples) {
		if(samples.length % 2 != 0)
			return false;
		return dev.uploadVibrationSample(address, samples);
	}
	

	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	

	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
}
