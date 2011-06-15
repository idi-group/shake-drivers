package SHAKE;

public class shake_device {

	static {
		try {
			System.loadLibrary("shake_java");	
		} catch (UnsatisfiedLinkError ule) {
			System.out.println("Failed to load shake_java library!");
			System.exit(-1);
		}
	}

	// stores pointer to C shake_device object
	private long dev;
	
	private static int SHAKE_ERROR = -1;
	private static int SHAKE_SUCCESS = 1;

	public static final int SHAKE_SK6 = 0;
	public static final int SHAKE_SK7 = 1;

	// the following are the possible values for the SHAKE callback function parameter
	public static final int SHAKE_NAV_UP = 1;
	public static final int SHAKE_NAV_DOWN = 2;
	public static final int SHAKE_NAV_CENTRE = 3;
	public static final int SHAKE_NAV_NORMAL = 4;
	public static final int SHAKE_CS0_UPPER = 5;
	public static final int SHAKE_CS0_LOWER = 6;
	public static final int SHAKE_CS1_UPPER = 7;
	public static final int SHAKE_CS1_LOWER = 8;
	public static final int SHAKE_PLAYBACK_COMPLETE = 9;
	public static final int SHAKE_SHAKING_EVENT = 10;
	public static final int SHAKE_HEART_RATE_EVENT = 11;
	public static final int SHAKE_RFID_TID_EVENT = 12;

	public static final int SHAKE_MODULE_NONE = 0;
	public static final int SHAKE_MODULE_GYRO1 = 1;		
	public static final int SHAKE_MODULE_GYRO2 = 2;
	public static final int SHAKE_MODULE_RFID = 3;
	public static final int SHAKE_MODULE_AUDIO = 4;
	public static final int SHAKE_MODULE_EXTVIB = 5;
	public static final int SHAKE_MODULE_USB = 6;
	public static final int SHAKE_MODULE_EXPANSION = 7;

	public static final int SHAKE_POWER_CAP = 0x01;
	public static final int SHAKE_POWER_ACC = 0x02;
	public static final int SHAKE_POWER_GYRO = 0x04;
	public static final int SHAKE_POWER_VIB = 0x08;
	public static final int SHAKE_POWER_MAG = 0x10;
	public static final int SHAKE_POWER_TEMP = 0x20;
	public static final int SHAKE_POWER_ANALOG = 0x40;
	public static final int SHAKE_POWER_NAV = 0x80;

	public static final int SHAKE_POWER_CS0_INC = 0x01;
	public static final int SHAKE_POWER_CS0_DEC = 0x02;
	public static final int SHAKE_POWER_CS1_INC = 0x04;
	public static final int SHAKE_POWER_CS1_DEC = 0x08;
	public static final int SHAKE_POWER_CS_EVENTS = 0x10;

	public static final int SHAKE_SENSOR_ACC = 0;
	public static final int SHAKE_SENSOR_GYRO = 1;
	public static final int SHAKE_SENSOR_MAG = 2;
	public static final int SHAKE_SENSOR_HEADING = 3;
	public static final int SHAKE_SENSOR_CAP0 = 4;
	public static final int SHAKE_SENSOR_CAP1 = 5;
	public static final int SHAKE_SENSOR_ANA0 = 6;
	public static final int SHAKE_SENSOR_ANA1 = 7;

	public static final int SHAKE_ACC_MAX_RATE = 0xFF;
	public static final int SHAKE_GYRO_MAX_RATE = 0xFF;
	public static final int SHAKE_MAG_MAX_RATE = 0xFF;
	public static final int SHAKE_HEADING_MAX_RATE = 0xFF;
	public static final int SHAKE_CAP0_MAX_RATE = 0xFF;
	public static final int SHAKE_CAP1_MAX_RATE = 0xFF;
	public static final int SHAKE_ANA0_MAX_RATE = 0xFF;
	public static final int SHAKE_ANA1_MAX_RATE = 0xFF;

