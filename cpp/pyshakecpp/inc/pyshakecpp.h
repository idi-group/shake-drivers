#ifndef _PYSHAKECPP_H_
#define _PYSHAKECPP_H_

#ifdef _WIN32
#ifdef PYSHAKECPP_COMPILE
#define PYSHAKECPP_EXPORT __declspec(dllexport)
#else
#define PYSHAKECPP_EXPORT __declspec(dllimport)
#endif
#else
#define PYSHAKECPP_EXPORT 
#endif

/*	Python wrapper for the SHAKE driver 
*	Requires Python 2.3+.
*/
#ifdef __cplusplus
extern "C" {
	PYSHAKECPP_EXPORT void initpyshakecpp(void);
}
#endif

static PyObject* pyshakecpp_init_device(PyObject* self, PyObject* args);
static PyObject* pyshakecpp_init_device_rfcomm_i64(PyObject* self, PyObject* args);
static PyObject* pyshakecpp_init_device_rfcomm_str(PyObject* self, PyObject* args);
#ifndef _WIN32
static PyObject* pyshakecpp_init_device_usb(PyObject* self, PyObject* args);
#endif
static PyObject* pyshakecpp_init_device_debug(PyObject* self, PyObject* args);
static PyObject* pyshakecpp_free_device(PyObject* self, PyObject* args);

static PyObject* pyshakecpp_info_firmware_revision(PyObject* self, PyObject* args);
static PyObject* pyshakecpp_info_hardware_revision(PyObject* self, PyObject* args);
static PyObject* pyshakecpp_info_serial_number(PyObject* self, PyObject* args);
static PyObject* pyshakecpp_info_module_slot1(PyObject* self, PyObject* args);
static PyObject* pyshakecpp_info_module_slot2(PyObject* self, PyObject* args);
static PyObject* pyshakecpp_info_retrieve(PyObject* self, PyObject* args);

static PyObject* pyshakecpp_factory_reset(PyObject* self, PyObject* args);

static PyObject* pyshakecpp_accx(PyObject* self, PyObject* args);
static PyObject* pyshakecpp_accy(PyObject* self, PyObject* args);
static PyObject* pyshakecpp_accz(PyObject* self, PyObject* args);
static PyObject* pyshakecpp_acc(PyObject* self, PyObject* args);

static PyObject* pyshakecpp_gyrx(PyObject* self, PyObject* args);
static PyObject* pyshakecpp_gyry(PyObject* self, PyObject* args);
static PyObject* pyshakecpp_gyrz(PyObject* self, PyObject* args);
static PyObject* pyshakecpp_gyr(PyObject* self, PyObject* args);

static PyObject* pyshakecpp_magx(PyObject* self, PyObject* args);
static PyObject* pyshakecpp_magy(PyObject* self, PyObject* args);
static PyObject* pyshakecpp_magz(PyObject* self, PyObject* args);
static PyObject* pyshakecpp_mag(PyObject* self, PyObject* args);

static PyObject* pyshakecpp_heading(PyObject* self, PyObject* args);
static PyObject* pyshakecpp_sk7_roll_pitch_heading(PyObject* self, PyObject* args);
static PyObject* pyshakecpp_sk7_roll_pitch_heading_quaternions(PyObject* self, PyObject* args);

static PyObject* pyshakecpp_sk6_cap0(PyObject* self, PyObject* args);
static PyObject* pyshakecpp_sk6_cap1(PyObject* self, PyObject* args);
static PyObject* pyshakecpp_sk6_cap(PyObject* self, PyObject* args);
static PyObject* pyshakecpp_sk7_cap(PyObject* self, PyObject* args);
static PyObject* pyshakecpp_sk7_cap_ext(PyObject* self, PyObject* args);

static PyObject* pyshakecpp_analog0(PyObject* self, PyObject* args);
static PyObject* pyshakecpp_analog1(PyObject* self, PyObject* args);
static PyObject* pyshakecpp_analog(PyObject* self, PyObject* args);

static PyObject* pyshakecpp_read(PyObject* self, PyObject* args);

static PyObject* pyshakecpp_write(PyObject* self, PyObject* args);

