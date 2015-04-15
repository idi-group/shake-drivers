/**
*	@file shake_driver.h
*	Main SHAKE header, for use with firmware version: 2.00/2.01 betas and higher
*
*	Copyright (c) 2006-2009, University of Glasgow
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

#ifndef _SHAKE_HEADER_
#define _SHAKE_HEADER_

#include "shake_platform.h"

#ifdef __cplusplus
extern "C" {
#endif


/**	Global success code for SHAKE functions. */
#define SHAKE_SUCCESS 	 1

/** Global error code for SHAKE functions. */
#define SHAKE_ERROR 	 -1

/** Values that can appear as the parameter of the SHAKE event callback function.
*	The SHAKE event callback function is used to relay several different events that can
*	be produced by the device. This enumeration defines a value for each individual event
*	so that the callback function can determine the type of the event that has occurred.
*	See the shake_register_event_callback() function for information on setting up the callback. */
enum shake_events { 
	/** Nav switch moved up */
	SHAKE_NAV_UP = 1,
	/** Nav switch moved down */
	SHAKE_NAV_DOWN = 2,
	/** Nav switch pressed */
	SHAKE_NAV_CENTRE,
	/** Nav switch released */
	SHAKE_NAV_NORMAL,
	/** Cap switch 0 upper threshold reached (SK6 only) */
	SK6_CS0_UPPER,
	/** Cap switch 0 lower threshold reached (SK6 only) */
	SK6_CS0_LOWER,
	/** Cap switch 1 upper threshold reached (SK6 only) */
	SK6_CS1_UPPER,
	/** Cap switch 1 lower threshold reached (SK6 only) */
	SK6_CS1_LOWER,
	/** Logging data playback complete */
	SHAKE_PLAYBACK_COMPLETE,
	/** Shaking event was triggered */
	SHAKE_SHAKING_EVENT,
	/** Heart rate monitor event */
	SHAKE_HEART_RATE_EVENT,
	/** RFID TID detection event */
	SHAKE_RFID_TID_EVENT,
	/**	Capacitive sensor 0 upper threshold reached (SK7 only) */
	SK7_CS0_UPPER,
	/** Capacitive sensor 0 lower threshold reached (SK7 only) */
	SK7_CS0_LOWER,
	/**	Capacitive sensor 1 upper threshold reached (SK7 only) */
	SK7_CS1_UPPER,
	/** Capacitive sensor 1 lower threshold reached (SK7 only) */
	SK7_CS1_LOWER,
	/**	Capacitive sensor 2 upper threshold reached (SK7 only) */
	SK7_CS2_UPPER,
	/** Capacitive sensor 2 lower threshold reached (SK7 only) */
	SK7_CS2_LOWER,
	/**	Capacitive sensor 3 upper threshold reached (SK7 only) */
	SK7_CS3_UPPER,
	/** Capacitive sensor 3 lower threshold reached (SK7 only) */
	SK7_CS3_LOWER,
	/**	Capacitive sensor 4 upper threshold reached (SK7 only) */
	SK7_CS4_UPPER,
	/** Capacitive sensor 4 lower threshold reached (SK7 only) */
	SK7_CS4_LOWER,
	/**	Capacitive sensor 5 upper threshold reached (SK7 only) */
	SK7_CS5_UPPER,
	/** Capacitive sensor 5 lower threshold reached (SK7 only) */
	SK7_CS5_LOWER,
	/**	Capacitive sensor 6 upper threshold reached (SK7 only) */
	SK7_CS6_UPPER,
	/** Capacitive sensor 6 lower threshold reached (SK7 only) */
	SK7_CS6_LOWER,
	/**	Capacitive sensor 7 upper threshold reached (SK7 only) */
	SK7_CS7_UPPER,
	/** Capacitive sensor 7 lower threshold reached (SK7 only) */
	SK7_CS7_LOWER,
	/**	Capacitive sensor 8 upper threshold reached (SK7 only) */
	SK7_CS8_UPPER,
	/** Capacitive sensor 8 lower threshold reached (SK7 only) */
	SK7_CS8_LOWER,
	/**	Capacitive sensor 9 upper threshold reached (SK7 only) */
	SK7_CS9_UPPER,
	/** Capacitive sensor 9 lower threshold reached (SK7 only) */
	SK7_CS9_LOWER,
	/**	Capacitive sensor 10 upper threshold reached (SK7 only) */
	SK7_CSA_UPPER,
	/** Capacitive sensor 10 lower threshold reached (SK7 only) */
	SK7_CSA_LOWER,
	/**	Capacitive sensor 11 upper threshold reached (SK7 only) */
	SK7_CSB_UPPER,
	/** Capacitive sensor 11 lower threshold reached (SK7 only) */
	SK7_CSB_LOWER,
};

/**	These are the set of optional modules that can be installed in the SHAKE SK6.
*	They are divided into two sets, one for each of the two available slots.
*	Most modules will only fit into one slot, eg the RFID module can only be installed
*	in slot 1. To check which module(s) are installed in a SHAKE, use the
*	shake_info_module_slot1() and shake_info_module_slot2() functions. Each function will
*	a value from the enumeration indicating which module is installed in the selected slot.
*	If no module is installed, the value SHAKE_MODULE_NONE is returned. */
enum sk6_optional_modules {
	/** indicates no module is installed */
	SK6_MODULE_NONE = 0,

	/** slot 1; G-01 gyro module */
	SK6_MODULE_GYRO1 = 1,		
	/** slot 1; G-02 gyro module */
	SK6_MODULE_GYRO2,			
	/** slot 1; RFID module */
	SK6_MODULE_RFID,	

	
	/** slot 2; audio module */
	SK6_MODULE_AUDIO,	
	/** slot 2; external vibration module */
	SK6_MODULE_EXTVIB,
	/** slot 2; usb module (none of these yet) */
	SK6_MODULE_USB,	
	/** slot 2; expansion module (STANE) */
	SK6_MODULE_EXPANSION,
	
	SK6_MODULE_LAST,
};

// TODO
enum sk7_optional_modules {
	SK7_MODULE_NONE = SK6_MODULE_LAST+1,

	SK7_MODULE_LAST,
};

/**	Valid values for the device_type parameter for all shake_init_device functions */
enum shake_device_types {
	SHAKE_SK6 = 0,
	SHAKE_SK7 = 1,
};

/** Instances of this structure represent a connection to a single SHAKE device. 
*	A succesful call to shake_init_device() will return a pointer to an instance of this structure. 
*	The application using the driver then has to pass this pointer as the first parameter when calling 
*	other SHAKE functions */
typedef struct {
	/** device ID number. */
	int handle; 		
	/** Pointer to internal data. */
	void* priv;			
} shake_device;

#include "shake_registers.h"

/*	=== Startup/shutdown functions === */

// NOTE:
//	valid forms for bluetooth address string in rfcomm funcs:
//		1. "aabbccddeeff"
//		2. "0xaabbccddeeff"
//		3. "aa:bb:cc:dd:ee:ff"
//	ALL LETTERS MUST BE LOWER CASE FOR NOW

#ifdef _WIN32
/**	(Windows only) Creates a connection to a SHAKE device.
*	This function attempts to create a Bluetooth connection to a SHAKE device. Typically on a 
*	Windows system you cannot specify a Bluetooth name or hardware address, so the only parameter
*	required is the outgoing COM port number to connect to the device through (see SHAKE user manual)
*	@param com_port The COM port number (accepted range is -1 to autodetect, else 1-100)
*	@param device_type indicates if the target is an SK6 or SK7
*
*	@return NULL on failure, otherwise a pointer to a shake_device structure */
SHAKE_API shake_device* shake_init_device(int com_port, int device_type);

/**	(Windows XP SP2 / Windows Mobile 5) Creates a connection to a SHAKE device.
*	Unlike the shake_init_device() function, this function uses the Microsoft Bluetooth API to
*	open an RFCOMM connection to a specific SHAKE device, currently specified by a numeric 
*	Bluetooth address. 
*	This function will not work if you are using non-Microsoft Bluetooth drivers!
*	@param btaddr Bluetooth MAC address of the target SHAKE
*	@param device_type indicates if the target is an SK6 or SK7
*	
*	@return NULL on failure, otherwise a pointer to a shake_device structure */

SHAKE_API shake_device* shake_init_device_rfcomm_i64(SHAKE_INT64 btaddr, int device_type);
SHAKE_API shake_device* shake_init_device_rfcomm_str(char* btaddr, int device_type);

#else
/*	++ Linux only ++
*	Main initialisation function for setting up a link to a SHAKE device.
*	<btaddr> is the Bluetooth MAC address of the device to connect to, 
*	given in string format (eg "12:34:56:78:90:ab")
*	
*	Returns NULL on failure, otherwise a pointer to a shake_device 
*	structure which can then be passed to the functions below. */
SHAKE_API shake_device* shake_init_device_rfcomm_i64(SHAKE_INT64 btaddr, int device_type);
SHAKE_API shake_device* shake_init_device_rfcomm_str(char* btaddr, int device_type);
#endif

#ifndef _WIN32
SHAKE_API shake_device* shake_init_device_usb_serial(char* usb_dev, int device_type);
#endif

/**	Only for debugging use */
SHAKE_API shake_device* shake_init_device_DEBUGFILE(char* readfile, char* writefile, int device_type);

/**
*	Close the link with a SHAKE device and free up any resources used in maintaining the connection.
*	@param sh pointer to a shake_device structure as returned by shake_init_device()
*
*	@return SHAKE_SUCCESS on success, SHAKE_ERROR on failure. */
SHAKE_API int shake_free_device(shake_device* sh);

/* === SHAKE miscellaneous functions === */

/**	Query SHAKE firmware revision number.
*	@param sh pointer to a shake_device structure as returned by shake_init_device()
*	@return SHAKE firmware revision number (format is x.yz), or 0.0 on error. */
SHAKE_API float shake_info_firmware_revision(shake_device* sh);

/**	Query SHAKE hardware revision number.
*	@param sh pointer to a shake_device structure as returned by shake_init_device()
*	@return SHAKE hardware revision number (format is x.yz), or 0.0 on error. */
SHAKE_API float shake_info_hardware_revision(shake_device* sh);

/**	Query SHAKE serial number.
*	@param sh pointer to a shake_device structure as returned by shake_init_device()
*	@return pointer to a null terminated string containing the SHAKE serial number. Format is
*	SK6-xxxx, eg SK6-0003. Do not modify or attempt to deallocate the returned string. 
*	On error, NULL is returned. */
SHAKE_API char* shake_info_serial_number(shake_device* sh);

/**	Query SHAKE Bluetooth firmware revision (SK7 only)
*	@param sh pointer to a shake_device structure as returned by shake_init_device()
*	@return SHAKE hardware revision number (format is x.yz), or 0.0 on error. */
SHAKE_API float shake_info_bluetooth_firmware_revision(shake_device* sh);

/**	Query SHAKE slot1 expansion module
*	@param sh pointer to a shake_device structure as returned by shake_init_device()
*	@return one of the values in the ::shake_optional_modules enumeration, indicating the type of module installed 
*	On error, SHAKE_ERROR is returned */
SHAKE_API int shake_info_module_slot1(shake_device* sh);

/**	Query SHAKE slot2 expansion module
*	@param sh pointer to a shake_device structure as returned by shake_init_device()
*	@return one of the values in the ::shake_optional_modules enumeration, indicating the type of module installed 
*	On error, SHAKE_ERROR is returned */
SHAKE_API int shake_info_module_slot2(shake_device* sh);

