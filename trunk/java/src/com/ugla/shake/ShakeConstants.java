package com.ugla.shake;

/** 	Various constant definitions */
public class ShakeConstants {
	/**	Global success code for SHAKE functions. */
	public static final int SHAKE_SUCCESS = 1;

	/** Global error code for SHAKE functions. */
	public static final int SHAKE_ERROR = -1;
	
	/** Device types */
	public enum ShakeDeviceType {
		SHAKE_SK6,
		SHAKE_SK7,
	}
	
	public static final int SHAKE_BAD_PACKET = -1;
	
	/**	These are the set of optional modules that can be installed in the SHAKE SK6.
	*	They are divided into two sets, one for each of the two available slots.
	*	Most modules will only fit into one slot, eg the RFID module can only be installed
	*	in slot 1. To check which module(s) are installed in a SHAKE, use the
	*	info_module_slot1() and info_module_slot2() functions of the shakeDevice object. Each function will
	*	return a value from the list below indicating which module is installed in the selected slot.
	*	If no module is installed, the value SHAKE_MODULE_NONE is returned. */
		
	/** indicates no module is installed */
	public static final int SK6_MODULE_NONE = 0;

	/** slot 1; G-01 gyro module */
	public static final int SK6_MODULE_GYRO1 = 1;	
	/** slot 1; G-02 gyro module */
	public static final int SK6_MODULE_GYRO2 = 2;		
	/** slot 1; RFID module */
	public static final int SK6_MODULE_RFID = 3;

	/** slot 2; audio module */
	public static final int SK6_MODULE_AUDIO = 4;
	/** slot 2; external vibration module */
	public static final int SK6_MODULE_EXTVIB = 5;
	/** slot 2; usb module (none of these yet) */
	public static final int SK6_MODULE_USB = 6;
	/** slot 2; expansion module (STANE) */
	public static final int SK6_MODULE_EXPANSION = 7;
		
	public static final int SK6_MODULE_LAST = 8;
	
	/** SK7 optional module list (TODO!) */
	public static final int SK7_MODULE_NONE = 20;
	public static final int SK7_MODULE_LAST = 21;
	
	/**	Accelerometer */
	public static final int SHAKE_SENSOR_ACC = 0;
	/**	Gyroscope */
	public static final int SHAKE_SENSOR_GYRO = 1;
	/** Magnetometer */
	public static final int SHAKE_SENSOR_MAG = 2;
	/**	Compass heading */
	public static final int SHAKE_SENSOR_HEADING = 3;
	/**	Capacitive sensor 0 (SK6 only!) */
	public static final int SHAKE_SENSOR_SK6_CAP0 = 4;
	/**	Capacitive sensor 1 (SK6 only!) */
	public static final int SHAKE_SENSOR_SK6_CAP1 = 5;
	/**	Analog input 0 */
	public static final int SHAKE_SENSOR_ANA0 = 6;
	/**	Analog input 1 */
	public static final int SHAKE_SENSOR_ANA1 = 7;
	
	/** Capacitive sensors (SK7 only!) */
	public static final int SHAKE_SENSOR_SK7_CAP = 4;

	public static final int SHAKE_VIB_MAIN = 0;
	public static final int SHAKE_VIB_LEFT = 1;
	public static final int SHAKE_VIB_RIGHT = 2;
	public static final int SHAKE_VIB_FORCEREACTOR = 3;
	public static final int SHAKE_VIB_ACTUATOR = 4;

	public static final int SHAKE_VIB_SAMPLE_MAX_LENGTH = 32;
	public static final int SHAKE_VIB_PROFILE_MIN = 0x01;
	public static final int SHAKE_VIB_PROFILE_MAX = 0xFF;

	public static final int SHAKE_EXT_POWER = 0x01;
	public static final int SHAKE_BAT_CHARGING = 0x02;
	public static final int SHAKE_BAT_FULL = 0x04;

	public static final int SHAKE_BATTERY_MAX = 0xFF;

	public static final int SHAKE_TEMP_MIN = 0x00;
	public static final int SHAKE_TEMP_MAX = 0x40;

	public static final int SHAKE_SHAKING_DETECTION = 0x01;
	public static final int SHAKE_SHAKING_DIRECTION_DETECTION = 0x02;
	public static final int SHAKE_SHAKING_VIBRATION = 0x04;


	// register addresses
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
	
	/** (SK7) roll-pitch-heading configuration */
	public static final int NV_REG_RPH_CONFIG	= 0x0046;
	
	/**	(SK7) heading feedback configuration register */
	public static final int NV_REG_HEADING_FEEDBACK = 0x0048;

	/**	(SK7) heading feedback lower threshold LSB */
	public static final int  NV_REG_HEADING_LOWER_LSB =	0x004A;

	/**	(SK7) heading feedback lower threshold MSB */
	public static final int NV_REG_HEADING_LOWER_MSB =	0x004B;

	/**	(SK7) heading feedback upper threshold LSB */
	public static final int NV_REG_HEADING_UPPER_LSB = 0x004C;

	/**	(SK7) heading feedback upper threshold MSB */
	public static final int NV_REG_HEADING_UPPER_MSB = 0x004D;

	/**	(SK7) heading feedback hysteris */
	public static final int NV_REG_HEADING_HYSTERESIS	= 0x004E;

	/**	(SK7) heading feedback vibration profile number */
	public static final int  NV_REG_HEADING_VIB_PROFILE = 0x004F;

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
	
	public static final int VO_REG_VIB_LEFT_CONTINUOUS = 0x0126;
	public static final int VO_REG_VIB_RIGHT_CONTINUOUS = 0x0127;
}
