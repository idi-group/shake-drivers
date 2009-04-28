/** @file SK7_registers.h
*	Contains SK7 register definitions. */

/*	Copyright (c) 2006-2009, University of Glasgow
*	All rights reserved.
*
*	Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
*
*		* Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
*		* Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer
*			in the documentation and/or other materials provided with the distribution.
*		* Neither the name of the University of Glasgow nor the names of its contributors may be used to endorse or promote products derived 
*			from this software without specific prior written permission.
*
*	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, 
*	THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS 
*	BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE 
*	GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT 
*	LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#ifndef _SK7_REGISTERS_
#define _SK7_REGISTERS_

/** The value in the SK7_NV_REG_POWER1 register will be a combination of zero or more of the values 
*	in this enumeration. */
enum sk7_power1 {
	/** Enable/disable power to capacitive switches */
	SK7_POWER_CAP	= 0x01,
	/** Enable/disable power to accelerometer */
	SK7_POWER_ACC	= 0x02,
	/** Enable/disable power to gyroscopes */
	SK7_POWER_GYRO = 0x04,
	/** Enable/disable power to vibrator */
	SK7_POWER_VIB = 0x08,
	/** Enable/disable power to magnetometers */
	SK7_POWER_MAG = 0x10,
	/** Enable/disable power to temperature sensor */
	SK7_POWER_TEMP = 0x20,
	/** Enable/disable power to analog inputs */
	SK7_POWER_ANALOG = 0x40,
	/** Enable/disable power to navigation switch */
	SK7_POWER_NAV	= 0x80,
};

/** The value in the SK7_NV_REG_POWER2 register will be a combination of zero or more of the values
*	in this enumeration */
enum sk7_power2 {
	/** Enable/disable increasing threshold trigger on cap switches */
	SK7_POWER_CAP_INC	= 0x01,
	/** Enable/disable decreasing threshold trigger on cap switches */
	SK7_POWER_CAP_DEC	= 0x02,
	/** Enable/disable event output for threshold triggering on both switches */
	SK7_POWER_CS_EVENTS = 0x10,
	/** Enable/disable compass heading sensor */
	SK7_POWER_COMPASS_HEADING = 0x40,
	/** Enable/disable power to aux socket */
	SK7_POWER_AUX = 0x80,
};

/**	SHAKE sensor listing. 
*	@see shake_read_sample_rate()
*	@see shake_write_sample_rate() */
enum sk7_sensors {
	/**	Accelerometer */
	SK7_SENSOR_ACC = 0,
	/**	Gyroscope */
	SK7_SENSOR_GYRO = 1,
	/** Magnetometer */
	SK7_SENSOR_MAG = 2,
	/**	Compass heading */
	SK7_SENSOR_HEADING = 3,
	/**	Capacitive switches */
	SK7_SENSOR_CAP = 4,
	/**	Analog input 0 */
	SK7_SENSOR_ANA0 = 6,
	/**	Analog input 1 */
	SK7_SENSOR_ANA1 = 7,
};

/**	 Maximum output rate for each sensor on the SHAKE. 
*	@see shake_read_sample_rate()
*	@see shake_write_sample_rate() */
enum sk7_max_output_rates {
	/**	Accelerometer */
	SK7_ACC_MAX_RATE = 0xFF,
	/**	Gyroscope */
	SK7_GYRO_MAX_RATE	= 0xFF,
	/** Magnetometer */
	SK7_MAG_MAX_RATE = 0xFF,
	/**	Compass heading */
	SK7_HEADING_MAX_RATE = 0xFF,
	/**	Capacitive switches */
	SK7_CAP_MAX_RATE = 0xFF,
	/**	Analog input 0 */
	SK7_ANA0_MAX_RATE	= 0xFF,
	/**	Analog input 1 */
	SK7_ANA1_MAX_RATE	= 0xFF,
};

/** Lists the available SHAKE vibration channels.
*	@see shake_playvib() */
enum sk7_vib_channels {
	/** Main (builtin) vibrator */
	SK7_VIB_MAIN = 0,
	/** External vibrator #1 */
	SK7_VIB_LEFT = 1,
	/** External vibrator #2 */
	SK7_VIB_RIGHT	= 2,
};

/** Maximum length of a vibration sample (in pairs of speed,time values) */
#define SK7_VIB_SAMPLE_MAX_LENGTH		32
/** Lowest available memory location for storing vibration samples (note that location 0 is reserved and cannot be used) */
#define SK7_VIB_PROFILE_MIN	0x01 
/** Highest available memory location for storing vibration samples */
#define SK7_VIB_PROFILE_MAX	0xFF
/** Maximum speed value for the vibration samples */
#define SK7_VIB_SPEED_MAX		0xFF
/** Maximum time value for the vibration samples (note that the times are in units of 10ms) */
#define SK7_VIB_TIME_MAX		0xFF