/**	Query SHAKE expansion module for a particular slot. For the SK6, valid values are 0-1. For the SK7, 0-3.
*	@param sh pointer to a shake_device structure as returned by shake_init_device()
*	@return one of the values in the ::sk6_optional_modules or ::sk7_optional_modules enumeration, indicating the type of module installed 
*	On error, SHAKE_ERROR is returned */
SHAKE_API int shake_info_module(shake_device* sh, int slot_number);

/**	Returns a string name for a SHAKE expansion module.
*	@param module a value returned from shake_info_module_slot1() or shake_info_module_slot2
*	@return pointer to a string containing the name of the specified module, eg "RFID module" */
SHAKE_API char* shake_info_module_name(int module);

/**	Requests device specific information from the connected SHAKE, including firmware and hardware versions plus
*	the serial number. After successfully calling this function, you can retrieve the information about the device
*	using the other functions with prefix shake_info, eg shake_info_firmware_revision(). You can also call these
*	functions directly, this function will be called internally if necessary. 
*
*	@param sh pointer to a shake_device structure as returned by shake_init_device()
*	@return SHAKE_SUCCESS or SHAKE_ERROR */
SHAKE_API int shake_info_retrieve(shake_device* sh);

/**	Reset various registers on the SHAKE to their default values. If a SHAKE has stopped outputting sensor data
*	or is otherwise behaving abnormally, using this function may solve the problem. It may need to be called several
*	times to take effect completely.
*
*	@param sh pointer to a shake_device structure as returned by shake_init_device()
*	@param repeat if >0, the reset process will be repeated <repeat> times
*	@return SHAKE_SUCCESS or SHAKE_ERROR */
SHAKE_API int shake_factory_reset(shake_device* sh, int repeat);

/** Allows you to register a callback function for receiving events from the SHAKE.
*	The callback function should have the signature:
*	
*	void callback(shake_device* dev, int event)
*
*	The event parameter will be one of the ::shake_events constants indicating the type of event, 
*	and the dev parameter will be the shake_device object that received the event.
*	To unregister the callback, call the function again with a NULL parameter. 
*
*	@param sh pointer to a shake_device structure as returned by shake_init_device()
*	@param callback pointer to user specified callback function
*
*	@return SHAKE_SUCCESS or SHAKE_ERROR */
SHAKE_API int shake_register_event_callback(shake_device* sh, void (SHAKE_CALLBACK *callback)(shake_device*, int));

/**	Allows you to control whether the driver will wait for acknowledgement packets when sending
*	commands to the SHAKE. The default is to enable this functionality as it can be used to confirm
*	whether a command succeeded or not. Register read commands will always wait for acknowledgements
*	since the acknowledgement contains the contents of the requested register.
*
*	@param sh pointer to a shake_device structure as returned by shake_init_device()
*	@param wait_for_ack zero to disable waiting for ACKs, nonzero to enable 
*
*	@return */
SHAKE_API void shake_wait_for_acks(shake_device* sh, int wait_for_ack);

/**	Allows you to override the LED colour of the SHAKE (SK7 only!)
*
*	@param sh pointer to a shake_device structure as returned by shake_init_device()	
*	@param red red value (1-255)
*	@param green green value (1-255)
*	@param blue blue (1-255)
*
*	@return SHAKE_SUCCESS or SHAKE_ERROR */
SHAKE_API int sk7_override_led(shake_device* sh, unsigned char red, unsigned char green, unsigned char blue);

#ifdef _WIN32
/** Identical to the shake_register_event_callback() function in every way, except that it accepts a pointer to
*	a callback function using the stdcall calling convention, while shake_register_event_callback requires a cdecl
*	function pointer. 
*
*	While both callbacks can be registered separately, in virtually all cases only one should be required.
*
*	@param sh pointer to a shake_device structure as returned by shake_init_device()
*	@param callback pointer to user specified callback function (with stdcall calling convention)
*
*	@return SHAKE_SUCCESS or SHAKE_ERROR */
SHAKE_API int shake_register_event_callback_STDCALL(shake_device* sh, void (SHAKE_STDCALL_CALLBACK *callback)(shake_device*, int));
#endif

/* === SHAKE audio functions === */

/*	Allows you to register a callback function for receiving audio data from SHAKEs with modified hardware.
*	The callback function should have the signature:
*
*	void callback(shake_device* dev, short* mic_samples, int mic_length, short* playback_samples, int playback_length);
*
*	The 'dev' parameter will be the shake_device* object that received the audio data. The 'mic_samples' parameter
*	will be a pointer to the latest set of audio samples, while the 'mic_length' parameter gives the number
*	of SAMPLES (ie not the number of bytes). The samples are provided in 16-bit signed PCM format, and can
*	be played back or written to a .wav file directly without any further conversion. The 'playback_samples' parameter
*	will be a pointer to a buffer where you can optionally place a new set of audio data to be played back on the SHAKE.
*	The 'playback_length' parameter gives the number of samples to write (again, not bytes). The samples should be 16-bit
*	signed PCM.
*
*	@param sh pointer to a shake_device structure as returned by shake_init_device()
*	@param callback pointer to user specified callback function
*
*	@return SHAKE_SUCCESS or SHAKE_ERROR */
SHAKE_API int shake_register_audio_callback(shake_device* sh, void (SHAKE_CALLBACK *callback)(shake_device*, short*, int, short*, int));

#ifdef _WIN32
/** Identical to the shake_register_audio_callback() function in every way, except that it accepts a pointer to
*	a callback function using the stdcall calling convention, while shake_register_audio_callback requires a cdecl
*	function pointer. 
*
*	While both callbacks can be registered separately, in virtually all cases only one should be required.
*
*	@param sh pointer to a shake_device structure as returned by shake_init_device()
*	@param callback pointer to user specified callback function (with stdcall calling convention)
*
*	@return SHAKE_SUCCESS or SHAKE_ERROR */
SHAKE_API int shake_register_audio_callback_STDCALL(shake_device* sh, void (SHAKE_STDCALL_CALLBACK *callback)(shake_device*, short*, int, short*, int));
#endif

/**	Uploads an audio sample to the internal memory of the SHAKE.
*	NOTE: this functionality shares memory with data logging, do not use both at the same time!
*	
*	The SHAKE has 8000 "pages" of memory for audio, each containing 1056 samples.
*	Once uploaded successfully, samples can be played back using the shake_play_audio_sample() function.
*
*	@param sh pointer to a shake_device structure as returned by shake_init_device()
*	@param address the page number to upload the sample to (valid range is SHAKE_UPLOAD_MIN_PAGE to SHAKE_UPLOAD_MAX_PAGE)
*	@param sample_data pointer to the audio data in 16 bit signed format. The data will be compressed to an 8-bit format
*	before transmission. 
*	@param sample_len length of the audio data in samples (not bytes). If this is less than SHAKE_AUDIO_PAGE_SIZE, the remainder
*	of the sample will be filled with zeroes.
*	
*	@return SHAKE_SUCCESS or SHAKE_ERROR */
SHAKE_API int shake_upload_audio_sample(shake_device* sh, unsigned short address, short* sample_data, unsigned short sample_len);

/**	Plays back an audio sample from the SHAKEs internal memory. The sample can span multiple memory "pages".
*
*	Use the shake_write_audio_config() function to enable sample playback before using this function.
*
*	@param sh pointer to a shake_device structure as returned by shake_init_device()
*	@param start_address starting page of the audio sample
*	@param end_address ending page (inclusive) of the audio sample
*	@param amplitude amplitude for audio playback, valid range is SHAKE_AUDIO_AMPLTIUDE_MIN to SHAKE_AUDIO_AMPLITUDE_MAX inclusive
*
*	@return SHAKE_SUCCESS or SHAKE_ERROR */
SHAKE_API int shake_play_audio_sample(shake_device* sh, unsigned short start_address, unsigned short end_address, unsigned short amplitude);

/* === SHAKE Expansion Module functions === */

/**	Uploads a vibration sample to the internal memory of the SHAKE.
*	NOTE: this functionality shares memory with data logging, do not use both at the same time!
*	Only available on SHAKEs with expansion module E01
*	
*	The SHAKE has 8000 "pages" of memory for vibration data, each containing 1056 samples.
*	Once uploaded successfully, samples can be played back using the shake_exp_play_vib_sample() function.
*
*	@param sh pointer to a shake_device structure as returned by shake_init_device()
*	@param address the page number to upload the sample to (valid range is SHAKE_UPLOAD_MIN_PAGE to SHAKE_UPLOAD_MAX_PAGE)
*	@param sample_data pointer to the vibration data in linear 8-bit 2's complement signed format.
*	@param sample_len length of the audio data in samples (not bytes). If this is less than SHAKE_UPLOAD_PAGE_SIZE, the remainder
*	of the sample will be filled with zeroes.
*	
*	@return SHAKE_SUCCESS or SHAKE_ERROR */
SHAKE_API int shake_exp_upload_vib_sample(shake_device* sh, unsigned short address, char* sample_data, unsigned short sample_len);

/**	Plays back a vibration sample from the SHAKEs internal memory. The sample can span multiple memory "pages".
*	Only available on SHAKEs with expansion module E01
*
*	@param sh pointer to a shake_device structure as returned by shake_init_device()
*	@param start_address starting page of the audio sample
*	@param end_address ending page (inclusive) of the audio sample
*	@param amplitude amplitude for audio playback, valid range is SHAKE_AUDIO_AMPLTIUDE_MIN to SHAKE_AUDIO_AMPLITUDE_MAX inclusive
*
*	@return SHAKE_SUCCESS or SHAKE_ERROR */
SHAKE_API int shake_exp_play_vib_sample(shake_device* sh, unsigned short start_address, unsigned short end_address, unsigned short amplitude);

/** Writes a value into one of the 6 general purpose registers available on SHAKEs with the E01 module installed.
*	These can be used to power LEDs or similar devices.
*	Only available on SHAKEs with expansion module E01
*
*	Valid values for each register range from 0-255, representing an output current of 0mA-10mA.
*
*	@param sh pointer to a shake_device structure as returned by shake_init_device()
*	@param reg_number the register to update, must be 0-5
*	@param value the value to write into the selected register
*	@return SHAKE_SUCCESS or SHAKE_ERROR */
SHAKE_API int shake_exp_write_gp_register(shake_device* sh, int reg_number, unsigned value);



/* === SHAKE data access functions === 
*	Each of these returns one or more sensor
*	readings from the given SHAKE device. Every function takes a pointer to
*	a shake_device structure as returned by shake_init_device. */

/**	Read current x-axis accelerometer value.
*	@param sh pointer to a shake_device structure as returned by shake_init_device()
*	@return current x-axis accelerometer value */
SHAKE_API int shake_accx(shake_device* sh);

/**	Read current y-axis accelerometer value.
*	@param sh pointer to a shake_device structure as returned by shake_init_device()
*	@return current y-axis accelerometer value */
SHAKE_API int shake_accy(shake_device* sh);

/**	Read current z-axis accelerometer value.
*	@param sh pointer to a shake_device structure as returned by shake_init_device()
*	@return current z-axis accelerometer value */
SHAKE_API int shake_accz(shake_device* sh);

/**	Read current accelerometer values.
*	@param sh pointer to a shake_device structure as returned by shake_init_device()
*	@param xyz pointer to a 3 element array of integers, into which the 3 accelerometer readings will
*	be placed (in x,y,z order).
*	@return SHAKE_SUCCESS, or SHAKE_ERROR if either parameter is NULL */
SHAKE_API int shake_acc(shake_device* sh, int* xyz);

/**	Read current x-axis gyroscope value.
*	@param sh pointer to a shake_device structure as returned by shake_init_device()
*	@return current x-axis gyroscope value */
SHAKE_API int shake_gyrx(shake_device* sh);