	public static final int SHAKE_REQ_ACC = 0x01;
	public static final int SHAKE_REQ_GYRO = 0x02;
	public static final int SHAKE_REQ_MAG = 0x04;
	public static final int SHAKE_REQ_HEADING = 0x08;
	public static final int SHAKE_REQ_CAP0 = 0x10;
	public static final int SHAKE_REQ_CAP1 = 0x20;
	public static final int SHAKE_REQ_ANA0 = 0x40;
	public static final int SHAKE_REQ_ANA1 = 0x80;

	public static final int SHAKE_VIB_MAIN = 0;
	public static final int SHAKE_VIB_LEFT = 1;
	public static final int SHAKE_VIB_RIGHT = 2;

	public static final int SHAKE_VIB_SAMPLE_MAX_LENGTH = 32;
	public static final int SHAKE_VIB_PROFILE_MIN = 0x01;
	public static final int SHAKE_VIB_PROFILE_MAX = 0xFF;

	public static final int SHAKE_CALIB_MAG = 0x01;
	public static final int SHAKE_CALIB_ACC = 0x02;
	public static final int SHAKE_CALIB_GYRO = 0x03;
	public static final int SHAKE_CALIB_TEMP = 0x04;
	public static final int SHAKE_CALIB_BATTERY = 0x05;
	public static final int SHAKE_CALIB_CAP = 0x06;
	public static final int SHAKE_CALIB_ANALONG = 0x07;

	public static final int SHAKE_BYPASS_ACC = 0x01;
	public static final int SHAKE_BYPASS_GYRO = 0x02;
	public static final int SHAKE_BYPASS_MAG = 0x04;
	public static final int SHAKE_BYPASS_CAP0 = 0x10;
	public static final int SHAKE_BYPASS_CAP1 = 0x20;
	public static final int SHAKE_BYPASS_ANA0 = 0x40;
	public static final int SHAKE_BYPASS_ANA1 = 0x80;

	public static final int SHAKE_CX_BYPASS_ACC = 0x01;
	public static final int SHAKE_CX_BYPASS_GYRO = 0x02;
	public static final int SHAKE_CX_BYPASS_MAG = 0x04;

	public static final int SHAKE_STREAM_ACC = 0x01;
	public static final int SHAKE_STREAM_GYRO = 0x02;
	public static final int SHAKE_STREAM_MAG = 0x04;
	public static final int SHAKE_STREAM_HEADING = 0x08;
	public static final int SHAKE_STREAM_CAP0 = 0x10;
	public static final int SHAKE_STREAM_CAP1 = 0x20;
	public static final int SHAKE_STREAM_ANA0 = 0x40;
	public static final int SHAKE_STREAM_ANA1 = 0x80;

	public static final int SHAKE_EXT_POWER = 0x01;
	public static final int SHAKE_BAT_CHARGING = 0x02;
	public static final int SHAKE_BAT_FULL = 0x04;

	public static final int SHAKE_BATTERY_MAX = 0xFF;

	public static final int SHAKE_TEMP_MIN = 0x00;
	public static final int SHAKE_TEMP_MAX = 0x40;

	public static final int SHAKE_LOGGING_PLAY = 0x01;
	public static final int SHAKE_LOGGING_PAUSE = 0x02;
	public static final int SHAKE_LOGGING_STOP = 0x03;
	public static final int SHAKE_LOGGING_RECORD = 0x04;
	public static final int SHAKE_LOGGING_RESET = 0x05;

	public static final int SHAKE_LOGGING_RECORDING = 0x01;
	public static final int SHAKE_LOGGING_PLAYING = 0x02;
	public static final int SHAKE_LOGGING_MEMORYFULL = 0x04;

	public static final int SHAKE_SHAKING_DETECTION = 0x01;
	public static final int SHAKE_SHAKING_DIRECTION_DETECTION = 0x02;
	public static final int SHAKE_SHAKING_VIBRATION = 0x04;

	public static final int SHAKE_AUDIO_SAMPLING = 0x01;
	public static final int SHAKE_AUDIO_PLAYBACK = 0x02;
	public static final int SHAKE_AUDIO_EXTERNAL_MIC = 0x04;

	public static final int SHAKE_AUDIO_DATA_LEN = 0x20;
	public static final int SHAKE_AUDIO_SAMPLE_RATE = 7200;
	public static final int SHAKE_AUDIO_SAMPLE_SIZE = 16;