/** The value in the VO_REG_PWRSTA register will be a combination of zero or more of these values */
enum sk7_power_state {
	/**	SHAKE is connected to AC power adapter. */
	SK7_EXT_POWER	= 0x01,
	/** SHAKE battery is currently charging. */
	SK7_BAT_CHARGING = 0x02,
	/** SHAKE battery is full. */
	SK7_BAT_FULL = 0x04,
};

/** length of RFID TID */
#define SK7_RFID_TAG_LENGTH	16

/** When writing into the SK7_NV_REG_RFID_CONFIG register, the value should be a combination of ONE or more of the following
*	values */
enum sk7_rfid_config {
	/**	Turns RFID scanner on/off */
	SK7_RFID_POWER_ON = 0x01,
	/** Enables Red LED blink on RFID scan success */
	SK7_RFID_VISUAL_FEEDBACK = 0x02,
	/** Enable auto scanning */
	SK7_RFID_AUTO_SCAN = 0x04,
};

/** When applied to the value contained in the VO_REG_PWRSTA register, nonzero if external power is connected */
#define SK7_EXT_POWER_ON(x)		(x & SK7_EXT_POWER)
/** When applied to the value contained in the VO_REG_PWRSTA register, nonzero if battery is charging */
#define SK7_BATTERY_CHARGING(x)	(x & SK7_BAT_CHARGING)
/** When applied to the value contained in the VO_REG_PWRSTA register, nonzero if battery is full */
#define SK7_BATTERY_FULL(x)		(x & SK7_BAT_FULL)

/** Maximum battery level
*	@see shake_read_battery_level() */
#define SK7_BATTERY_MAX	0xFF

/** Minimum temperature level for SHAKE temperature sensor.
*	@see shake_read_temperature() */
#define SK7_TEMP_MIN		0x00

/** Maximum temperature level for SHAKE temperature sensor.
*	@see shake_read_temperature() */
#define SK7_TEMP_MAX		0x40

/**	Length of an audio sample packet in bytes */
#define SK7_AUDIO_DATA_LEN	0x20

/**	SHAKE audio sample rate in Hz */
#define SK7_AUDIO_SAMPLE_RATE		7200

/** SHAKE audio sample size in bits */
#define SK7_AUDIO_SAMPLE_SIZE		16

/** Mute MIDI synth */
#define SK7_MIDI_AMPLITUDE_MUTE	0x00

/** Minimum MIDI synth amplitude value */
#define SK7_MIDI_AMPLITUDE_MIN	0x01

/** Maximum MIDI synth amplitude value */
#define SK7_MIDI_AMPLITUDE_MAX	0xFF

/** Minimum MIDI note number */
#define SK7_MIDI_NOTE_MIN		0x21

/** Maximum MIDI note number */
#define SK7_MIDI_NOTE_MAX		0x69

/** Audio page size */
#define SK7_UPLOAD_PAGE_SIZE	1056

/**	Audio sample minimum amplitude */
#define SK7_AUDIO_AMPLITUDE_MIN	0

/** Audio sample maximum amplitude */
#define SK7_AUDIO_AMPLITUDE_MAX	255

/** Audio sample upload minimum address */
#define SK7_UPLOAD_MIN_PAGE 0

/**	Audio sample upload maximum address */
#define SK7_UPLOAD_MAX_PAGE 7999

/**	Vibration sample minimum amplitude for ext module */
#define SK7_VIB_AMPLITUDE_MIN	0

/** Vibration sample maximum amplitude for ext module */
#define SK7_VIB_AMPLITUDE_MAX	255

#define SK7_UPLOAD_CHUNK_SIZE		64
#define SK7_UPLOAD_DELAY		30

/** If writing into the VO_REG_MIDI_WAVEFORM register, the value to write should be ONE of the following values */
enum sk7_midi_waveforms {
	/** Sine wave */
	SK7_MIDI_WAVEFORM_SINE = 0,
	/** Square wave */
	SK7_MIDI_WAVEFORM_SQUARE = 1,
	/** Triangle wave */
	SK7_MIDI_WAVEFORM_TRIANGLE = 2,
	/** Sawtooth wave */
	SK7_MIDI_WAVEFORM_SAWTOOTH = 3
};