/**	Read current y-axis gyroscope value.
*	@param sh pointer to a shake_device structure as returned by shake_init_device()
*	@return current y-axis gyroscope value */
SHAKE_API int shake_gyry(shake_device* sh);

/**	Read current z-axis gyroscope value.
*	@param sh pointer to a shake_device structure as returned by shake_init_device()
*	@return current z-axis gyroscope value */
SHAKE_API int shake_gyrz(shake_device* sh);

/**	Read current gyroscope values.
*	@param sh pointer to a shake_device structure as returned by shake_init_device()
*	@param xyz pointer to a 3 element array of integers, into which the 3 gyroscope readings will
*	be placed (in x,y,z order).
*	@return SHAKE_SUCCESS, or SHAKE_ERROR if either parameter is NULL */
SHAKE_API int shake_gyr(shake_device* sh, int* xyz);

/**	Read current x-axis magnetometer value.
*	@param sh pointer to a shake_device structure as returned by shake_init_device()
*	@return current x-axis magnetometer value */
SHAKE_API int shake_magx(shake_device* sh);

/**	Read current y-axis magnetometer value.
*	@param sh pointer to a shake_device structure as returned by shake_init_device()
*	@return current y-axis magnetometer value */
SHAKE_API int shake_magy(shake_device* sh);

/**	Read current z-axis magnetometer value.
*	@param sh pointer to a shake_device structure as returned by shake_init_device()
*	@return current z-axis magnetometer value */
SHAKE_API int shake_magz(shake_device* sh);

/**	Read current magnetometer values.
*	@param sh pointer to a shake_device structure as returned by shake_init_device()
*	@param xyz pointer to a 3 element array of integers, into which the 3 magnetometer readings will
*	be placed (in x,y,z order).
*	@return SHAKE_SUCCESS, or SHAKE_ERROR if either parameter is NULL */
SHAKE_API int shake_mag(shake_device* sh, int* xyz);

/** Read current compass heading value.
*	@param sh pointer to a shake_device structure as returned by shake_init_device()
*	@return A value are in the range 0-3599 (tenths of a degree), SHAKE_ERROR otherwise */
SHAKE_API int shake_heading(shake_device* sh);

/**	SK7 only. Read current roll-pitch-heading values (requires the roll-pitch-heading packet output
*	to be enabled first, see sk7_configure_roll_pitch_heading). If legacy heading packet output is
*	enabled, use shake_heading to retrieve that data.
*	
*	@param sh pointer to a shake_device structure as returned by shake_init_device()
*	@param rph pointer to a 3 element array of integers, into which the roll, pitch and heading readings
*			will be placed (in that order). See the user manual for details of the format of each value.
*	@return SHAKE_SUCCESS or SHAKE_ERROR */
SHAKE_API int sk7_roll_pitch_heading(shake_device* sh, int* rph);

/**	SK7 only. Read current roll-pitch-heading values when in quaternion output mode. 
*	(requires the roll-pitch-heading packet output
*	to be enabled first, see sk7_configure_roll_pitch_heading). If legacy heading packet output is
*	enabled, use shake_heading to retrieve that data.
*	
*	@param sh pointer to a shake_device structure as returned by shake_init_device()
*	@param rph pointer to a 4 element array of floats, into which the quaternions will be placed. 
*										See the user manual for details of the format of each value.
*	@return SHAKE_SUCCESS or SHAKE_ERROR */
SHAKE_API int sk7_roll_pitch_heading_quaternions(shake_device* sh, float* rphq);

/** Read the proximity value of the first capacitive sensor.
*	@param sh pointer to a shake_device structure as returned by shake_init_device()
*	@return A value in the range 0-255 (255 is highest proximity), SHAKE_ERROR otherwise */
SHAKE_API int sk6_cap0(shake_device* sh);

/** Read the proximity value of the second capacitive sensor.
*	@param sh pointer to a shake_device structure as returned by shake_init_device()
*	@return A value in the range 0-255 (255 is highest proximity), SHAKE_ERROR otherwise */
SHAKE_API int sk6_cap1(shake_device* sh);

/** Read the proximity values of both capacitive sensors on the SK6
*	@param sh pointer to a shake_device structure as returned by shake_init_device()
*	@param proxboth pointer to a 2 element int array, into which the proximity values will be placed.
*	in the order cap0, cap1.
*	@return SHAKE_SUCCESS or SHAKE_ERROR */
SHAKE_API int sk6_cap(shake_device* sh, int* proxboth);

/** Read the proximity values of all 12 capacitive sensors on the SK7
*	@param sh pointer to a shake_device structure as returned by shake_init_device()
*	@param prox pointer to a 12 element int array, into which the proximity values will be placed.
*	@return SHAKE_SUCCESS or SHAKE_ERROR */
SHAKE_API int sk7_cap(shake_device* sh, int* prox);

/**	Read capacitive data from one or both blocks on an SK7 external capactive device. 
*	@param sh pointer to a shake_device structure as returned by shake_init_device()
*	@param prox pointer to a 12 element int array, into which the proximity values will be placed.
*	@param blocks the blocks to return data for;	0 for 1st block only (prox array must be >= 12 elements long),
*													1 for 2nd block only (prox array must be >= 12 elements long),
*													2 for both blocks (prox array must be >= 24 elements long).
*	@param prox pointer to an array into which the proximity values will be placed (note length requirements above!)
*	@return SHAKE_SUCCESS or SHAKE_ERROR */
SHAKE_API int sk7_cap_ext(shake_device* sh, int blocks, int* prox);

/**	Read the current value of the first analog input.
*	@param sh pointer to a shake_device structure as returned by shake_init_device()
*	@return a value in the range 0-2500 (mV), SHAKE_ERROR on error */
SHAKE_API int shake_analog0(shake_device* sh);

/**	Read the current value of the second analog input.
*	@param sh pointer to a shake_device structure as returned by shake_init_device()
*	@return a value in the range 0-2500 (mV), SHAKE_ERROR on error */
SHAKE_API int shake_analog1(shake_device* sh);

/**	Read the current values of both analog inputs.
*	@param sh pointer to a shake_device structure as returned by shake_init_device()
*	@param a0a1 pointer to a 2 element int array, into which the two values will be placed, 
*	in the order analog0, analog1.
*	@return SHAKE_SUCCESS or SHAKE_ERROR */
SHAKE_API int shake_analog(shake_device* sh, int* a0a1);

/**	Reads the peak acceleration value for the last shaking event detected by the onboard
*	shake detection algorithm. You should normally call this function from your SHAKE event
*	callback function, since the values will be updated when a SHAKE_SHAKING_EVENT is received.
*
*	@param sh pointer to a shake_device structure as returned by shake_init_device()
*	@return the peak acceleration value in 0.1g units */
SHAKE_API int shake_shaking_peakaccel(shake_device* sh);

/**	Reads the direction value for the last shaking event detected by the onboard
*	shake detection algorithm. You should normally call this function from your SHAKE event
*	callback function, since the values will be updated when a SHAKE_SHAKING_EVENT is received.
*
*	Possible return values are 0 (direction detection is disabled), 1 (x-axis), 2 (y-axis) and
*	3 (z-axis).
*
*	@param sh pointer to a shake_device structure as returned by shake_init_device()
*	@return the direction value, SHAKE_ERROR on error */
SHAKE_API int shake_shaking_direction(shake_device* sh);

/**	Reads the timestamp value for the last shaking event detected by the onboard
*	shake detection algorithm. You should normally call this function from your SHAKE event
*	callback function, since the values will be updated when a SHAKE_SHAKING_EVENT is received.
*
*	@param sh pointer to a shake_device structure as returned by shake_init_device()
*	@return the time modulo 1 second in units of 1/1024 second that the event was detected */
SHAKE_API int shake_shaking_timestamp(shake_device* sh);

/** Reads the current heart rate BPM value from the heart rate monitor (not installed in all SHAKEs!)
*	You should normally call this function from your SHAKE event callback function, since the value
*	will be updated when a SHAKE_HEART_RATE_EVENT is received.
*
*	@param sh pointer to a shake_device structure as returned by shake_init_device()
*	@return the current heart rate in beats per minute */
SHAKE_API int shake_heart_rate(shake_device* sh);

/** Reads the last detected RFID TID tag for RFID-enabled SHAKEs. Normally you should call this function
*	from your SHAKE event callback, since the tag is only updated when a SHAKE_RFID_TID_EVENT is received.
*
*	The returned value will be a pointer to a 16 byte ASCII hex string containing the tag. It will be overwritten
*	by subsequent tags, so applications should store a copy themselves if they wish to keep a history.
*
*	@param sh pointer to a shake_device structure as returned by shake_init_device()
*	@return the last detected RFID TID tag */
SHAKE_API char* shake_rfid_tid(shake_device* sh);

/**	Triggers an RFID scan by writing to the VO_REG_RFID_SCAN register. You can use this to start a
*	scan when RFID autoscanning is disabled.
*
*	@param sh pointer to a shake_device structure as returned by shake_init_device()
*	@return SHAKE_SUCCESS or SHAKE_ERROR */
SHAKE_API int shake_rfid_scan(shake_device* sh);

/**	Retrieves temperatures of gyro (and in binary output mode also accelerometer) sensors.
*
*	@param sh pointer to a shake_device structure as returned by shake_init_device()
*	@param temps pointer to a 4 element array. On return this will be populated with the current temperatures of
*				the gyro sensors in the following order: pitch, roll, yaw. Additionally if the SK7 is in binary
*				output mode, the 4th element will contain the temperature of the accelerometer sensor (this is not
*				available in ASCII output mode).
*	@return SHAKE_SUCCESS or SHAKE_ERROR */
SHAKE_API int shake_gyro_temperatures(shake_device* sh, float* temps);

/**	Each SHAKE data packet contains a sequence number in the range 0-99. This function can be used to retrieve
*	the sequence number from the last accessed packet for a particular sensor. For example, if the accelerometer
*	sensor is selected, the value returned will be the sequence number of the current accelerometer packet at
*	the time of the last call to one of the shake_acc() group of functions.
*
*	@param sh pointer to a shake_device structure as returned by shake_init_device()
*	@param sensor the sensor to read the sequence number for
*	@return the last sequence number for the selected sensor*/
SHAKE_API int shake_data_timestamp(shake_device* sh, int sensor);

/* 	=== Data logging functions === 
*	These functions allow you to control the SHAKE data logging functionality found in firmware 2.00 and later */

/** Initiates playback of recorded data over the serial link.
*	Playback will start from the current position in memory, call shake_logging_stop() first to read from the start. 
*	
*	To detect the end of the playback process, use shake_register_event_callback() to register a callback function, 
*	and wait for the SHAKE_PLAYBACK_COMPLETE event to be triggered.
*	
*	@param sh pointer to a shake_device structure as returned by shake_init_device()
*	@param output_filename (optional path) and filename to write the log data into.
*	@return SHAKE_SUCCESS or SHAKE_ERROR */
SHAKE_API int shake_logging_play(shake_device* sh, char* output_filename);

/**	If recording or playback active, pauses the process at the current write/read position in memory.
*	To resume a paused operation, simply call shake_logging_record() or shake_logging_play() as appropriate.
*
*	@param sh pointer to a shake_device structure as returned by shake_init_device()
*	@return SHAKE_SUCCESS or SHAKE_ERROR */
SHAKE_API int shake_logging_pause(shake_device* sh);

/** Stops recording or playback AND resets the write/read position to the start of internal memory.
*	This command will also close any output logfile currently open. 
*
*	You should usually call this function before calling either shake_logging_play() or shake_logging_record(), 
*	to reset the write/read position. 
*
*	@param sh pointer to a shake_device structure as returned by shake_init_device()
*	@return SHAKE_SUCCESS or SHAKE_ERROR */
SHAKE_API int shake_logging_stop(shake_device* sh);

