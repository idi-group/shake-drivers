/** @file shake_registers.h
*	Contains SHAKE register definitions common to SK6 and SK7 
*/

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


#ifndef _SHAKE_REGISTERS_
#define _SHAKE_REGISTERS_


/**	SHAKE sensor listing. 
*	@see shake_read_sample_rate()
*	@see shake_write_sample_rate() */
/**	Accelerometer */
#define SHAKE_SENSOR_ACC 0
/**	Gyroscope */
#define SHAKE_SENSOR_GYRO 1
/** Magnetometer */
#define SHAKE_SENSOR_MAG 2
/**	Compass heading */
#define SHAKE_SENSOR_HEADING 3
/**	Capacitive sensor 0 (SK6 only!) */
#define SHAKE_SENSOR_SK6_CAP0 4
/**	Capacitive sensor 1 (SK6 only!) */
#define SHAKE_SENSOR_SK6_CAP1 5
/**	Analog input 0 */
#define SHAKE_SENSOR_ANA0 6
/**	Analog input 1 */
#define SHAKE_SENSOR_ANA1 7

/**	Capacitive sensors (SK7 only!) */
#define SHAKE_SENSOR_CAP 4


#define SHAKE_MAX_OUTPUT_RATE 0xFF

/** Lists the available SHAKE vibration channels.
*	@see shake_playvib() */
enum shake_vib_channels {
	/** Main (builtin) vibrator */
	SHAKE_VIB_MAIN = 0,
	/** External vibrator #1 */
	SHAKE_VIB_LEFT = 1,
	/** External vibrator #2 */
	SHAKE_VIB_RIGHT	= 2,
};

/** Maximum length of a vibration sample (in pairs of speed,time values) */
#define SHAKE_VIB_SAMPLE_MAX_LENGTH		32
/** Lowest available memory location for storing vibration samples (note that location 0 is reserved and cannot be used) */
#define SHAKE_VIB_PROFILE_MIN	0x01 
/** Highest available memory location for storing vibration samples */
#define SHAKE_VIB_PROFILE_MAX	0xFF
/** Maximum speed value for the vibration samples */
#define SHAKE_VIB_SPEED_MAX		0xFF
/** Maximum time value for the vibration samples (note that the times are in units of 10ms) */
#define SHAKE_VIB_TIME_MAX		0xFF

/** The value in the VO_REG_PWRSTA register will be a combination of zero or more of these values */
enum shake_power_state {
	/**	SHAKE is connected to AC/USB power adapter. */
	SHAKE_EXT_POWER	= 0x01,
	/** SHAKE battery is currently charging. */
	SHAKE_BAT_CHARGING = 0x02,
	/** SHAKE battery is full. */
	SHAKE_BAT_FULL = 0x04,
};


/** When writing into the SK6_NV_REG_RFID_CONFIG register, the value should be a combination of ONE or more of the following
*	values */
enum shake_rfid_config {
	/**	Turns RFID scanner on/off */
	SHAKE_RFID_POWER_ON = 0x01,
	/** Enables Red LED blink on RFID scan success */
	SHAKE_RFID_VISUAL_FEEDBACK = 0x02,
	/** Enable auto scanning */
	SHAKE_RFID_AUTO_SCAN = 0x04,
};


/** When applied to the value contained in the VO_REG_PWRSTA register, nonzero if external power is connected */
#define SHAKE_EXT_POWER_ON(x)		(x & SHAKE_EXT_POWER)
/** When applied to the value contained in the VO_REG_PWRSTA register, nonzero if battery is charging */
#define SHAKE_BATTERY_CHARGING(x)	(x & SHAKE_BAT_CHARGING)
/** When applied to the value contained in the VO_REG_PWRSTA register, nonzero if battery is full */
#define SHAKE_BATTERY_FULL(x)		(x & SHAKE_BAT_FULL)

/** If writing into the VO_REG_MIDI_WAVEFORM register, the value to write should be ONE of the following values */
enum shake_midi_waveforms {
	/** Sine wave */
	SHAKE_MIDI_WAVEFORM_SINE = 0,
	/** Square wave */
	SHAKE_MIDI_WAVEFORM_SQUARE = 1,
	/** Triangle wave */
	SHAKE_MIDI_WAVEFORM_TRIANGLE = 2,
	/** Sawtooth wave */
	SHAKE_MIDI_WAVEFORM_SAWTOOTH = 3
};