/**	If writing into the VO_REG_LOGGING_CTRL register, the value to write should be ONE of the following values */
enum sk7_logging_commands {
	/**	Playback logged data */
	SK7_LOGGING_PLAY = 0x01,
	/**	Pause playback/recording */
	SK7_LOGGING_PAUSE = 0x02,
	/** Stop playback/recording and reset memory read/write position back to the start */
	SK7_LOGGING_STOP = 0x03,
	/**	Start recording data */
	SK7_LOGGING_RECORD = 0x04,
	/**	Reset SHAKE logging timer to zero */
	SK7_LOGGING_RESET	= 0x05,
};

/**	When reading the VO_REG_LOGGING_STATUS register, the result will be a combination of zero or more of these values */
enum sk7_logging_state {
	/**	Logging is currently active */
	SK7_LOGGING_RECORDING	= 0x01,
	/** Playback is currently active */
	SK7_LOGGING_PLAYING = 0x02,
	/** Internal memory is almost full */
	SK7_LOGGING_MEMORYFULL = 0x04,
};

/*	the rest of these #defines list the actual register addresses on the device. You don't
*	need to bother with these unless you're using the basic shake_read/shake_write register
*	access functions */

/*  non-volatile register addresses. The values stored in these
*	registers control various aspects of each SHAKE device. The values 
*	each register can take is described in the associated comments.
*	Note that you can both read and write all these registers. */

/**	Power configuration register #1 */
#define SK7_NV_REG_POWER1 		0x0000
/** Power configuration register #2 */
#define SK7_NV_REG_POWER2 		0x0001

/** Data output format register */
#define SK7_NV_REG_DATAFMT		0x0002

/** Communications configuration register (currently reserved) */
#define SK7_NV_REG_COMMS_CONFIG 0x0003

/** MEMS configuration register */
#define SK7_NV_REG_MEMS_CONFIG	0x0004

/** Data output calibration bypass register */
#define SK7_NV_REG_CALIB_BYPASS	0x0006

/** Data output cross-axes calibration bypass register */
#define SK7_NV_REG_CX_CALIB_BYPASS		0x0007

/** Output data rates register for accelerometer */
#define SK7_NV_REG_ACCOUT 		0x0008
/** Output data rates register for gyroscope */
#define SK7_NV_REG_GYROUT		0x0009
/** Output data rates register for magnetometer */
#define SK7_NV_REG_MAGOUT		0x000A
/** Output data rates register for compass heading */
#define SK7_NV_REG_HEDOUT		0x000B
/** Output data rates register for capacitive switch 0 */
#define SK7_NV_REG_CAP0OUT		0x000C
/** Output data rates register for capacitive switch 1 */
#define SK7_NV_REG_CAP1OUT		0x000D	
/** Output data rates register for analog input 0 */
#define SK7_NV_REG_ANA0OUT		0x000E
/** Output data rates register for analog input 1 */
#define SK7_NV_REG_ANA1OUT		0x000F

/** Temperature compensation register */
#define SK7_NV_REG_TEMP_COMPENSATION 0x0010

/** Data packet streaming disable register */
#define SK7_NV_REG_STREAM_DISABLE	0x0012

/** Digital filter configuration for accelerometer */
#define SK7_NV_REG_DIGFIL_ACC		0x0014
/** Digital filter configuration for gyroscope */
#define SK7_NV_REG_DIGFIL_GYR		0x0015
/** Digital filter configuration for magnetometer */
#define SK7_NV_REG_DIGFIL_MAG		0x0016
/** Digital filter configuration for compass heading */
#define SK7_NV_REG_DIGFIL_HED		0x0017
/** Digital filter configuration for capacitive switch 0 */
#define SK7_NV_REG_DIGFIL_CAP0		0x0018
/** Digital filter configuration for capacitive switch 1 */
#define SK7_NV_REG_DIGFIL_CAP1		0x0019
/** Digital filter configuration for analog input 0 */
#define SK7_NV_REG_DIGFIL_ANA0		0x001A
/** Digital filter configuration for analog input 1 */
#define SK7_NV_REG_DIGFIL_ANA1		0x001B

/**	Audio configuration register */
#define SK7_NV_REG_AUDIO_CONFIG		0x001E

/** Expansion module configuration register */
#define SK7_NV_REG_EXPANSION_CONFIG	0x0026