/**	Starts logging data into internal memory from the current writing position. 
*	Call shake_logging_stop() first to reset the writing position to the start of internal memory. 
*
*	Note that while recording in progress, changes to the SHAKE configuration registers (using the register access functions)
*	will be disallowed. If you need to change sample rates etc mid-recording, call shake_logging_pause(), change
*	the registers, then resume logging by calling shake_logging_record() again
*	@param sh pointer to a shake_device structure as returned by shake_init_device()
*	@return SHAKE_SUCCESS or SHAKE_ERROR */
SHAKE_API int shake_logging_record(shake_device* sh);

/** Resets the data logging timestamp clock to zero.
*	Should usually only be sent prior to a shake_logging_record command.
*
*	@param sh pointer to a shake_device structure as returned by shake_init_device()
*	@return SHAKE_SUCCESS or SHAKE_ERROR */
SHAKE_API int shake_logging_reset(shake_device* sh);

/** Indicates the current status of data logging on the SHAKE. 
*	The value returned will be a bitwise combination of these three values:
*	- SHAKE_LOGGING_RECORDING		(recording is in progress)
*	- SHAKE_LOGGING_PLAYING		(playback is in progress)
*	- SHAKE_LOGGING_MEMORYFULL	(memory is almost full, <1% available)
*
*	Obviously only 1 of the RECORDING/PLAYING values will be set at any particular time. 
*
*	@param sh pointer to a shake_device structure as returned by shake_init_device()
*	@return SHAKE_SUCCESS or SHAKE_ERROR */
SHAKE_API int shake_logging_status(shake_device* sh);

/** Indicates the proportion of internal memory remaining free for logged data.
*	The value returned will be between 0 and 255, with 0 meaning no space remaining, and 255
*	meaning the whole memory is free. 
*
*	@warning Currently doesn't work properly, may always report 100% full.
*
*	@param sh pointer to a shake_device structure as returned by shake_init_device()
*	@return SHAKE_SUCCESS or SHAKE_ERROR */
SHAKE_API int shake_logging_memory_status(shake_device* sh);

/** Returns the approximate number of packets logged to internal memory. 
*	@param sh pointer to a shake_device structure as returned by shake_init_device()
*	@return The number of packets logged (only accurate to nearest 100), or SHAKE_ERROR */
SHAKE_API int shake_logging_packet_count(shake_device* sh);

/** During playback of logged data, this function can be used to count incoming packets.
*	Unlike the shake_logging_packet_count() function, which indicates the total number of packets stored in
*	the SHAKE itself, this function queries a local value which gives the number of packets received from
*	the SHAKE. This value can then be used to estimate overall progress when taken along with the result 
*	of the shake_logging_packet_count() function.
*
*	@param sh pointer to a shake_device structure as returned by shake_init_device()
*	@return SHAKE_SUCCESS or SHAKE_ERROR */
SHAKE_API int shake_logging_packets_read(shake_device* sh);

/**	The SHAKE SK7 can optionally power down the Bluetooth module while logging is active to increase 
*	battery life. To power down the Bluetooth module, simply call this function. The module will remain
*	inactive until the SK7 is reconnected using a USB cable. 
*
*	@param sh pointer to a shake_device structure as returned by shake_init_device()
*	@return SHAKE_SUCCESS or SHAKE_ERROR */
SHAKE_API int sk7_logging_bt_power_down(shake_device* sh);

/*	=== Register access functions === 
*	These functions allow you to easily get/set the values of the various configuration registers
*	on a SHAKE device */

/** Read the SHAKE_NV_REG_POWER1 register, which controls power to the various sensors on the device. 
*	The value returned will be a combination of the values from the ::shake_power1 enumeration.
*	@param sh pointer to a shake_device structure as returned by shake_init_device()
*	@param value pointer to an unsigned char variable which will receive the contents of the register 
*	@return SHAKE_SUCCESS or SHAKE_ERROR */
SHAKE_API int shake_read_power_state(shake_device* sh, unsigned char* value);

/** Write the SHAKE_NV_REG_POWER1 register, which controls power to the various sensors on the device. 
*	@param sh pointer to a shake_device structure as returned by shake_init_device()
*	@param value to write into the register (should be a combination of the values from the ::shake_power1 enumeration)
*		For example, to power on the accelerometer and magnetometer (disabling everything else), the value should be:
*		SHAKE_POWER_ACC | SHAKE_POWER_MAG
*	@return SHAKE_SUCCESS or SHAKE_ERROR */
SHAKE_API int shake_write_power_state(shake_device* sh, unsigned char value);

/** Read the SHAKE_NV_REG_POWER2 register, which enables/disables capacitive switch thresholds and event output.
*	The value returned will be a combination of the values from the ::shake_power2 enumeration.
*	@param sh pointer to a shake_device structure as returned by shake_init_device()
*	@param value pointer to an unsigned char variable which will receive the contents of the register 
*	@return SHAKE_SUCCESS or SHAKE_ERROR */
SHAKE_API int shake_read_power_state_2(shake_device* sh, unsigned char* value);

/** Write the SHAKE_NV_REG_POWER2 register, which enables/disables capacitive switch thresholds and event output.
*	@param sh pointer to a shake_device structure as returned by shake_init_device()
*	@param value to write into the register (should be a combination of the values from the ::shake_power2 enumeration)
*		For example, to enable upper and lower thresholds for cap switch 0, plus event packet output, the value
*		would be SHAKE_POWER_CS0_INC | SHAKE_POWER_CS0_DEC | SHAKE_POWER_CS_EVENTS
*	@return SHAKE_SUCCESS or SHAKE_ERROR */
SHAKE_API int shake_write_power_state_2(shake_device* sh, unsigned char value);

/** Read the SHAKE_NV_REG_ACC_CONFIG register, which configures a couple of aspects of the accelerometer.
*	The first bit in the returned value indicates the accelerometer range:
*	- if 0, the range is +/- 2g
*	- if 1, the range is +/- 6g
*
*	If set, the 2nd bit indicates the high pass filter is enabled, if unset the filter is disabled.
*
*	The remaining bits are not currently used.
*	@param sh pointer to a shake_device structure as returned by shake_init_device()
*	@param value pointer to an unsigned char variable which will receive the contents of the register 
*	@return SHAKE_SUCCESS or SHAKE_ERROR */
SHAKE_API int shake_read_acc_config(shake_device* sh, unsigned char* value);

/** Write the SHAKE_NV_REG_ACC_CONFIG register, which configures a couple of aspects of the accelerometer.
*	The first bit of \a value controls the accelerometer range:
*		- if 0, the range is +/- 2g
*		- if 1, the range is +/- 6g
*
*		If set, the 2nd bit enabless the high pass filter. If unset the filter is disabled.
*
*		The remaining bits are not currently used.
*	@param sh pointer to a shake_device structure as returned by shake_init_device()
*	@param value the value to write into the register.
*	@return SHAKE_SUCCESS or SHAKE_ERROR */
SHAKE_API int shake_write_acc_config(shake_device* sh, unsigned char value);

/** Read the SHAKE_NV_REG_DATAFMT register, which configures the output data format from the SHAKE.
*	If set, the first bit of the returned value indicates that checksums will be attached to ASCII data packets.
*
*	If set, the second bit indicates that output will be in raw (binary) mode, if not set, output is ASCII.
*
*	The remaining bits are not currently used.
*	@param sh pointer to a shake_device structure as returned by shake_init_device()
*	@param value pointer to an unsigned char variable which will receive the contents of the register 
*	@return SHAKE_SUCCESS or SHAKE_ERROR */
SHAKE_API int shake_read_data_format(shake_device* sh, unsigned char* value);

/** Write the SHAKE_NV_REG_DATAFMT register, which configures the output data format from the SHAKE.
*	If set, the first bit of \a value enables checksums for ASCII data packets. If not set, checksums are disabled.
*
*	If set, the second bit indicates that output will be in raw (binary) mode. If not set, output is ASCII.
*
*	The remaining bits are not currently used.
*	@param sh pointer to a shake_device structure as returned by shake_init_device()
*	@param value the value to write into the register
*	@return SHAKE_SUCCESS or SHAKE_ERROR */
SHAKE_API int shake_write_data_format(shake_device* sh, unsigned char value);

/** Read the SHAKE_NV_REG_CALIB_BYPASS register, which can be used to obtain uncalibrated data from selected sensors.
*	The value returned will be a combination of the values from the ::shake_bypass_sensor_calib enumeration. If 
*	the bit for a particular sensor is set, it means uncalibrated output is enabled.
*	@param sh pointer to a shake_device structure as returned by shake_init_device()
*	@param value pointer to an unsigned char variable which will receive the contents of the register 
*	@return SHAKE_SUCCESS or SHAKE_ERROR */
SHAKE_API int shake_read_calib_bypass(shake_device* sh, unsigned char* value);

/** Write the SHAKE_NV_REG_CALIB_BYPASS register, which can be used to obtain uncalibrated data from selected sensors.
*	The value to write should be a combination of the values from the ::shake_bypass_sensor_calib enumeration. If 
*	the bit for a particular sensor is set, it means uncalibrated output will be enabled.
*	@param sh pointer to a shake_device structure as returned by shake_init_device()
*	@param value the value to write into the register
*	@return SHAKE_SUCCESS or SHAKE_ERROR */
SHAKE_API int shake_write_calib_bypass(shake_device* sh, unsigned char value);

/** Read the SHAKE_NV_REG_CX_CALIB_BYPASS register, which can be used to disable the cross axes calibration for
*	sensors with multiple degrees of freedom (accelerometer, gyro, magnetometer).
*	The value returned will be a combination of the values from the ::shake_cx_bypass_sensor_calib enumeration. If 
*	the bit for a particular sensor is set, it means uncalibrated output is enabled.
*	@param sh pointer to a shake_device structure as returned by shake_init_device()
*	@param value pointer to an unsigned char variable which will receive the contents of the register 
*	@return SHAKE_SUCCESS or SHAKE_ERROR */
SHAKE_API int shake_read_cx_bypass(shake_device* sh, unsigned char* value);

/** Write the SHAKE_NV_REG_CX_CALIB_BYPASS register, which can be used to disable the cross axes calibration for
*	sensors with multiple degrees of freedom (accelerometer, gyro, magnetometer).
*	The value to write will be a combination of the values from the ::shake_cx_bypass_sensor_calib enumeration. If 
*	the bit for a particular sensor is set, it means uncalibrated output will be enabled.
*	@param sh pointer to a shake_device structure as returned by shake_init_device()
*	@param value the value to write into the register
*	@return SHAKE_SUCCESS or SHAKE_ERROR */
SHAKE_API int shake_write_cx_bypass(shake_device* sh, unsigned char value);

/** Read the SHAKE_NV_REG_TEMP_COMPENSATION register, which indicates the state of temperature compensation for sensor output.
*	Currently only gyroscope compensation is possible
*	@warning This might not be working yet.
*	The 2nd bit in the register is currently the only one used - if set, gyro temperature compensation is enabled.
*	@param sh pointer to a shake_device structure as returned by shake_init_device()
*	@param value pointer to an unsigned char variable which will receive the contents of the register 
*	@return SHAKE_SUCCESS or SHAKE_ERROR */
SHAKE_API int shake_read_temp_compensation(shake_device* sh, unsigned char* value);