	public static final int SHAKE_MIDI_AMPLITUDE_MUTE = 0x00;	
	public static final int SHAKE_MIDI_AMPLITUDE_MIN = 0x01;
	public static final int SHAKE_MIDI_AMPLITUDE_MAX = 0xFF;

	public static final int SHAKE_MIDI_NOTE_MIN = 0x21;
	public static final int SHAKE_MIDI_NOTE_MAX = 0x69;

	public static final int SHAKE_MIDI_WAVEFORM_SINE = 0;
	public static final int SHAKE_MIDI_WAVEFORM_SQUARE = 1;
	public static final int SHAKE_MIDI_WAVEFORM_TRIANGLE = 2;
	public static final int SHAKE_MIDI_WAVEFORM_SAWTOOTH = 3;

	public static final int SHAKE_AUDIO_PAGE_SIZE = 1056;
	public static final int SHAKE_AUDIO_AMPLITUDE_MIN = 0;
	public static final int SHAKE_AUDIO_AMPLITUDE_MAX = 255;
	public static final int SHAKE_AUDIO_UPLOAD_MIN = 0;
	public static final int SHAKE_AUDIO_UPLOAD_MAX = 7999;

	public shake_device() {
		dev = 0;
		device_type = SHAKE_SK7;
	}

	public shake_device(int dev_type) {
		dev = 0;
		device_type = dev_type;
	}

	public boolean connect(int com_port) {
		dev = shake_init_device(com_port, device_type);
		if(dev == 0)
			return false;
		return true;
	}

	public boolean connect(String bt_addr) {
		// TODO
		return false;
	}

	public boolean connect_rfcomm(long btaddr) {
		dev = shake_init_device_rfcomm(btaddr, device_type);
		if(dev == 0)
			return false;
		return true;
	}

	public boolean connect_usb(String devname) {
		dev = shake_init_device_usb_serial(devname, device_type);
		if(dev == 0)
			return false;
		return true;
	}

	public boolean close() {
		register_event_callback(null);
		if(shake_free_device(dev) == SHAKE_ERROR)
			return false;
		dev = 0;
		return true;
	}

	public float info_firmware_revision() {
		return shake_info_firmware_revision(dev);
	}

	public float info_hardware_revision() {
		return shake_info_hardware_revision(dev);
	}

	public String info_serial_number() {
		return shake_info_serial_number(dev);
	}

	public int info_module_slot1() {
		return shake_info_module_slot1(dev);
	}

	public int info_module_slot2() {
		return shake_info_module_slot2(dev);
	}

	public int info_retrieve() {
		return shake_info_retrieve(dev);
	}

	public String info_module_name(int module) {
		return shake_info_module_name(module);
	}

	public int accx() {
		return shake_accx(dev);
	}

	public int accy() {
		return shake_accy(dev);
	}

	public int accz() {
		return shake_accz(dev);
	}

	public int acc(int[] xyz) {
		return shake_acc(dev, xyz);
	}

	public int gyrx() {
		return shake_gyrx(dev);
	}

	public int gyry() {
		return shake_gyry(dev);
	}

	public int gyrz() {
		return shake_gyrz(dev);
	}

	public int gyr(int[] xyz) {
		return shake_gyr(dev, xyz);
	}

	public int magx() {
		return shake_magx(dev);
	}

	public int magy() {
		return shake_magy(dev);
	}

	public int magz() {
		return shake_magz(dev);
	}

	public int mag(int[] xyz) {
		return shake_mag(dev, xyz);
	}

	public int heading() {
		return shake_heading(dev);
	}

	public int cap0() { 
		return shake_cap0(dev);
	}

	public int cap1() {	
		return shake_cap1(dev);
	}

	public int cap(int[] proxboth) {
		return shake_cap(dev, proxboth);
	}

	public int analog0() {
		return shake_analog0(dev);
	}

	public int analog1() {
		return shake_analog1(dev);
	}

	public int analog(int[] a0a1) {
		return shake_analog(dev, a0a1);
	}