/**	If writing into the VO_REG_LOGGING_CTRL register, the value to write should be ONE of the following values */
enum shake_logging_commands {
	/**	Playback logged data */
	SHAKE_LOGGING_PLAY = 0x01,
	/**	Pause playback/recording */
	SHAKE_LOGGING_PAUSE = 0x02,
	/** Stop playback/recording and reset memory read/write position back to the start */
	SHAKE_LOGGING_STOP = 0x03,
	/**	Start recording data */
	SHAKE_LOGGING_RECORD = 0x04,
	/**	Reset SHAKE logging timer to zero */
	SHAKE_LOGGING_RESET	= 0x05,
};

/**	When reading the VO_REG_LOGGING_STATUS register, the result will be a combination of zero or more of these values */
enum shake_logging_state {
	/**	Logging is currently active */
	SHAKE_LOGGING_RECORDING	= 0x01,
	/** Playback is currently active */
	SHAKE_LOGGING_PLAYING = 0x02,
	/** Internal memory is almost full */
	SHAKE_LOGGING_MEMORYFULL = 0x04,
};


/** length of RFID TID */
#define SHAKE_RFID_TAG_LENGTH	16

/** Maximum battery level
*	@see shake_read_battery_level() */
#define SHAKE_BATTERY_MAX	0xFF

/** Minimum temperature level for SHAKE temperature sensor.
*	@see shake_read_temperature() */
#define SHAKE_TEMP_MIN		0x00

/** Maximum temperature level for SHAKE temperature sensor.
*	@see shake_read_temperature() */
#define SHAKE_TEMP_MAX		0x40

/**	Length of an audio sample packet in bytes */
#define SHAKE_AUDIO_DATA_LEN	0x20

/**	SHAKE audio sample rate in Hz */
#define SHAKE_AUDIO_SAMPLE_RATE		7200

/** SHAKE audio sample size in bits */
#define SHAKE_AUDIO_SAMPLE_SIZE		16

/** Mute MIDI synth */
#define SHAKE_MIDI_AMPLITUDE_MUTE	0x00

/** Minimum MIDI synth amplitude value */
#define SHAKE_MIDI_AMPLITUDE_MIN	0x01

/** Maximum MIDI synth amplitude value */
#define SHAKE_MIDI_AMPLITUDE_MAX	0xFF

/** Minimum MIDI note number */
#define SHAKE_MIDI_NOTE_MIN		0x21

/** Maximum MIDI note number */
#define SHAKE_MIDI_NOTE_MAX		0x69

/** Audio page size */
#define SHAKE_UPLOAD_PAGE_SIZE	1056

/**	Audio sample minimum amplitude */
#define SHAKE_AUDIO_AMPLITUDE_MIN	0

/** Audio sample maximum amplitude */
#define SHAKE_AUDIO_AMPLITUDE_MAX	255

/** Audio sample upload minimum address */
#define SHAKE_UPLOAD_MIN_PAGE 0

/**	Audio sample upload maximum address */
#define SHAKE_UPLOAD_MAX_PAGE 7999

/**	Vibration sample minimum amplitude for ext module */
#define SHAKE_VIB_AMPLITUDE_MIN	0

/** Vibration sample maximum amplitude for ext module */
#define SHAKE_VIB_AMPLITUDE_MAX	255

#define SHAKE_UPLOAD_CHUNK_SIZE		64
#define SHAKE_UPLOAD_DELAY		30

/*	the rest of these #defines list the actual register addresses on the device. You don't
*	need to bother with these unless you're using the basic shake_read/shake_write register
*	access functions */

/*  non-volatile register addresses. The values stored in these
*	registers control various aspects of each SHAKE device. The values 
*	each register can take is described in the associated comments.
*	Note that you can both read and write all these registers. */

/**	Power configuration register #1 */
#define SHAKE_NV_REG_POWER1 		0x0000
/** Power configuration register #2 */
#define SHAKE_NV_REG_POWER2 		0x0001