/** Write the SHAKE_NV_REG_TEMP_COMPENSATION register, which controls the state of temperature compensation for sensor output.
*	Currently only gyroscope compensation is possible
*	@warning This might not be working yet.
*	The 2nd bit in the register is currently the only one used - if set, gyro temperature compensation is enabled.
*	@param sh pointer to a shake_device structure as returned by shake_init_device()
*	@param value the value to write into the register
*	@return SHAKE_SUCCESS or SHAKE_ERROR */
SHAKE_API int shake_write_temp_compensation(shake_device* sh, unsigned char value);

/** Read the SHAKE_NV_REG_STREAM_DISABLE register, which can be used to disable the streaming of data packets for selected sensors.
*	The value returned will be a combination of the values from the ::shake_stream_disable enumeration. If 
*	the bit for a particular sensor is set, it means data streaming is DISABLED for that sensor (even if sample rate above 0).
*
*	NOTE: this functionality is removed on SK7s with firmware version >= 1.00. Setting the sample rate to 0 has an 
*	equivalent effect.
*	
*	@param sh pointer to a shake_device structure as returned by shake_init_device()
*	@param value pointer to an unsigned char variable which will receive the contents of the register 
*	@return SHAKE_SUCCESS or SHAKE_ERROR */
SHAKE_API int shake_read_packet_streaming(shake_device* sh, unsigned char* value);

/** Write the SHAKE_NV_REG_STREAM_DISABLE register, which can be used to disable the streaming of data packets for selected sensors.
*	The value to write should be a combination of the values from the ::shake_stream_disable enumeration. If 
*	the bit for a particular sensor is set, it means data streaming will be DISABLED for that sensor (even if sample rate above 0).
*
*	NOTE: this functionality is removed on SK7s with firmware version >= 1.00. Setting the sample rate to 0 has an 
*	equivalent effect.
*
*	@param sh pointer to a shake_device structure as returned by shake_init_device()
*	@param value the value to write into the register
*	@return SHAKE_SUCCESS or SHAKE_ERROR */
SHAKE_API int shake_write_packet_streaming(shake_device* sh, unsigned char value);

/** Read the SHAKE_NV_REG_AUDIO_CONFIG register, which can be used to configure the audio functionality on SHAKEs with
*	supported hardware. 
*	The value returned will be a combination of the values from the ::shake_audio_config enumeration.
*	@param sh pointer to a shake_device structure as returned by shake_init_device()
*	@param value pointer to an unsigned char variable which will receive the contents of the register 
*	@return SHAKE_SUCCESS or SHAKE_ERROR */
SHAKE_API int shake_read_audio_config(shake_device* sh, unsigned char* value);

/** Write the SHAKE_NV_REG_AUDIO_CONFIG register, which can be used to configure the audio functionality on SHAKEs with
*	supported hardware. 
*	The value to write should be a combination of the values from the ::shake_audio_config enumeration. 
*	@param sh pointer to a shake_device structure as returned by shake_init_device()
*	@param value the value to write into the register
*	@return SHAKE_SUCCESS or SHAKE_ERROR */
SHAKE_API int shake_write_audio_config(shake_device* sh, unsigned char value);

/** Write the VO_REG_MIDI_AMPLITUDE register, which can be used to configure the ampltiude of the SHAKE
*	MIDI synthesiser. 
*	The value to write should be SHAKE_MIDI_AMPLITUDE_MUTE if the amplitude should be muted, otherwise
*	a value between SHAKE_MID_AMPLITUDE_MIN and SHAKE_MIDI_AMPLITUDE_MAX inclusive.
*	@param sh pointer to a shake_device structure as returned by shake_init_device()
*	@param value the value to write into the register
*	@return SHAKE_SUCCESS or SHAKE_ERROR */
SHAKE_API int shake_write_midi_amplitude(shake_device* sh, unsigned char value);

/** Write the VO_REG_MIDI_NOTE register, which can be used to configure the MIDI note to play from the SHAKE
*	MIDI synthesiser.
*	The value to write should be in the range SHAKE_MIDI_NOTE_MIN and SHAKE_MIDI_NOTE_MAX inclusive.
*	@param sh pointer to a shake_device structure as returned by shake_init_device()
*	@param value the value to write into the register
*	@return SHAKE_SUCCESS or SHAKE_ERROR */
SHAKE_API int shake_write_midi_note(shake_device* sh, unsigned char value);

/** Write the VO_REG_MIDI_WAVEFORM register, which can be used to configure the waveform to use from the SHAKE
*	MIDI synthesiser. 
*	The value to write should be a member of the ::shake_midi_waveforms enumeration. 
*	@param sh pointer to a shake_device structure as returned by shake_init_device()
*	@param value the value to write into the register
*	@return SHAKE_SUCCESS or SHAKE_ERROR */
SHAKE_API int shake_write_midi_waveform(shake_device* sh, unsigned char value);

/** Read the SHAKE_NV_REG_CS0_INC register, which contains the increasing threshold value for capacitive switch 0.
*
*	SK6 only.
*	The value returned will be in the range 0-255 (the same range as the normal output from the sensor).
*	@param sh pointer to a shake_device structure as returned by shake_init_device()
*	@param value pointer to an unsigned char variable which will receive the contents of the register 
*	@return SHAKE_SUCCESS or SHAKE_ERROR */
SHAKE_API int sk6_read_cs0_inc(shake_device* sh, unsigned char* value);

/** Write the SHAKE_NV_REG_CS0_INC register, which contains the increasing threshold value for capacitive switch 0.
*
*	SK6 only.
*	The value for the threshold should be in the range 0-255 (the same range as the normal output from the sensor).
*	@param sh pointer to a shake_device structure as returned by shake_init_device()
*	@param value the threshold value (must be greater than the decreasing threshold!)
*	@return SHAKE_SUCCESS or SHAKE_ERROR 
*	@see shake_write_cs0_dec() */
SHAKE_API int sk6_write_cs0_inc(shake_device* sh, unsigned char value);

/** Read the SHAKE_NV_REG_CS0_DEC register, which contains the decreasing threshold value for capacitive switch 0.
*
*	SK6 only.
*	The value returned will be in the range 0-255 (the same range as the normal output from the sensor).
*	@param sh pointer to a shake_device structure as returned by shake_init_device()
*	@param value pointer to an unsigned char variable which will receive the contents of the register.
*	@return SHAKE_SUCCESS or SHAKE_ERROR */
SHAKE_API int sk6_read_cs0_dec(shake_device* sh, unsigned char* value);

/** Write the SHAKE_NV_REG_CS0_DEC register, which contains the decreasing threshold value for capacitive switch 0.
*
*	SK6 only.
*	The value for the threshold should be in the range 0-255 (the same range as the normal output from the sensor).
*	@param sh pointer to a shake_device structure as returned by shake_init_device()
*	@param value the threshold value (must be less than the increasing threshold!)
*	@return SHAKE_SUCCESS or SHAKE_ERROR
*	@see shake_write_cs0_inc() */
SHAKE_API int sk6_write_cs0_dec(shake_device* sh, unsigned char value);

/** Read the SHAKE_NV_REG_CS0_INC_PROFILE register, which contains the number of the vibration profile to play when
*	the increasing threshold on capacitive switch 0 is triggered.
*
*	SK6 only.
*	The value returned will be in the range 1-255, and indicates the address of the vibration profile uploaded
*	using shake_upload_vib_sample(). If 0, no sample is currently set to play.
*	@param sh pointer to a shake_device structure as returned by shake_init_device()
*	@param value pointer to an unsigned char variable which will receive the contents of the register 
*	@return SHAKE_SUCCESS or SHAKE_ERROR 
*	@see shake_read_power_state_2()
*	@see shake_write_power_state_2() */
SHAKE_API int sk6_read_cs0_inc_profile(shake_device* sh, unsigned char* value);

/** Write the SHAKE_NV_REG_CS0_INC_PROFILE register, which controls the number of the vibration profile to play when
*	the increasing threshold on capacitive switch 0 is triggered.
*
*	SK6 only.
*	The value returned will be in the range 1-255, and indicates the address of the vibration profile uploaded
*	using shake_upload_vib_sample(). If 0, no sample is currently set to play.
*	@param sh pointer to a shake_device structure as returned by shake_init_device()
*	@param value pointer to an unsigned char variable which will receive the contents of the register 
*	@return SHAKE_SUCCESS or SHAKE_ERROR 
*	@see shake_read_power_state_2()
*	@see shake_write_power_state_2() */
SHAKE_API int sk6_write_cs0_inc_profile(shake_device* sh, unsigned char value);

/** Read the SHAKE_NV_REG_CS0_DEC_PROFILE register, which contains the number of the vibration profile to play when
*	the decreasing threshold on capacitive switch 0 is triggered.
*
*	SK6 only.
*	The value returned will be in the range 1-255, and indicates the address of the vibration profile uploaded
*	using shake_upload_vib_sample(). If 0, no sample is currently set to play.
*	@param sh pointer to a shake_device structure as returned by shake_init_device()
*	@param value pointer to an unsigned char variable which will receive the contents of the register 
*	@return SHAKE_SUCCESS or SHAKE_ERROR 
*	@see shake_read_power_state_2()
*	@see shake_write_power_state_2()*/
SHAKE_API int sk6_read_cs0_dec_profile(shake_device* sh, unsigned char* value);

/** Write the SHAKE_NV_REG_CS0_DEC_PROFILE register, which controls the number of the vibration profile to play when
*	the decreasing threshold on capacitive switch 0 is triggered.
*
*	SK6 only.
*	The value returned will be in the range 1-255, and indicates the address of the vibration profile uploaded
*	using shake_upload_vib_sample(). If 0, no sample is currently set to play.
*	@param sh pointer to a shake_device structure as returned by shake_init_device()
*	@param value pointer to an unsigned char variable which will receive the contents of the register 
*	@return SHAKE_SUCCESS or SHAKE_ERROR 
*	@see shake_read_power_state_2()
*	@see shake_write_power_state_2() */
SHAKE_API int sk6_write_cs0_dec_profile(shake_device* sh, unsigned char value);

/** Read the SHAKE_NV_REG_CS1_INC register, which contains the increasing threshold value for capacitive switch 1.
*
*	SK6 only.
*	The value returned will be in the range 0-255 (the same range as the normal output from the sensor).
*	@param sh pointer to a shake_device structure as returned by shake_init_device()
*	@param value pointer to an unsigned char variable which will receive the contents of the register 
*	@return SHAKE_SUCCESS or SHAKE_ERROR */
SHAKE_API int sk6_read_cs1_inc(shake_device* sh, unsigned char* value);

/** Write the SHAKE_NV_REG_CS1_INC register, which contains the decreasing threshold value for capacitive switch 1.
*
*	SK6 only.
*	The value for the threshold should be in the range 0-255 (the same range as the normal output from the sensor).
*	@param sh pointer to a shake_device structure as returned by shake_init_device()
*	@param value the threshold value (must be less than the increasing threshold!)
*	@return SHAKE_SUCCESS or SHAKE_ERROR
*	@see shake_write_cs1_dec() */
SHAKE_API int sk6_write_cs1_inc(shake_device* sh, unsigned char value);

/** Read the SHAKE_NV_REG_CS1_DEC register, which contains the decreasing threshold value for capacitive switch 1.
*
*	SK6 only.
*	The value returned will be in the range 0-255 (the same range as the normal output from the sensor).
*	@param sh pointer to a shake_device structure as returned by shake_init_device()
*	@param value pointer to an unsigned char variable which will receive the contents of the register 
*	@return SHAKE_SUCCESS or SHAKE_ERROR */
SHAKE_API int sk6_read_cs1_dec(shake_device* sh, unsigned char* value);