	public int shaking_peakaccel() {
		return shake_shaking_peakaccel(dev);
	}
	
	public int shaking_direction() {
		return shake_shaking_direction(dev);
	}

	public int shaking_timestamp() {
		return shake_shaking_timestamp(dev);
	}

	public int data_timestamp(int sensor) {
		return shake_data_timestamp(dev, sensor);
	}

	public int sk7_roll_pitch_heading(int[] rph) {
		return sk7_roll_pitch_heading(dev, rph);
	}

	public int sk7_roll_pitch_heading_quaternions(int[] rphq) {
		return sk7_roll_pitch_heading_quaternions(dev, rphq);
	}

	public int sk7_configure_roll_pitch_heading(int value) {
		return sk7_configure_roll_pitch_heading(dev, value);
	}

	// Pass this function a reference to an object which has a function called "callback", with the signature:
	// void callback(int)
	// This method of the specified object will be used as the callback from the SHAKE driver. 
	// The parameter will be one of the 4 SHAKE_NAV_ values defined above (indicating movement of the nav switch 
	// on the SHAKE) or one of the SHAKE_CS0/SHAKE_CS1 values also defined above (indicating capacitive switch
	// thresholds have been triggered).
	// To unregister the callback, pass a null reference as the parameter.
	public int register_event_callback(Object o) {
		return shake_register_event_callback(o, dev);
	}

	public int logging_play(char[] filename) {
		return shake_logging_play(dev, filename);
	}

	public int logging_pause() {
		return shake_logging_pause(dev);
	}

	public int logging_stop() {
		return shake_logging_stop(dev);
	}

	public int logging_record() {
		return shake_logging_record(dev);
	}

	public int logging_reset() {
		return shake_logging_reset(dev);
	}

	public int logging_status() {
		return shake_logging_status(dev);
	}

	public int logging_memory_status() {
		return shake_logging_memory_status(dev);
	}

	public int logging_packet_count() {
		return shake_logging_packet_count(dev);
	}

	public int logging_packets_read() {
		return shake_logging_packets_read(dev);
	}

	public int read_power_state() {
		return shake_read_power_state(dev);
	}

	public int write_power_state(int value) {
		return shake_write_power_state(dev, value);
	}

	public int read_power_state_2() {
		return shake_read_power_state_2(dev);
	}

	public int write_power_state_2(int value) {
		return shake_write_power_state_2(dev, value);
	}

	public int read_acc_config() {
		return shake_read_acc_config(dev);
	}

	public int write_acc_config(int  value) {
		return shake_write_acc_config(dev, value);
	}

	public int read_data_format() {
		return shake_read_data_format(dev);
	}

	public int write_data_format(int value) {
		return shake_write_data_format(dev, value);
	}

	public int read_sample_rate(int sensor) {
		return shake_read_sample_rate(dev, sensor);
	}

	public int write_sample_rate(int sensor, int  value) {
		return shake_write_sample_rate(dev, sensor, value);
	}

	public int read_digital_filter(int sensor) {
		return shake_read_digital_filter(dev, sensor);
	}

	public int write_digital_filter(int sensor, int value) {
		return shake_write_digital_filter(dev, sensor, value);
	}

	public int read_calib_bypass() {
		return shake_read_calib_bypass(dev);
	}

	public int write_calib_bypass(int value) {
		return shake_write_calib_bypass(dev, value);
	}

	public int read_cx_bypass() {
		return shake_read_cx_bypass(dev);
	}

	public int write_cx_bypass(int value) {
		return shake_write_cx_bypass(dev, value);
	}

	public int read_temp_compensation() {
		return shake_read_temp_compensation(dev);
	}

	public int write_temp_compensation(int value) {
		return shake_write_temp_compensation(dev, value);
	}

	public int read_packet_streaming() {
		return shake_read_packet_streaming(dev);
	}

	public int write_packet_streaming(int value) {
		return shake_write_packet_streaming(dev, value);
	}

	public int read_audio_config() {
		return shake_read_audio_config(dev);
	}

	public int write_audio_config(int value) {
		return shake_write_audio_config(dev, value);
	}

