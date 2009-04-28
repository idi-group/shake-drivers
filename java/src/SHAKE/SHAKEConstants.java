package SHAKE;

public class SHAKEConstants {
	
	public static int SHAKE_ERROR = -1;
	public static int SHAKE_SUCCESS = 1;
	
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
	

	
	public static final int NV_REG_POWER1 		= 0x0000;

	public static final int NV_REG_POWER2 		= 0x0001;

	
	public static final int NV_REG_DATAFMT		= 0x0002;


	public static final int NV_REG_ACC_CONFIG	= 0x0004;


	public static final int NV_REG_CALIB_BYPASS	= 0x0006;

	
	public static final int NV_REG_CX_CALIB_BYPASS		= 0x0007;


	public static final int NV_REG_ACCOUT 		= 0x0008;
	
	public static final int NV_REG_GYROUT		= 0x0009;

	public static final int NV_REG_MAGOUT		= 0x000A;
	
	public static final int NV_REG_HEDOUT		= 0x000B;
	
	public static final int NV_REG_CAP0OUT		= 0x000C;
	
	public static final int NV_REG_CAP1OUT		= 0x000D	;
	
	public static final int NV_REG_ANA0OUT		= 0x000E;
	
	public static final int NV_REG_ANA1OUT		= 0x000F;

	
	public static final int NV_REG_TEMP_COMPENSATION = 0x0010;

	
	public static final int NV_REG_STREAM_DISABLE	= 0x0012;

	
	public static final int NV_REG_DIGFIL_ACC		= 0x0014;

	public static final int NV_REG_DIGFIL_GYR		= 0x0015;
	
	public static final int NV_REG_DIGFIL_MAG		= 0x0016;
	
	public static final int NV_REG_DIGFIL_HED		= 0x0017;
	
	public static final int NV_REG_DIGFIL_CAP0		= 0x0018;
	
	public static final int NV_REG_DIGFIL_CAP1		= 0x0019;

	public static final int NV_REG_DIGFIL_ANA0		= 0x001A;
	
	public static final int NV_REG_DIGFIL_ANA1		= 0x001B;

	public static final int NV_REG_AUDIO_CONFIG		= 0x001E;

	public static final int NV_REG_EXPANSION_CONFIG	= 0x0026;

	public static final int NV_REG_CS0_INC			= 0x0028;
	public static final int NV_REG_CS0_DEC			= 0x0029;

	public static final int NV_REG_CS0_INC_PROFILE	= 0x002A;

	public static final int NV_REG_CS0_DEC_PROFILE	= 0x002B;

	public static final int NV_REG_CS1_INC			= 0x002C;

	public static final int NV_REG_CS1_DEC			= 0x002D;

	public static final int NV_REG_CS1_INC_PROFILE	= 0x002E;

	public static final int NV_REG_CS1_DEC_PROFILE	= 0x002F;

	public static final int NV_REG_SHAKING_CONFIG	= 0x0030;


	public static final int NV_REG_SHAKING_ACCEL_THRESHOLD			= 0x0032;

	public static final int NV_REG_SHAKING_HOLDOFF_TIME				= 0x0033;

	public static final int NV_REG_SHAKING_VIBRATION_PROFILE		= 0x0034;

	public static final int NV_REG_SHAKING_HPF_CONSTANT	= 0x0035;
	
	public static final int NV_REG_SHAKING_LPF_CONSTANT	= 0x0036;


	public static final int NV_REG_LOGGING_PKT_LSB	= 0x0038;

	public static final int NV_REG_LOGGING_PKT_MSB	= 0x0039;

	public static final int NV_REG_HEART_RATE_CONFIG	= 0x003A;

	public static final int NV_REG_RFID_CONFIG		= 0x003C;

	public static final int NV_REG_RFID_FREQUENCY	= 0x003D;

	/* volatile register addresses. These are either write-only or read-only as indicated. */

	public static final int VO_REG_PKTREQ		= 0x0100;

	public static final int VO_REG_DATAREQ		= 0x0101;

	public static final int VO_REG_VIB_MAIN		= 0x0102;
	public static final int VO_REG_VIB_LEFT		= 0x0103;
	public static final int VO_REG_VIB_RIGHT	= 0x0104;

	public static final int VO_REG_BATTERY 		= 0x0105;

	public static final int VO_REG_PWRSTA		= 0x0106;

	public static final int VO_REG_TEMPERATURE	= 0x0108;

	public static final int VO_REG_CALIBRATION	= 0x010A;

	public static final int VO_REG_LOGGING_CTRL = 0x010D;

	public static final int VO_REG_LOGGING_STATUS = 0x010E;

	public static final int VO_REG_LOGGING_MEM	= 0x0110;

	public static final int VO_REG_MIDI_AMPLITUDE = 0x0111;

	public static final int VO_REG_MIDI_NOTE		= 0x0112;


	public static final int VO_REG_MIDI_WAVEFORM	= 0x0113;

	public static final int VO_REG_RFID_SCAN		= 0x011A;

	public static final int VO_REG_EXP_PWM1		= 0x0120;
	public static final int VO_REG_EXP_PWM2		= 0x0121;
	public static final int VO_REG_EXP_PWM3		= 0x0122;
	public static final int VO_REG_EXP_PWM4		= 0x0123;
	public static final int VO_REG_EXP_PWM5		= 0x0124;
	public static final int VO_REG_EXP_PWM6		= 0x0125;
}