/** Write the SHAKE_NV_REG_CS1_DEC register, which contains the increasing threshold value for capacitive switch 1.
*
*	SK6 only.
*	The value for the threshold should be in the range 0-255 (the same range as the normal output from the sensor).
*	@param sh pointer to a shake_device structure as returned by shake_init_device()
*	@param value the threshold value (must be less than the increasing threshold!)
*	@return SHAKE_SUCCESS or SHAKE_ERROR
*	@see shake_write_cs1_inc() */
SHAKE_API int sk6_write_cs1_dec(shake_device* sh, unsigned char value);

/** Read the SHAKE_NV_REG_CS1_INC_PROFILE register, which contains the number of the vibration profile to play when
*	the increasing threshold on capacitive switch 1 is triggered.
*
*	SK6 only.
*	The value returned will be in the range 1-255, and indicates the address of the vibration profile uploaded
*	using shake_upload_vib_sample(). If 0, no sample is currently set to play.
*	@param sh pointer to a shake_device structure as returned by shake_init_device()
*	@param value pointer to an unsigned char variable which will receive the contents of the register 
*	@return SHAKE_SUCCESS or SHAKE_ERROR 
*	@see shake_read_power_state_2()
*	@see shake_write_power_state_2()*/
SHAKE_API int sk6_read_cs1_inc_profile(shake_device* sh, unsigned char* value);

/** Write the SHAKE_NV_REG_CS1_INC_PROFILE register, which controls the number of the vibration profile to play when
*	the increasing threshold on capacitive switch 1 is triggered.
*
*	SK6 only.
*	The value returned will be in the range 1-255, and indicates the address of the vibration profile uploaded
*	using shake_upload_vib_sample(). If 0, no sample is currently set to play.
*	@param sh pointer to a shake_device structure as returned by shake_init_device()
*	@param value pointer to an unsigned char variable which will receive the contents of the register 
*	@return SHAKE_SUCCESS or SHAKE_ERROR 
*	@see shake_read_power_state_2()
*	@see shake_write_power_state_2() */
SHAKE_API int sk6_write_cs1_inc_profile(shake_device* sh, unsigned char value);

/** Read the SHAKE_NV_REG_CS1_DEC_PROFILE register, which contains the number of the vibration profile to play when
*	the decreasing threshold on capacitive switch 1 is triggered.
*
*	SK6 only.
*	The value returned will be in the range 1-255, and indicates the address of the vibration profile uploaded
*	using shake_upload_vib_sample(). If 0, no sample is currently set to play.
*	@param sh pointer to a shake_device structure as returned by shake_init_device()
*	@param value pointer to an unsigned char variable which will receive the contents of the register 
*	@return SHAKE_SUCCESS or SHAKE_ERROR 
*	@see shake_read_power_state_2()
*	@see shake_write_power_state_2()*/
SHAKE_API int sk6_read_cs1_dec_profile(shake_device* sh, unsigned char* value);

/** Write the SHAKE_NV_REG_CS1_DEC_PROFILE register, which controls the number of the vibration profile to play when
*	the increasing threshold on capacitive switch 1 is triggered.
*
*	SK6 only.
*	The value returned will be in the range 1-255, and indicates the address of the vibration profile uploaded
*	using shake_upload_vib_sample(). If 0, no sample is currently set to play.
*	@param sh pointer to a shake_device structure as returned by shake_init_device()
*	@param value pointer to an unsigned char variable which will receive the contents of the register 
*	@return SHAKE_SUCCESS or SHAKE_ERROR 
*	@see shake_read_power_state_2()
*	@see shake_write_power_state_2() */
SHAKE_API int sk6_write_cs1_dec_profile(shake_device* sh, unsigned char value);

/**	This function allows you to read all 8 registers related to capacitive switch thresholds in a single operation.
*
*	SK6 only.
*	@param sh pointer to a shake_device structure as returned by shake_init_device()
*	@param values pointer to an 8 element array, which will be populated with the values from each of the 8
*	registers in the following order:
*	- SK6_NV_REG_CS0_INC			(cap switch 0, increasing threshold)
*	- SK6_NV_REG_CS0_DEC			(cap switch 0, decreasing threshold)
*	- SK6_NV_REG_CS0_INC_PROFILE	(cap switch 0, increasing threshold vib profile)
*	- SK6_NV_REG_CS0_DEC_PROFILE	(cap switch 0, decreasing threshold vib profile)
*	- SK6_NV_REG_CS1_INC			(cap switch 1, increasing threshold)
*	- SK6_NV_REG_CS1_DEC			(cap switch 1, decreasing threshold)
*	- SK6_NV_REG_CS1_INC_PROFILE	(cap switch 1, increasing threshold vib profile)
*	- SK6_NV_REG_CS1_DEC_PROFILE	(cap switch 1, decreasing threshold vib profile)
* 
*	@return SHAKE_SUCCESS or SHAKE_ERROR
*/
SHAKE_API int sk6_read_cap_thresholds(shake_device* sh, unsigned char* values);

/**	This function allows you to write all 8 registers related to capacitive switch thresholds in a single operation.
*
*	SK6 only.
*	@param sh pointer to a shake_device structure as returned by shake_init_device()
*	@param values pointer to an 8 element array, which should be populated with the values for each of the 8
*	registers in the following order:
*	- SK6_NV_REG_CS0_INC			(cap switch 0, increasing threshold)
*	- SK6_NV_REG_CS0_DEC			(cap switch 0, decreasing threshold)
*	- SK6_NV_REG_CS0_INC_PROFILE	(cap switch 0, increasing threshold vib profile)
*	- SK6_NV_REG_CS0_DEC_PROFILE	(cap switch 0, decreasing threshold vib profile)
*	- SK6_NV_REG_CS1_INC			(cap switch 1, increasing threshold)
*	- SK6_NV_REG_CS1_DEC			(cap switch 1, decreasing threshold)
*	- SK6_NV_REG_CS1_INC_PROFILE	(cap switch 1, increasing threshold vib profile)
*	- SK6_NV_REG_CS1_DEC_PROFILE	(cap switch 1, decreasing threshold vib profile)
*	
*	@return SHAKE_SUCCESS or SHAKE_ERROR
*/
SHAKE_API int sk6_write_cap_thresholds(shake_device* sh, unsigned char* values);

/**	This function allows you to read all 4 registers related to capacitive switch thresholds in a single operation.
*
*	SK7 only.
*	@param sh pointer to a shake_device structure as returned by shake_init_device()
*	@param values pointer to a 4 element array, which will be populated with the values from each of the 4 relevant
*	registers in the following order:
*	- SK7_NV_REG_CAP_INC			(increasing threshold)
*	- SK7_NV_REG_CAP_DEC			(decreasing threshold)
*	- SK7_NV_REG_CAP_INC_PROFILE	(increasing threshold vib profile)
*	- SK7_NV_REG_CAP_DEC_PROFILE	(decreasing threshold vib profile)
*	@return SHAKE_SUCCESS or SHAKE_ERROR
*/
SHAKE_API int sk7_read_cap_thresholds(shake_device* sh, unsigned char* values);

/**	This function allows you to write all 4 registers related to capacitive switch thresholds in a single operation.
*
*	SK7 only.
*	@param sh pointer to a shake_device structure as returned by shake_init_device()
*	@param values pointer to a 4 element array, which should be populated with the values for each of the 4
*	registers in the following order:
*	- SK7_NV_REG_CAP_INC			(increasing threshold)
*	- SK7_NV_REG_CAP_DEC			(decreasing threshold)
*	- SK7_NV_REG_CAP_INC_PROFILE	(increasing threshold vib profile)
*	- SK7_NV_REG_CAP_DEC_PROFILE	(decreasing threshold vib profile)
*	@return SHAKE_SUCCESS or SHAKE_ERROR
*/
SHAKE_API int sk7_write_cap_thresholds(shake_device* sh, unsigned char* values);

/** Reads the various registers which control the output sample rate for each sensor.
*	Each sensor has a an output sample rate range between 0 and some maximum, which is given by the
*	value of the corresponding constant from the ::shake_max_output_rates enumeration. For example,
*	SHAKE_ACC_MAX_RATE is the maximum accelerometer sample rate. Each sensor also has a dedicated register
*	containing the current output sample rate.
*
*	To get the sample rate for a selected sensor, use the appropriate value from ::shake_sensors as the
*	\a sensor_id parameter (eg SHAKE_SENSOR_ACC for accelerometer).
*
*	@param sh pointer to a shake_device structure as returned by shake_init_device()
*	@param sensor_id sensor to get sample rate for (from ::shake_sensors)
*	@param value pointer to an unsigned char variable which will receive the contents of the register 
*	@return SHAKE_SUCCESS or SHAKE_ERROR */
SHAKE_API int shake_read_sample_rate(shake_device* sh, int sensor_id, unsigned char* value);

/** Write the various registers which control the output sample rate for each sensor.
*	Each sensor has a an output sample rate range between 0 and some maximum, which is given by the
*	value of the corresponding constant from the ::shake_max_output_rates enumeration. For example,
*	SHAKE_ACC_MAX_RATE is the maximum accelerometer sample rate. Each sensor also has a dedicated register
*	containing the current output sample rate.
*
*	To set the sample rate for a selected sensor, use the appropriate value from ::shake_sensors as the
*	\a sensor_id parameter (eg SHAKE_SENSOR_ACC for accelerometer).
*
*	@param sh pointer to a shake_device structure as returned by shake_init_device()
*	@param sensor_id sensor to set sample rate for (from ::shake_sensors)
*	@param value the sample rate to set for the selected sensor
*	@return SHAKE_SUCCESS or SHAKE_ERROR */
SHAKE_API int shake_write_sample_rate(shake_device* sh, int sensor_id, unsigned char value);

/** Reads the various registers which control the digital filter settings for each sensor.
*	The first bit in the returned value indicates filtering is enabled (if set) or disabled (if unset).
*
*	The second bit in the returned value indicates the type of filtering - if set, minimum phase, if unset, linear phase.
*
*	See the user manual for more details on digital filtering.
*
*	@param sh pointer to a shake_device structure as returned by shake_init_device()
*	@param sensor_id sensor to get digital filtering configuration for (from ::shake_sensors)
*	@param value pointer to an unsigned char variable which will receive the contents of the register 
*	@return SHAKE_SUCCESS or SHAKE_ERROR */
SHAKE_API int shake_read_digital_filter(shake_device* sh, int sensor_id, unsigned char* value);

/** Write the various registers which control the digital filter settings for each sensor.
*	The first bit in \a value controls whether filtering is enabled (if set) or disabled (if unset).
*
*	The second bit in \a value controls the type of filtering - if set, minimum phase, if unset, linear phase.
*
*	See the user manual for more details on digital filtering.
*
*	@param sh pointer to a shake_device structure as returned by shake_init_device()
*	@param sensor_id sensor to set digital filtering configuration for (from ::shake_sensors)
*	@param value the value to write into the register
*	@return SHAKE_SUCCESS or SHAKE_ERROR */
SHAKE_API int shake_write_digital_filter(shake_device* sh, int sensor_id, unsigned char value);

/** Read the value of the SHAKE_NV_REG_SHAKING_CONFIG register, which controls the state of the
*	shaking detection algorithm in firmware >= 2.01.
*
*	The value returned by the function will be zero, or a combination of values from the ::shake_shaking_configuration enumeration.
*
*	@param sh pointer to a shake_device structure as returned by shake_init_device()
*	@param value pointer to an unsigned char variable which will receive the contents of the register 
*	@return SHAKE_SUCCESS or SHAKE_ERROR */
SHAKE_API int shake_read_shaking_config(shake_device* sh, unsigned char* value);