	public int write_midi_amplitude(int value) {
		return shake_write_midi_amplitude(dev, value);
	}

	public int write_midi_note(int value) {
		return shake_write_midi_note(dev, value);
	}

	public int write_midi_waveform(int value) {
		return shake_write_midi_waveform(dev, value);
	}


	public int sk6_read_cs0_inc() {
		return sk6_read_cs0_inc(dev);
	}

	public int sk6_write_cs0_inc(int value) {
		return sk6_write_cs0_inc(dev, value);
	}

	public int sk6_read_cs0_dec() {
		return sk6_read_cs0_dec(dev);
	}

	public int sk6_write_cs0_dec(int value) {
		return sk6_write_cs0_dec(dev, value);
	}

	public int sk6_read_cs0_inc_profile() {
		return sk6_read_cs0_inc_profile(dev);
	}

	public int sk6_write_cs0_inc_profile(int value) {
		return sk6_write_cs0_inc_profile(dev, value);
	}

	public int sk6_read_cs0_dec_profile() {
		return sk6_read_cs0_dec_profile(dev);
	}

	public int sk6_write_cs0_dec_profile(int value) {
		return sk6_write_cs0_dec_profile(dev, value);
	}

	public int sk6_cs1_inc() {
		return sk6_read_cs1_inc(dev);
	}

	public int sk6_cs1_inc(int value) {
		return sk6_write_cs1_inc(dev, value);
	}

	public int sk6_cs1_dec() {
		return sk6_read_cs1_dec(dev);
	}

	public int sk6_cs1_dec(int value) {
		return sk6_write_cs1_dec(dev, value);
	}

	public int sk6_read_cs1_inc_profile() {
		return sk6_read_cs1_inc_profile(dev);
	}

	public int sk6_cs1_inc_profile(int value) {
		return sk6_write_cs1_inc_profile(dev, value);
	}

	public int sk6_cs1_dec_profile() {
		return sk6_read_cs1_dec_profile(dev);
	}

	public int sk6_cs1_dec_profile(int value) {
		return sk6_write_cs1_dec_profile(dev, value);
	}

	public int sk6_read_cap_thresholds(byte[] values) {
		return sk6_read_cap_thresholds(dev, values);
	}

	public int sk6_cap_thresholds(byte[] values) {
		return sk6_write_cap_thresholds(dev, values);
	}


	public int read_shaking_config() {
		return shake_read_shaking_config(dev);
	}

	public int write_shaking_config(int value) {
		return shake_write_shaking_config(dev, value);
	}

	public int read_shaking_accel_threshold() {
		return shake_read_shaking_accel_threshold(dev);
	}

	public int write_shaking_accel_threshold(int value) {
		return shake_write_shaking_accel_threshold(dev, value);
	}

	public int read_shaking_holdoff_time() {
		return shake_read_shaking_holdoff_time(dev);
	}

	public int write_shaking_holdoff_time(int value) {
		return shake_write_shaking_holdoff_time(dev, value);
	}

	public int read_shaking_vibration_profile() {
		return shake_read_shaking_vibration_profile(dev);
	}

	public int write_shaking_vibration_profile(int value) {
		return shake_write_shaking_vibration_profile(dev, value);
	}

	public int read_shaking_hpf_constant() {
		return shake_read_shaking_hpf_constant(dev);
	}

	public int write_shaking_hpf_constant(int value) {
		return shake_write_shaking_hpf_constant(dev, value);
	}
	
	public int read_shaking_lpf_constant() {
		return shake_read_shaking_lpf_constant(dev);
	}

	public int write_shaking_lpf_constant(int value) {
		return shake_write_shaking_lpf_constant(dev, value);
	}

	public int reset_shaking_detection() {
		return shake_reset_shaking_detection(dev);
	}

	public int write_packet_request(int value) {
		return shake_write_packet_request(dev, value);
	}

	public int write_data_request(int value) {
		return shake_write_data_request(dev, value);
	}

	public int playvib(int channel, int profile) {
		return shake_playvib(dev, channel, (byte)profile);
	}

	public int sk6_upload_vib_sample(int profile, int[] samples) {
		return sk6_upload_vib_sample(dev, (byte)profile, samples);
	}