/**	Increasing threshold for capacitive switch 0 */
#define SK7_NV_REG_CS0_INC			0x0028
/**	Decreasing threshold for capacitive switch 0 */
#define SK7_NV_REG_CS0_DEC			0x0029
/**	Increasing threshold vibration profile for capacitive switch 0 */
#define SK7_NV_REG_CS0_INC_PROFILE	0x002A
/**	Decreasing threshold vibration profile for capacitive switch 0 */
#define SK7_NV_REG_CS0_DEC_PROFILE	0x002B
/**	Increasing threshold for capacitive switch 1 */
#define SK7_NV_REG_CS1_INC			0x002C
/**	Decreasing threshold for capacitive switch 1 */
#define SK7_NV_REG_CS1_DEC			0x002D
/**	Increasing threshold vibration profile for capacitive switch 1 */
#define SK7_NV_REG_CS1_INC_PROFILE	0x002E
/**	Decreasing threshold vibration profile for capacitive switch 1 */
#define SK7_NV_REG_CS1_DEC_PROFILE	0x002F

/** Shaking detection configuration register */
#define SK7_NV_REG_SHAKING_CONFIG	0x0030

/** Controls shaking detection minimum acceleration threshold */
#define SK7_NV_REG_SHAKING_ACCEL_THRESHOLD			0x0032
/** Controls shaking detection trigger hold-off time */
#define SK7_NV_REG_SHAKING_HOLDOFF_TIME				0x0033
/** Sets the vibration profile to playback when a shaking event is registered */
#define SK7_NV_REG_SHAKING_VIBRATION_PROFILE		0x0034
/** Sets the high pass filter time constant for the detection algorithm */
#define SK7_NV_REG_SHAKING_HPF_CONSTANT	0x0035
/** Sets the low pass filter time constant for the detection algorithm */
#define SK7_NV_REG_SHAKING_LPF_CONSTANT	0x0036

/** Least significant byte of logging packet count (READ ONLY register) */
#define SK7_NV_REG_LOGGING_PKT_LSB	0x0038
/** Most significant byte of logging packet count (READ ONLY register) */
#define SK7_NV_REG_LOGGING_PKT_MSB	0x0039

/** Heart rate monitor configuration register */
#define SK7_NV_REG_HEART_RATE_CONFIG	0x003A

/**	RFID scanner configuration */
#define SK7_NV_REG_RFID_CONFIG		0x003C

/** RFID scanner frequency */
#define SK7_NV_REG_RFID_FREQUENCY	0x003D

/* volatile register addresses. These are either write-only or read-only as indicated. */

/** Packet request register (WRITE ONLY) */
#define SK7_VO_REG_PKTREQ		0x0100

/** Data request register (WRITE ONLY) */
#define SK7_VO_REG_DATAREQ		0x0101

/** Main vibrotactile channel profile selection register (WRITE ONLY) */
#define SK7_VO_REG_VIB_MAIN		0x0102
/** Left vibrotactile channel profile selection register (WRITE ONLY) */
#define SK7_VO_REG_VIB_LEFT		0x0103
/** Right vibrotactile channel profile selection register (WRITE ONLY) */
#define SK7_VO_REG_VIB_RIGHT	0x0104

/** Battery level status register (READ ONLY) */
#define SK7_VO_REG_BATTERY 		0x0105

/** Power status register (READ ONLY) */
#define SK7_VO_REG_PWRSTA		0x0106

/** Temperature sensor reading (READ ONLY) */
#define SK7_VO_REG_TEMPERATURE	0x0108

/** Calibration mode control register (WRITE ONLY) */
#define SK7_VO_REG_CALIBRATION	0x010A

/** Data logging command register (WRITE ONLY) */
#define SK7_VO_REG_LOGGING_CTRL 0x010D

/** Data logging status register (READ ONLY) */
#define SK7_VO_REG_LOGGING_STATUS 0x010E

/** Data logging memory remaining register (READ ONLY) */ 
#define SK7_VO_REG_LOGGING_MEM	0x0110

/** MIDI synth amplitude control */
#define SK7_VO_REG_MIDI_AMPLITUDE 0x0111

/** MIDI note number */
#define SK7_VO_REG_MIDI_NOTE		0x0112

/** MIDI waveform */
#define SK7_VO_REG_MIDI_WAVEFORM	0x0113

/**	RFID scan trigger */
#define SK7_VO_REG_RFID_SCAN		0x011A

#define SK7_VO_REG_EXP_PWM1		0x0120
#define SK7_VO_REG_EXP_PWM2		0x0121
#define SK7_VO_REG_EXP_PWM3		0x0122
#define SK7_VO_REG_EXP_PWM4		0x0123
#define SK7_VO_REG_EXP_PWM5		0x0124
#define SK7_VO_REG_EXP_PWM6		0x0125

#define SK7_VO_REG_VIB_LEFT_CONTINUOUS	0x0126
#define SK7_VO_REG_VIB_RIGHT_CONTINUOUS 0x0127

#endif