/** Write a value into the SHAKE_NV_REG_SHAKING_CONFIG register, which allows you to configure the state of the
*	shaking detection algorithm in firmware >= 2.01.
*
*	Using this register, you can separately enable/disable the shaking detection algorithm, the direction detection 
*	part of the algorithm, and the use of vibrotactile feedback for shaking events.
*
*	The value to write should be zero or a combination of values from the ::shake_shaking_configuration enumeration.
*
*	@param sh pointer to a shake_device structure as returned by shake_init_device()
*	@param value the value to write into the register
*	@return SHAKE_SUCCESS or SHAKE_ERROR
*/
SHAKE_API int shake_write_shaking_config(shake_device* sh, unsigned char value);

/** Reads the value of the SHAKE_NV_REG_SHAKING_ACCEL_THRESHOLD register.
*
*	This register controls the minimum peak acceleration below which shaking events will not be triggered.
*
*	The value returned by the function gives the peak acceleration in 0.1g units, so a value of 10 = 1g.
*
*	@param sh pointer to a shake_device structure as returned by shake_init_device()
*	@param value pointer to an unsigned char variable which will receive the contents of the register 
*	@return SHAKE_SUCCESS or SHAKE_ERROR */
SHAKE_API int shake_read_shaking_accel_threshold(shake_device* sh, unsigned char* value);

/** Writes a value into the SHAKE_NV_REG_SHAKING_ACCEL_THRESHOLD register.
*
*	This register controls the minimum peak acceleration below which shaking events will not be triggered.
*
*	\a value should be the minimum peak acceleration in units of 0.1g (10 = 1g).
*
*	@param sh pointer to a shake_device structure as returned by shake_init_device()
*	@param value the value to write into the register
*	@return SHAKE_SUCCESS or SHAKE_ERROR */
SHAKE_API int shake_write_shaking_accel_threshold(shake_device* sh, unsigned char value);

/** Reads the value of the SHAKE_NV_REG_SHAKING_HOLDOFF_TIME register.
*
*	This register controls the hold-off time that the detection algorithm will wait after an acceleration
*	peak before beginning to look for the next peak.
*
*	The value returned by the function gives the time in units of 7.8ms, so a value of 10 = 78ms.
*
*	@param sh pointer to a shake_device structure as returned by shake_init_device()
*	@param value pointer to an unsigned char variable which will receive the contents of the register 
*	@return SHAKE_SUCCESS or SHAKE_ERROR */
SHAKE_API int shake_read_shaking_holdoff_time(shake_device* sh, unsigned char* value);

/** Writes a value into the SHAKE_NV_REG_SHAKING_HOLDOFF_TIME register.
*
*	This register controls the hold-off time that the detection algorithm will wait after an acceleration
*	peak before beginning to look for the next peak.
*
*	\a value should be the hold-off time in units of 7.8ms (10 = 78ms).
*
*	@param sh pointer to a shake_device structure as returned by shake_init_device()
*	@param value the value to write into the register
*	@return SHAKE_SUCCESS or SHAKE_ERROR */
SHAKE_API int shake_write_shaking_holdoff_time(shake_device* sh, unsigned char value);

/** Reads the value of the SHAKE_NV_REG_SHAKING_VIBRATION_PROFILE register.
*
*	This register controls the vibration profile that will be activated when a shaking event is triggered (assuming
*	the vibration control flag is enabled, see the shake_write_shaking_config() function).
*
*	The value returned by the function gives the address of the vibration profile that will be played.
*
*	@param sh pointer to a shake_device structure as returned by shake_init_device()
*	@param value pointer to an unsigned char variable which will receive the contents of the register 
*	@return SHAKE_SUCCESS or SHAKE_ERROR */
SHAKE_API int shake_read_shaking_vibration_profile(shake_device* sh, unsigned char* value);

/** Writes a value into the SHAKE_NV_REG_SHAKING_VIBRATION_PROFILE register.
*
*	This register controls the vibration profile that will be activated when a shaking event is triggered (assuming
*	the vibration control flag is enabled, see the shake_write_shaking_config() function).
*
*	\a value should be the vibration profile address (between SHAKE_VIB_PROFILE_MIN and SHAKE_VIB_PROFILE_MAX inclusive)
*
*	@param sh pointer to a shake_device structure as returned by shake_init_device()
*	@param value the value to write into the register
*	@return SHAKE_SUCCESS or SHAKE_ERROR */
SHAKE_API int shake_write_shaking_vibration_profile(shake_device* sh, unsigned char value);

/** Reads the value of the SHAKE_NV_REG_SHAKING_HPF_CONSTANT register.
*
*	This register controls the high pass filter length in the detection algorithm. The higher this value is,
*	the more sensitive the algorithm will be to very low frequency movements.
*
*	The value returned by the function gives the length in units of 7.8ms, so a value of 10 = 78ms.
*
*	@param sh pointer to a shake_device structure as returned by shake_init_device()
*	@param value pointer to an unsigned char variable which will receive the contents of the register 
*	@return SHAKE_SUCCESS or SHAKE_ERROR */
SHAKE_API int shake_read_shaking_hpf_constant(shake_device* sh, unsigned char* value);

/** Writes a value into the SHAKE_NV_REG_SHAKING_HPF_CONSTANT register.
*
*	This register controls the high pass filter length in the detection algorithm. The higher this value is,
*	the more sensitive the algorithm will be to very low frequency movements.
*
*	\a value should be the filter length in units of 7.8ms (10 = 78ms).
*
*	@param sh pointer to a shake_device structure as returned by shake_init_device()
*	@param value the value to write into the register
*	@return SHAKE_SUCCESS or SHAKE_ERROR */
SHAKE_API int shake_write_shaking_hpf_constant(shake_device* sh, unsigned char value);

/** Reads the value of the SHAKE_NV_REG_SHAKING_LPF_CONSTANT register.
*
*	This register controls the low pass filter length in the detection algorithm. The higher this value is,
*	the less susceptible the algorithm will be to higher frequency vibration.
*
*	The value returned by the function gives the length in units of 7.8ms, so a value of 10 = 78ms.
*
*	@param sh pointer to a shake_device structure as returned by shake_init_device()
*	@param value pointer to an unsigned char variable which will receive the contents of the register 
*	@return SHAKE_SUCCESS or SHAKE_ERROR */
SHAKE_API int shake_read_shaking_lpf_constant(shake_device* sh, unsigned char* value);

/** Writes a value into the SHAKE_NV_REG_SHAKING_LPF_CONSTANT register.
*
*	This register controls the low pass filter length in the detection algorithm. The higher this value is,
*	the less susceptible the algorithm will be to higher frequency vibration.
*
*	\a value should be the filter length in units of 7.8ms (10 = 78ms).
*
*	@param sh pointer to a shake_device structure as returned by shake_init_device()
*	@param value the value to write into the register
*	@return SHAKE_SUCCESS or SHAKE_ERROR */
SHAKE_API int shake_write_shaking_lpf_constant(shake_device* sh, unsigned char value);

/** Resets the state of the shaking algorithm configuration registers to their recommended values.
*
*	Calling this function performs the following operations:
*		- the minimum peak acceleration threshold is set to 1g
*		- the hold-off time is set to 47ms
*		- the vibration profile address is set to zero
*		- the high pass filter length is set to 250ms
*		- the low pass filter length is set to 47ms
*		- shaking detection is enabled
*		- shaking detection direction detection is enabled
*		- shaking detection vibration playback is enabled 
*
*	@param sh pointer to a shake_device structure as returned by shake_init_device()
*	@return SHAKE_SUCCESS or SHAKE_ERROR */
SHAKE_API int shake_reset_shaking_detection(shake_device* sh);

/**	Reads the value of the SHAKE_NV_REG_HEART_RATE_CONFIG register.
*
*	This register controls the configuration of the heart rate monitor installed in some SHAKEs.
*
*	The value returned by the function will be zero or a combination of one or more values from the
*	::shake_heart_rate_configuration enumeration.
*
*	@param sh pointer to a shake_device structure as returned by shake_init_device()
*	@param value pointer to an unsigned char variable which will receive the contents of the register 
*	@return SHAKE_SUCCESS or SHAKE_ERROR */
SHAKE_API int shake_read_heart_rate_config(shake_device* sh, unsigned char* value);

/** Writes a value into the SHAKE_NV_REG_HEART_RATE_CONFIG register.
*
*	This register controls the configuration of the heart rate monitor installed in some SHAKEs.
*
*	\a value should be zero or a combination of one or more values from ::shake_heart_rate_configuration
*
*	@param sh pointer to a shake_device structure as returned by shake_init_device()
*	@param value the value to write into the register
*	@return SHAKE_SUCCESS or SHAKE_ERROR */
SHAKE_API int shake_write_heart_rate_config(shake_device* sh, unsigned char value);

/** Write a value into the VO_REG_PKTREQ register, which allows you to explicitly request that the SHAKE send a particular type or
*	types of data packet.
*
*	This function writes into a volatile register (write-only) that allows you to request
*	a single set of data from one or more sensors (this is instead of or in addition to the usual
*	data stream(s) generated by the device). 
*
*	The value you write should be a logical OR of one or more of the members of the ::shake_packet_requests enumeration,
*	indicating which sensor(s) you want to request a data packet from.
*
*	The data will arrive in the usual way and so you should use the normal data access functions to read it.
*
*	@param sh pointer to a shake_device structure as returned by shake_init_device()
*	@param value the value to write into the register
*	@return SHAKE_SUCCESS or SHAKE_ERROR */
SHAKE_API int shake_write_packet_request(shake_device* sh, unsigned char value);

/** Write a value into the VO_REG_DATAREQ register, which allows you to request that the SHAKE re-output the 
*	startup splash text (including firmware/hardware version) at any time.
*
*	The only valid value for the register at the moment is 1, which will cause the startup text to be output.
*
*	Future firmware versions may add extra functionality.
*	
*	@param sh pointer to a shake_device structure as returned by shake_init_device()
*	@param value the value to write into the register
*	@return SHAKE_SUCCESS or SHAKE_ERROR */
SHAKE_API int shake_write_data_request(shake_device* sh, unsigned char value);

/** Plays a vibration profile on one of the channels supported by the SHAKE (SK6 and SK7)
*
*	Note that for typical SHAKEs, the only valid channel is SHAKE_VIB_MAIN - the other channel
*	values refer to externally connected vibrotactile units or plugin modules that may not be installed.
*
*	@param sh pointer to a shake_device structure as returned by shake_init_device()
*	@param channel should be a value from the ::shake_vib_channels enumeration
*	@param profile should indicate the number of the vibration profile to playback from the SHAKE internal memory. The valid range of this number is SHAKE_VIB_PROFILE_MIN - SHAKE_VIB_PROFILE_MAX inclusive.
*
*	@return SHAKE_SUCCESS or SHAKE_ERROR */
SHAKE_API int shake_playvib(shake_device* sh, int channel, unsigned char profile);

/**	Plays a continuous vibration through one of two external channels supported by the SK6 (NOT SK7)
*
*	This function can only be used with a SHAKE SK6 that has firmware version >= 2.50, and has one or
*	more external piezo vibrators attached. For more information see the SHAKE SK6 manual. 
*
*	@param sh pointer to a shake_device structure as returned by shake_init_device()
*	@param channel should be either SHAKE_VIB_LEFT or SHAKE_VIB_RIGHT
*	@param amplitude amplitude of the vibration signal. Valid values are 0, 33, 66 and 100. 
*	@param time period of the vibration signal. The range is 0-63, in units of 26ms (1 = 26ms, 63 = 1.66s)
*
*	@return SHAKE_SUCCESS or SHAKE_ERROR */
SHAKE_API int sk6_playvib_continuous(shake_device* sh, int channel, unsigned char amplitude, unsigned char time);