	public int read_battery_level() {
		return shake_read_battery_level(dev);
	}

	public int read_power_status() {
		return shake_read_power_status(dev);
	}

	public float read_temperature() {
		return shake_read_temperature(dev);
	}

	public int read(int addr) {
		return shake_read(dev, addr);
	}

	public int write(int addr, int value) {
		return shake_write(dev, addr, value);
	}

	public void callback(int event) {
		System.out.println(dev + " | " + event);	
	}

	public static void main(String[] args) {
		shake_device sd = new shake_device();
		sd.connect(2);
		sd.write_audio_config(0);
		sd.write_power_state((byte)255);
		sd.register_event_callback(sd);

		System.out.println("Serial: " + sd.info_serial_number());
		System.out.println("Firmware: " + sd.info_firmware_revision());
		System.out.println("Hardware: " + sd.info_hardware_revision());

		try {
			Thread.sleep(3000);	
		} catch (InterruptedException ie) {}
		
		sd.register_event_callback(null);
		System.out.println("Closing dev");
		boolean ret = sd.close();
		System.out.println("ret = " + ret);
		//System.exit(1);
	}

	private int device_type;

	// Startup/shutdown functions
	private static native long shake_init_device(int com_port, int dev_type);
	private static native long shake_init_device_rfcomm(long btaddr, int dev_type);
	private static native long shake_init_device_usb_serial(String devname, int dev_type);
	private static native int shake_free_device(long dev);

	// Information functions
	private static native float shake_info_firmware_revision(long dev);
	private static native float shake_info_hardware_revision(long dev);
	private static native String shake_info_serial_number(long dev);
	private static native int shake_info_module_slot1(long dev);
	private static native int shake_info_module_slot2(long dev);
	private static native int shake_info_retrieve(long dev);
	private static native String shake_info_module_name(int module);

	// Data access functions
	private static native int shake_accx(long dev);
	private static native int shake_accy(long dev);
	private static native int shake_accz(long dev);
	private static native int shake_acc(long dev, int[] xyz);

	private static native int shake_gyrx(long dev);
	private static native int shake_gyry(long dev);
	private static native int shake_gyrz(long dev);
	private static native int shake_gyr(long dev, int[] xyz);

	private static native int shake_magx(long dev);
	private static native int shake_magy(long dev);
	private static native int shake_magz(long dev);
	private static native int shake_mag(long dev, int[] xyz);

	private static native int shake_heading(long dev);

	private static native int shake_cap0(long dev);
	private static native int shake_cap1(long dev);
	private static native int shake_cap(long dev, int[] proxboth);

	private static native int shake_analog0(long dev);
	private static native int shake_analog1(long dev);
	private static native int shake_analog(long dev, int[] a0a1);

	private static native int shake_shaking_peakaccel(long dev);
	private static native int shake_shaking_direction(long dev);
	private static native int shake_shaking_timestamp(long dev);

	private static native int sk7_roll_pitch_heading(long dev, int[] rph);
	private static native int sk7_roll_pitch_heading_quaternions(long dev, int[] rphq);
	private static native int sk7_configure_roll_pitch_heading(long dev, int value);

	private static native int shake_register_event_callback(Object obj, long dev);

	private static native int shake_data_timestamp(long dev, int sensor);

	// Data logging functions
	private static native int shake_logging_play(long dev, char[] filename);
	private static native int shake_logging_pause(long dev);
	private static native int shake_logging_stop(long dev);
	private static native int shake_logging_record(long dev);
	private static native int shake_logging_reset(long dev);

	private static native int shake_logging_status(long dev);
	private static native int shake_logging_memory_status(long dev);
	private static native int shake_logging_packet_count(long dev);
	private static native int shake_logging_packets_read(long dev);

	// Register access functions
	// Note that these prototypes are different from the C versions because here the return
	// value is used for both positive and negative returns, whereas in C the return value
	// only indicates success/failure. Java support for output parameters is a bit lacking,
	// especially for primitive types, so this is just a simple way to get around that. By
	// returning -1 from the stub method on error, that provides a clear indication that
	// the value indicates an error (because successful returns are int values, 0-255)
	