/** Data output format register */
#define SHAKE_NV_REG_DATAFMT		0x0002

/** Accelerometer configuration register */
#define SHAKE_NV_REG_ACC_CONFIG	0x0004

/** Data output calibration bypass register */
#define SHAKE_NV_REG_CALIB_BYPASS	0x0006

/** Data output cross-axes calibration bypass register */
#define SHAKE_NV_REG_CX_CALIB_BYPASS		0x0007

/** Output data rates register for accelerometer */
#define SHAKE_NV_REG_ACCOUT 		0x0008
/** Output data rates register for gyroscope */
#define SHAKE_NV_REG_GYROUT		0x0009
/** Output data rates register for magnetometer */
#define SHAKE_NV_REG_MAGOUT		0x000A
/** Output data rates register for compass heading */
#define SHAKE_NV_REG_HEDOUT		0x000B
/** Output data rates register for capacitive switch 0 */
#define SHAKE_NV_REG_CAP0OUT		0x000C
/** Output data rates register for capacitive switch 1 */
#define SHAKE_NV_REG_CAP1OUT		0x000D	
/** Output data rates register for analog input 0 */
#define SHAKE_NV_REG_ANA0OUT		0x000E
/** Output data rates register for analog input 1 */
#define SHAKE_NV_REG_ANA1OUT		0x000F

/** Temperature compensation register */
#define SHAKE_NV_REG_TEMP_COMPENSATION 0x0010

/** Data packet streaming disable register */
#define SHAKE_NV_REG_STREAM_DISABLE	0x0012

/** Digital filter configuration for accelerometer */
#define SHAKE_NV_REG_DIGFIL_ACC		0x0014
/** Digital filter configuration for gyroscope */
#define SHAKE_NV_REG_DIGFIL_GYR		0x0015
/** Digital filter configuration for magnetometer */
#define SHAKE_NV_REG_DIGFIL_MAG		0x0016
/** Digital filter configuration for compass heading */
#define SHAKE_NV_REG_DIGFIL_HED		0x0017
/** Digital filter configuration for capacitive switch 0 */
#define SHAKE_NV_REG_DIGFIL_CAP0		0x0018
/** Digital filter configuration for capacitive switch 1 */
#define SHAKE_NV_REG_DIGFIL_CAP1		0x0019
/** Digital filter configuration for analog input 0 */
#define SHAKE_NV_REG_DIGFIL_ANA0		0x001A
/** Digital filter configuration for analog input 1 */
#define SHAKE_NV_REG_DIGFIL_ANA1		0x001B

/**	Audio configuration register */
#define SHAKE_NV_REG_AUDIO_CONFIG		0x001E

/** Expansion module configuration register */
#define SHAKE_NV_REG_EXPANSION_CONFIG	0x0026

/**	Increasing threshold for capacitive switch 0 */
#define SHAKE_NV_REG_CS0_INC			0x0028
/**	Decreasing threshold for capacitive switch 0 */
#define SHAKE_NV_REG_CS0_DEC			0x0029
/**	Increasing threshold vibration profile for capacitive switch 0 */
#define SHAKE_NV_REG_CS0_INC_PROFILE	0x002A
/**	Decreasing threshold vibration profile for capacitive switch 0 */
#define SHAKE_NV_REG_CS0_DEC_PROFILE	0x002B
/**	Increasing threshold for capacitive switch 1 */
#define SHAKE_NV_REG_CS1_INC			0x002C
/**	Decreasing threshold for capacitive switch 1 */
#define SHAKE_NV_REG_CS1_DEC			0x002D
/**	Increasing threshold vibration profile for capacitive switch 1 */
#define SHAKE_NV_REG_CS1_INC_PROFILE	0x002E
/**	Decreasing threshold vibration profile for capacitive switch 1 */
#define SHAKE_NV_REG_CS1_DEC_PROFILE	0x002F

/** Shaking detection configuration register */
#define SHAKE_NV_REG_SHAKING_CONFIG	0x0030