/** This function uploads a vibration sample to the SHAKE SK6, storing it in one of the available locations in internal memory.
*
*	@param sh pointer to a shake_device structure as returned by shake_init_device()
*	@param profile should indicate the number of the vibration profile to store the sample in.
*	The valid range of this number is SHAKE_VIB_PROFILE_MIN - SHAKE_VIB_PROFILE_MAX inclusive.
*	@param sample should point to an array of integers containing the sample data. The array should
*	contain pairs of speed/time values (ie speed1,time1,speed2,time2,...). The speed values
*	should be in the range 0 - SHAKE_VIB_SPEED_MAX inclusive. The time values should be in the
*	range 0 - SHAKE_VIB_TIME_MAX inclusive, and are in units of 10 MILLISECONDS, so a value of
*	10 means 100ms. 
*	@param sample_length should be the the number of PAIRS in the @param sample array (NOT the total number
*	of values). The maximum number of pairs is given by SHAKE_VIB_SAMPLE_MAX_LENGTH.
*
*	@return SHAKE_SUCCESS or SHAKE_ERROR. Note that if and when the function returns
*	SHAKE_SUCCESS, it means that the sample has been fully uploaded and is now available
*	for immediate playback using shake_playvib() */
SHAKE_API int sk6_upload_vib_sample(shake_device* sh, unsigned char profile, int* sample, int sample_length);

/**	Similar to the above function except that it allows setting extra parameters in the vibration profile.
*	This function can be used with the following device configurations:
*		- A SHAKE SK6 with the SK6-V01 module, with continuous vibration support (firmware >= 2.60).
*		- A SHAKE SK7
*
*	@param sh pointer to a shake_device structure as returned by shake_init_device()
*	@param profile should indicate the number of the vibration profile to store the sample in.
*	The valid range of this number is SHAKE_VIB_PROFILE_MIN - SHAKE_VIB_PROFILE_MAX inclusive.
*	@param sample should point to an array of integers containing the sample data. The array should
*	contain pairs of speed/time values (ie speed1,time1,speed2,time2,...). The speed values
*	should be in the range 0 - SHAKE_VIB_SPEED_MAX inclusive. The time values should be in the
*	range 0 - SHAKE_VIB_TIME_MAX inclusive, and are in units of 10 MILLISECONDS, so a value of
*	10 means 100ms. 
*	@param sample_length should be the the number of PAIRS in the @param sample array (NOT the total number
*	of values). The maximum number of pairs is given by SHAKE_VIB_SAMPLE_MAX_LENGTH.
*
*	@param mode represents the mode of the drive when the actuation is triggered. For pulse drive must be 0x02 (this is the only
*				valid option for the SK6!)
*	@param freq frequency of the pulsed waveform. Actual output frequency is twice this value (eg 250Hz = 0x7D)
*	@param duty ratio of the pulsed output. Range of values is 1-9, representing 10%-90% respectively.
*	
*	@return SHAKE_SUCCESS or SHAKE_ERROR. Note that if and when the function returns
*	SHAKE_SUCCESS, it means that the sample has been fully uploaded and is now available
*	for immediate playback using shake_playvib() */
SHAKE_API int shake_upload_vib_sample_extended(shake_device* sh, unsigned char profile, int* sample, int sample_length, unsigned char mode, unsigned char freq, unsigned char duty);

/** Reads the battery level from the device. 
*	@param sh pointer to a shake_device structure as returned by shake_init_device()
*	@param value pointer to an unsigned char variable which will receive the result (range is 0 to SHAKE_BATTERY_MAX)
*	@return SHAKE_SUCCESS or SHAKE_ERROR */
SHAKE_API int shake_read_battery_level(shake_device* sh, unsigned char* value);

/** Reads the power status of the device.
*
*	The value from this register is a combination of 3 different items:
*		- External power on/off
*		- Battery charging/not charging
*		- Battery charged/not fully charged
*
*	There are 3 simple macros defined in shake_registers.h to make checking each item 
*	convenient (x is the value read from the device):
*		- SHAKE_EXT_POWER_ON(x) (nonzero when ext power connected)
*		- SHAKE_BATTERY_CHARGING(x) (nonzero if battery charging)
*		- SHAKE_BATTERY_FULL(x) (nonzero if battery fully charged)
*
*	@param sh pointer to a shake_device structure as returned by shake_init_device()
*	@param value pointer to an unsigned char variable which will receive the contents of the register 
*	@return SHAKE_SUCCESS or SHAKE_ERROR */
SHAKE_API int shake_read_power_status(shake_device* sh, unsigned char* value);

/** Reads the temperature sensor on the SHAKE device. 
*	The value of this sensor is updated every second, but must be retrieved manually using this function 
*	rather than as part of the normal data stream from the other sensors. 
*	The measureable range is 0-64 degrees C, mapped onto 255 0.25 degree steps in the
*	1-byte register. The reading returned by the function will be converted into 
*	this 0-64C range automatically.
*	
*	@param sh pointer to a shake_device structure as returned by shake_init_device()
*	@return a value between 0-64 on success, SHAKE_ERROR on error */
SHAKE_API float shake_read_temperature(shake_device* sh);

/** Read the SHAKE_NV_REG_EXPANSION_CONFIG register, which controls power to the various sensors on the
*	E01 expansion module.
*
*	The value returned will be a combination of the values from the ::shake_expansion_configuration enumeration.
*	@param sh pointer to a shake_device structure as returned by shake_init_device()
*	@param value pointer to an unsigned char variable which will receive the contents of the register 
*	@return SHAKE_SUCCESS or SHAKE_ERROR */
SHAKE_API int shake_read_expansion_config(shake_device* sh, unsigned char* value);

/** Write the SHAKE_NV_REG_EXPANSION_CONFIG register, which controls power to the various sensors 
*	E01 expansion module.
*
*	@param sh pointer to a shake_device structure as returned by shake_init_device()
*	@param value to write into the register (should be a combination of the values from the ::shake_expansion_configuration enumeration)
*	@return SHAKE_SUCCESS or SHAKE_ERROR */
SHAKE_API int shake_write_expansion_config(shake_device* sh, unsigned char value);

/**	Read the SHAKE_NV_REG_RFID_CONFIG register, which controls the RFID expansion module settings.
*
*	The value returned will be a combination of the values from the ::shake_rfid_config enumeration.
*	@param sh pointer to a shake_device structure as returned by shake_init_device()
*	@param value pointer to an unsigned char variable which will receive the contents of the register 
*	@return SHAKE_SUCCESS or SHAKE_ERROR */
SHAKE_API int shake_read_rfid_config(shake_device* sh, unsigned char* value);

/**	Write the SHAKE_NV_REG_RFID_CONFIG register, which controls the RFID expansion module settings.
*
*	@param sh pointer to a shake_device structure as returned by shake_init_device()
*	@param value to write into the register (should be a combination of the values from the ::shake_rfid_config enumeration)
*	@return SHAKE_SUCCESS or SHAKE_ERROR */
SHAKE_API int shake_write_rfid_config(shake_device* sh, unsigned char value);

/**	Read the SHAKE_NV_REG_RFID_FREQUENCY register, which controls the RFID autoscan frequency
*
*	The value returned will be between SHAKE_RFID_AUTOSCAN_MIN_FREQ and SHAKE_RFID_AUTOSCAN_MAX_FREQ
*	@param sh pointer to a shake_device structure as returned by shake_init_device()
*	@param value pointer to an unsigned char variable which will receive the contents of the register 
*	@return SHAKE_SUCCESS or SHAKE_ERROR */
SHAKE_API int shake_read_rfid_scan_freq(shake_device* sh, unsigned char* value);

/**	Write the SHAKE_NV_REG_RFID_FREQUENCY register, which controls the RFID autoscan frequency
*
*	@param sh pointer to a shake_device structure as returned by shake_init_device()
*	@param value to write into the register (between SHAKE_RFID_AUTOSCAN_MIN_FREQ and SHAKE_RFID_AUTOSCAN_MAX_FREQ)
*	@return SHAKE_SUCCESS or SHAKE_ERROR */
SHAKE_API int shake_write_rfid_scan_freq(shake_device* sh, unsigned char value);

/**	SK7 only. Configures vibration feedback which is activated when the compass heading enters a defined range. Note that
*	vibration feedback must be enabled using sk7_configure_heading_feedback, and the vibrator power must be turned on.
*
*	@param sh pointer to a shake_device structure as returned by shake_init_device()
*	@param lower_threshold lower threshold for the heading value, in tenths of a degree
*	@param upper_threshold upper threshold for the heading value, in tenths of a degree
*	@param hysteresis the amount by which the heading must change above/below a threshold before the vibration
*			feedback will be triggered again. Allowed values are 0-255, in tenths of a degree.
*	@param vib_profile the address of the vibration profile to play when a threshold is crossed.
*
*	@return SHAKE_SUCCESS or SHAKE_ERROR */
SHAKE_API int sk7_configure_heading_feedback(shake_device* sh, int lower_threshold, int upper_threshold, int hysteresis, int vib_profile);

/**	SK7 only. Enable/disable heading vibration feedback partially or completely 
*
*	@param sh pointer to a shake_device structure as returned by shake_init_device()
*	@param enabled set to 0 to disable ALL heading feedback, any other value to enable it
*	@param vib_looping set to 0 to disable vibration looping, any other value to enable it
*	@param led_feedback set to 0 to disable LED feedback when a threshold is crossed, any other value to enable it
*
*	@return SHAKE_SUCCESS or SHAKE_ERROR */
SHAKE_API int sk7_control_heading_feedback(shake_device* sh, int enabled, int vib_looping, int led_feedback);

/**	SK7 only. Configure output of roll-pitch-heading packet in firmware >= v1.00
*
*	The value parameter should be set as follows (21/6/09):
*		0:	disable RPH output, use old heading only packet instead
*		1:	output RPH packets using accelerometer and magnetometer to calculate the data
*	More options may be available in future firmware releases.
*	
*	@param sh pointer to a shake_device structure as returned by shake_init_device()
*	@param value the value to write into the roll-pitch-heading configuration register
*
*	@return SHAKE_SUCCESS or SHAKE_ERROR */
SHAKE_API int sk7_configure_roll_pitch_heading(shake_device* sh, int value);

/* 	=== Basic register access functions === 
*	These functions allow you direct access to the various registers on the device.
*	Usually it's easier to use one of the functions above, which don't involve
*	searching for the correct register address or checking if a register is
*	read/write etc. */

/** Reads one of the SHAKE device configuration registers and returns its contents.
*
*	This function can be used to read the contents of any valid register (as an alternative to using
*	the other register specific functions).
*
*	@param sh pointer to a shake_device structure as returned by shake_init_device()
*	@param addr the register address (use one of the SHAKE_NV_REG/VO_REG constants from shake_registers.h)
*	@param value pointer to an unsigned char variable which will receive the contents of the register 
*	
*	@return SHAKE_SUCCESS or SHAKE_ERROR */
SHAKE_API int shake_read(shake_device* sh, int addr, unsigned char* value);

/** Writes a value into one of the SHAKE device configuration registers.
*
*	This function can be used to write a value into any valid register (as an alternative to using
*	the other register specific functions).
*
*	@param sh pointer to a shake_device structure as returned by shake_init_device()
*	@param addr the register address (use one of the SHAKE_NV_REG/VO_REG constants from shake_registers.h)
*	@param value the value that will be placed into the register (note that the range
*		of values is dependent on the selected register, see user manual for details)
* 
*	@return SHAKE_SUCCESS or SHAKE_ERROR */
SHAKE_API int shake_write(shake_device* sh, int addr, unsigned char value);

#ifdef __cplusplus
}
#endif

#endif /* _SHAKE_HEADER_ */