	private static native int shake_read_power_state(long dev);
	private static native int shake_write_power_state(long dev, int value);

	private static native int shake_read_power_state_2(long dev);
	private static native int shake_write_power_state_2(long dev, int value);

	private static native int shake_read_acc_config(long dev);
	private static native int shake_write_acc_config(long dev, int value);

	private static native int shake_read_data_format(long dev);
	private static native int shake_write_data_format(long dev, int value);

	private static native int shake_read_sample_rate(long dev, int sensor);
	private static native int shake_write_sample_rate(long dev, int sensor, int value);

	private static native int shake_read_digital_filter(long dev, int sensor);
	private static native int shake_write_digital_filter(long dev, int sensor, int value);

	private static native int shake_read_calib_bypass(long dev);
	private static native int shake_write_calib_bypass(long dev, int value);

	private static native int shake_read_cx_bypass(long dev);
	private static native int shake_write_cx_bypass(long dev, int value);

	private static native int shake_read_temp_compensation(long dev);
	private static native int shake_write_temp_compensation(long dev, int value);

	private static native int shake_read_packet_streaming(long dev);
	private static native int shake_write_packet_streaming(long dev, int value);

	private static native int shake_read_audio_config(long dev);
	private static native int shake_write_audio_config(long dev, int value);

	private static native int shake_write_midi_amplitude(long dev, int value);
	private static native int shake_write_midi_note(long dev, int value);
	private static native int shake_write_midi_waveform(long dev, int value);

	private static native int sk6_read_cs0_inc(long dev);
	private static native int sk6_write_cs0_inc(long dev, int value);

	private static native int sk6_read_cs0_dec(long dev);
	private static native int sk6_write_cs0_dec(long dev, int value);

	private static native int sk6_read_cs0_inc_profile(long dev);
	private static native int sk6_write_cs0_inc_profile(long dev, int value);

	private static native int sk6_read_cs0_dec_profile(long dev);
	private static native int sk6_write_cs0_dec_profile(long dev, int value);

	private static native int sk6_read_cs1_inc(long dev);
	private static native int sk6_write_cs1_inc(long dev, int value);

	private static native int sk6_read_cs1_dec(long dev);
	private static native int sk6_write_cs1_dec(long dev, int value);

	private static native int sk6_read_cs1_inc_profile(long dev);
	private static native int sk6_write_cs1_inc_profile(long dev, int value);

	private static native int sk6_read_cs1_dec_profile(long dev);
	private static native int sk6_write_cs1_dec_profile(long dev, int value);

	private static native int sk6_read_cap_thresholds(long dev, byte[] values);
	private static native int sk6_write_cap_thresholds(long dev, byte[] values);

	private static native int shake_read_shaking_config(long dev);
	private static native int shake_write_shaking_config(long dev, int value);

	private static native int shake_read_shaking_accel_threshold(long dev);
	private static native int shake_write_shaking_accel_threshold(long dev, int value);

	private static native int shake_read_shaking_holdoff_time(long dev);
	private static native int shake_write_shaking_holdoff_time(long dev, int value);

	private static native int shake_read_shaking_vibration_profile(long dev);
	private static native int shake_write_shaking_vibration_profile(long dev, int value);

	private static native int shake_read_shaking_hpf_constant(long dev);
	private static native int shake_write_shaking_hpf_constant(long dev, int value);

	private static native int shake_read_shaking_lpf_constant(long dev);
	private static native int shake_write_shaking_lpf_constant(long dev, int value);

	private static native int shake_reset_shaking_detection(long dev);

	private static native int shake_write_packet_request(long dev, int value);

	private static native int shake_write_data_request(long dev, int value);

	private static native int shake_playvib(long dev, int channel, byte profile);

	private static native int sk6_upload_vib_sample(long dev, byte profile, int[] samples);

	private static native int shake_read_battery_level(long dev);

	private static native int shake_read_power_status(long dev);

	private static native float shake_read_temperature(long dev);

	private static native int shake_read(long dev, int addr);
	private static native int shake_write(long dev, int addr, int value);
}