static PyObject* pyshakecpp_playvib(PyObject* self, PyObject* args);
static PyObject* pyshakecpp_sk6_upload_vib_sample(PyObject* self, PyObject* args);
static PyObject* pyshakecpp_upload_vib_sample_extended(PyObject* self, PyObject* args);
static PyObject* pyshakecpp_read_temperature(PyObject* self, PyObject* args);
static PyObject* pyshakecpp_register_event_callback(PyObject* self, PyObject* args);

static PyObject* pyshakecpp_logging_play(PyObject* self, PyObject* args);
static PyObject* pyshakecpp_logging_packets_read(PyObject* self, PyObject* args);

static PyObject* pyshakecpp_shaking_peakaccel(PyObject* self, PyObject* args);
static PyObject* pyshakecpp_shaking_direction(PyObject* self, PyObject* args);
static PyObject* pyshakecpp_shaking_timestamp(PyObject* self, PyObject* args);

static PyObject* pyshakecpp_heart_rate(PyObject* self, PyObject* args);

static PyObject* pyshakecpp_data_timestamp(PyObject* self, PyObject* args);

static PyObject* pyshakecpp_upload_audio_sample(PyObject* self, PyObject* args);
static PyObject* pyshakecpp_play_audio_sample(PyObject* self, PyObject* args);
static PyObject* pyshakecpp_register_audio_callback(PyObject* self, PyObject* args);

static PyObject* pyshakecpp_exp_upload_vib_sample(PyObject* self, PyObject* args);
static PyObject* pyshakecpp_exp_play_vib_sample(PyObject* self, PyObject* args);

static PyObject* pyshakecpp_rfid_tid(PyObject* self, PyObject* args);
static PyObject* pyshakecpp_rfid_scan(PyObject* self, PyObject* args);

static PyObject* pyshakecpp_wait_for_acks(PyObject* self, PyObject* args);


static PyObject* pyshakecpp_cleanup(PyObject* self, PyObject* args);

