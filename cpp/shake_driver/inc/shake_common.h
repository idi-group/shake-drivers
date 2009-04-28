#ifndef _SHAKE_COMMON_H_
#define _SHAKE_COMMON_H_

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

#endif