/** Controls shaking detection minimum acceleration threshold */
#define SHAKE_NV_REG_SHAKING_ACCEL_THRESHOLD			0x0032
/** Controls shaking detection trigger hold-off time */
#define SHAKE_NV_REG_SHAKING_HOLDOFF_TIME				0x0033
/** Sets the vibration profile to playback when a shaking event is registered */
#define SHAKE_NV_REG_SHAKING_VIBRATION_PROFILE		0x0034
/** Sets the high pass filter time constant for the detection algorithm */
#define SHAKE_NV_REG_SHAKING_HPF_CONSTANT	0x0035
/** Sets the low pass filter time constant for the detection algorithm */
#define SHAKE_NV_REG_SHAKING_LPF_CONSTANT	0x0036

/** Least significant byte of logging packet count (READ ONLY register) */
#define SHAKE_NV_REG_LOGGING_PKT_LSB	0x0038
/** Most significant byte of logging packet count (READ ONLY register) */
#define SHAKE_NV_REG_LOGGING_PKT_MSB	0x0039

/** Heart rate monitor configuration register */
#define SHAKE_NV_REG_HEART_RATE_CONFIG	0x003A

/**	RFID scanner configuration */
#define SHAKE_NV_REG_RFID_CONFIG		0x003C

/** RFID scanner frequency */
#define SHAKE_NV_REG_RFID_FREQUENCY	0x003D

/* volatile register addresses. These are either write-only or read-only as indicated. */

/** Packet request register (WRITE ONLY) */
#define SHAKE_VO_REG_PKTREQ		0x0100

/** Data request register (WRITE ONLY) */
#define SHAKE_VO_REG_DATAREQ		0x0101

/** Main vibrotactile channel profile selection register (WRITE ONLY) */
#define SHAKE_VO_REG_VIB_MAIN		0x0102
/** Left vibrotactile channel profile selection register (WRITE ONLY) */
#define SHAKE_VO_REG_VIB_LEFT		0x0103
/** Right vibrotactile channel profile selection register (WRITE ONLY) */
#define SHAKE_VO_REG_VIB_RIGHT	0x0104

/** Battery level status register (READ ONLY) */
#define SHAKE_VO_REG_BATTERY 		0x0105

/** Power status register (READ ONLY) */
#define SHAKE_VO_REG_PWRSTA		0x0106

/** Temperature sensor reading (READ ONLY) */
#define SHAKE_VO_REG_TEMPERATURE	0x0108

/** Calibration mode control register (WRITE ONLY) */
#define SHAKE_VO_REG_CALIBRATION	0x010A

/** Data logging command register (WRITE ONLY) */
#define SHAKE_VO_REG_LOGGING_CTRL 0x010D

/** Data logging status register (READ ONLY) */
#define SHAKE_VO_REG_LOGGING_STATUS 0x010E

/** Data logging memory remaining register (READ ONLY) */ 
#define SHAKE_VO_REG_LOGGING_MEM	0x0110

/** MIDI synth amplitude control */
#define SHAKE_VO_REG_MIDI_AMPLITUDE 0x0111

/** MIDI note number */
#define SHAKE_VO_REG_MIDI_NOTE		0x0112

/** MIDI waveform */
#define SHAKE_VO_REG_MIDI_WAVEFORM	0x0113

/**	RFID scan trigger */
#define SHAKE_VO_REG_RFID_SCAN		0x011A

/** LED colour override registers */
#define SHAKE_VO_REG_LED_RED		0x011B
#define SHAKE_VO_REG_LED_GREEN		0x011C
#define SHAKE_VO_REG_LED_BLUE		0x011D

#define SHAKE_VO_REG_EXP_PWM1		0x0120
#define SHAKE_VO_REG_EXP_PWM2		0x0121
#define SHAKE_VO_REG_EXP_PWM3		0x0122
#define SHAKE_VO_REG_EXP_PWM4		0x0123
#define SHAKE_VO_REG_EXP_PWM5		0x0124
#define SHAKE_VO_REG_EXP_PWM6		0x0125

#define SHAKE_VO_REG_VIB_LEFT_CONTINUOUS	0x0126
#define SHAKE_VO_REG_VIB_RIGHT_CONTINUOUS 0x0127

#endif