static PyMethodDef pyshakecpp_methods[] = {
	{ "init_device",	pyshakecpp_init_device,		1,	"Initialise a SHAKE connection" },
	{ "init_device_rfcomm_i64",	pyshakecpp_init_device_rfcomm_i64,		1,	"Initialise a SHAKE connection over RFCOMM" },
	{ "init_device_rfcomm_str",	pyshakecpp_init_device_rfcomm_str,		1,	"Initialise a SHAKE connection over RFCOMM" },
#ifndef _WIN32
	{ "init_device_usb", pyshakecpp_init_device_usb, 1, "Initialise a SHAKE connection using USB serial port"},
#endif
	{ "init_device_debug", pyshakecpp_init_device_debug, 1, "initialise a SHAKE connection to a file (debugging)" },
	{ "free_device",	pyshakecpp_free_device,		1,	"Close a SHAKE connection" },


	{ "info_firmware_revision", pyshakecpp_info_firmware_revision, 1, "Firmware revision" },
	{ "info_hardware_revision", pyshakecpp_info_hardware_revision, 1, "Hardware revision" },
	{ "info_serial_number", pyshakecpp_info_serial_number, 1, "Serial number" },
	{ "info_module_slot1",	pyshakecpp_info_module_slot1,	1,	"Expansion module in slot 1" },
	{ "info_module_slot2",	pyshakecpp_info_module_slot2,	1,	"Expansion module in slot 2" },
	{ "info_retrieve",		pyshakecpp_info_retrieve, 1, "Retrieves device info" },

	{ "factory_reset",		pyshakecpp_factory_reset, 1,	"Factory reset" },

	{ "accx",			pyshakecpp_accx,				1,	"x-axis acceleration" },
	{ "accy",			pyshakecpp_accy,				1,	"y-axis acceleration" },
	{ "accz",			pyshakecpp_accz,				1,	"z-axis acceleration" },
	{ "acc",			pyshakecpp_acc,				1,	"x,y,z-axis acceleration" },

	{ "gyrx",			pyshakecpp_gyrx,				1,	"x-axis gyro" },
	{ "gyry",			pyshakecpp_gyry,				1,	"y-axis gyro" },
	{ "gyrz",			pyshakecpp_gyrz,				1,	"z-axis gyro" },
	{ "gyr",			pyshakecpp_gyr,				1,	"x,y,z-axis gyro" },

	{ "magx",			pyshakecpp_magx,				1,	"x-axis magnetometer" },
	{ "magy",			pyshakecpp_magy,				1,	"y-axis magnetometer" },
	{ "magz",			pyshakecpp_magz,				1,	"z-axis magnetometer" },
	{ "mag",			pyshakecpp_mag,				1,	"x,y,z-axis magnetometer" },

	{ "heading",		pyshakecpp_heading,			1,	"heading" },
	{ "sk7_roll_pitch_heading", pyshakecpp_sk7_roll_pitch_heading, 1, "roll, pitch, heading" },
	{ "sk7_roll_pitch_heading_quaternions", pyshakecpp_sk7_roll_pitch_heading_quaternions, 1, "roll, pitch, heading (quaternion format)" },

	{ "sk6_cap0",			pyshakecpp_sk6_cap0,				1,	"cap sw 0 proximity" },
	{ "sk6_cap1",			pyshakecpp_sk6_cap1,				1,	"cap sw 1 proximity" },
	{ "sk6_cap",			pyshakecpp_sk6_cap,				1,	"cap sw 0+1 proximity (SK6)" },
	{ "sk7_cap",			pyshakecpp_sk7_cap,				1, "cap sw (SK7)" },
	{ "sk7_cap_ext",		pyshakecpp_sk7_cap_ext,			1, "cap sw (ext, SK7)" },

	{ "analog0",		pyshakecpp_analog0,			1,	"analog input 0" },
	{ "analog1",		pyshakecpp_analog1,			1,	"analog input 1" },
	{ "analog",			pyshakecpp_analog,				1,	"analog inputs 0 + 1" },

	{ "heart_rate",		pyshakecpp_heart_rate,			1,	"heart rate" },

	{ "read",			pyshakecpp_read,				1,	"read a register" },
	{ "write",			pyshakecpp_write,				1,	"write a register" },

	{ "playvib",			pyshakecpp_playvib,				1, "play a vibration" },
	{ "sk6_upload_vib_sample",	pyshakecpp_sk6_upload_vib_sample, 1, "uploads a vibration sample" },
	{ "upload_vib_sample_extended", pyshakecpp_upload_vib_sample_extended, 1, "uploads a vibration sample with extra parameters" },
	{ "read_temperature", pyshakecpp_read_temperature, 1, "reads temperature register" },

	{ "register_event_callback", pyshakecpp_register_event_callback, METH_VARARGS, "registers callback for SHAKE events" },

	{ "logging_play",	pyshakecpp_logging_play,		1,	"logging playback" },
	{ "logging_packets_read", pyshakecpp_logging_packets_read, 1, "get number of logged packets received" },

	{ "shaking_peakaccel", pyshakecpp_shaking_peakaccel,		1, "peak acceleration for last shaking event" },
	{ "shaking_direction", pyshakecpp_shaking_direction,		1, "direction of last shaking event" },
	{ "shaking_timestamp", pyshakecpp_shaking_timestamp,		1, "timestamp for last shaking event" },

	{ "data_timestamp", pyshakecpp_data_timestamp,				1, "data timestamp" },

	{ "upload_audio_sample", pyshakecpp_upload_audio_sample, 1, "upload audio sample" },
	{ "play_audio_sample", pyshakecpp_play_audio_sample, 1, "play audio sample" },
	{ "register_audio_callback", pyshakecpp_register_audio_callback, 1, "registers audio callback" },

	{ "exp_upload_vib_sample", pyshakecpp_exp_upload_vib_sample, 1, "upload vib sample" },
	{ "exp_play_vib_sample", pyshakecpp_exp_play_vib_sample, 1, "play vib sample" },

	{ "rfid_tid",	pyshakecpp_rfid_tid, 1, "read last RFID tag" },
	{ "rfid_scan", pyshakecpp_rfid_scan, 1, "trigger RFID scan" },

	{ "wait_for_acks", pyshakecpp_wait_for_acks, 1, "enable/disable waiting for acks"},

	{ "cleanup", 		pyshakecpp_cleanup, 			1, "clean up on exit" },

	{ NULL, NULL, 0, NULL }
};

#endif
