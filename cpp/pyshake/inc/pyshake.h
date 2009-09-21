#ifndef _PYSHAKE_H_
#define _PYSHAKE_H_

#ifdef _WIN32
#ifdef PYSHAKE_COMPILE
#define PYSHAKE_EXPORT __declspec(dllexport)
#else
#define PYSHAKE_EXPORT __declspec(dllimport)
#endif
#else
#define PYSHAKE_EXPORT 
#endif

/*	Python wrapper for the SHAKE driver 
*	Requires Python 2.3+.
*/
#ifdef __cplusplus
extern "C" {
	PYSHAKE_EXPORT void initpyshake(void);
}
#endif

static PyObject* pyshake_init_device(PyObject* self, PyObject* args);
static PyObject* pyshake_init_device_rfcomm_i64(PyObject* self, PyObject* args);
static PyObject* pyshake_init_device_rfcomm_str(PyObject* self, PyObject* args);
static PyObject* pyshake_init_device_debug(PyObject* self, PyObject* args);
static PyObject* pyshake_free_device(PyObject* self, PyObject* args);

static PyObject* pyshake_info_firmware_revision(PyObject* self, PyObject* args);
static PyObject* pyshake_info_hardware_revision(PyObject* self, PyObject* args);
static PyObject* pyshake_info_serial_number(PyObject* self, PyObject* args);
static PyObject* pyshake_info_module_slot1(PyObject* self, PyObject* args);
static PyObject* pyshake_info_module_slot2(PyObject* self, PyObject* args);
static PyObject* pyshake_info_retrieve(PyObject* self, PyObject* args);

static PyObject* pyshake_factory_reset(PyObject* self, PyObject* args);

static PyObject* pyshake_accx(PyObject* self, PyObject* args);
static PyObject* pyshake_accy(PyObject* self, PyObject* args);
static PyObject* pyshake_accz(PyObject* self, PyObject* args);
static PyObject* pyshake_acc(PyObject* self, PyObject* args);

static PyObject* pyshake_gyrx(PyObject* self, PyObject* args);
static PyObject* pyshake_gyry(PyObject* self, PyObject* args);
static PyObject* pyshake_gyrz(PyObject* self, PyObject* args);
static PyObject* pyshake_gyr(PyObject* self, PyObject* args);

static PyObject* pyshake_magx(PyObject* self, PyObject* args);
static PyObject* pyshake_magy(PyObject* self, PyObject* args);
static PyObject* pyshake_magz(PyObject* self, PyObject* args);
static PyObject* pyshake_mag(PyObject* self, PyObject* args);

static PyObject* pyshake_heading(PyObject* self, PyObject* args);
static PyObject* pyshake_sk7_roll_pitch_heading(PyObject* self, PyObject* args);

static PyObject* pyshake_sk6_cap0(PyObject* self, PyObject* args);
static PyObject* pyshake_sk6_cap1(PyObject* self, PyObject* args);
static PyObject* pyshake_sk6_cap(PyObject* self, PyObject* args);
static PyObject* pyshake_sk7_cap(PyObject* self, PyObject* args);
static PyObject* pyshake_sk7_cap_ext(PyObject* self, PyObject* args);

static PyObject* pyshake_analog0(PyObject* self, PyObject* args);
static PyObject* pyshake_analog1(PyObject* self, PyObject* args);
static PyObject* pyshake_analog(PyObject* self, PyObject* args);

static PyObject* pyshake_read(PyObject* self, PyObject* args);

static PyObject* pyshake_write(PyObject* self, PyObject* args);

static PyObject* pyshake_playvib(PyObject* self, PyObject* args);
static PyObject* pyshake_sk6_upload_vib_sample(PyObject* self, PyObject* args);
static PyObject* pyshake_upload_vib_sample_extended(PyObject* self, PyObject* args);
static PyObject* pyshake_read_temperature(PyObject* self, PyObject* args);
static PyObject* pyshake_register_event_callback(PyObject* self, PyObject* args);

static PyObject* pyshake_logging_play(PyObject* self, PyObject* args);
static PyObject* pyshake_logging_packets_read(PyObject* self, PyObject* args);

static PyObject* pyshake_shaking_peakaccel(PyObject* self, PyObject* args);
static PyObject* pyshake_shaking_direction(PyObject* self, PyObject* args);
static PyObject* pyshake_shaking_timestamp(PyObject* self, PyObject* args);

static PyObject* pyshake_heart_rate(PyObject* self, PyObject* args);

static PyObject* pyshake_data_timestamp(PyObject* self, PyObject* args);

static PyObject* pyshake_upload_audio_sample(PyObject* self, PyObject* args);
static PyObject* pyshake_play_audio_sample(PyObject* self, PyObject* args);
static PyObject* pyshake_register_audio_callback(PyObject* self, PyObject* args);

static PyObject* pyshake_exp_upload_vib_sample(PyObject* self, PyObject* args);
static PyObject* pyshake_exp_play_vib_sample(PyObject* self, PyObject* args);

static PyObject* pyshake_rfid_tid(PyObject* self, PyObject* args);
static PyObject* pyshake_rfid_scan(PyObject* self, PyObject* args);

static PyObject* pyshake_wait_for_acks(PyObject* self, PyObject* args);


static PyObject* pyshake_cleanup(PyObject* self, PyObject* args);

static PyMethodDef pyshake_methods[] = {
	{ "init_device",	pyshake_init_device,		1,	"Initialise a SHAKE connection" },
	{ "init_device_rfcomm_i64",	pyshake_init_device_rfcomm_i64,		1,	"Initialise a SHAKE connection over RFCOMM" },
	{ "init_device_rfcomm_str",	pyshake_init_device_rfcomm_str,		1,	"Initialise a SHAKE connection over RFCOMM" },
	{ "init_device_debug", pyshake_init_device_debug, 1, "initialise a SHAKE connection to a file (debugging)" },
	{ "free_device",	pyshake_free_device,		1,	"Close a SHAKE connection" },


	{ "info_firmware_revision", pyshake_info_firmware_revision, 1, "Firmware revision" },
	{ "info_hardware_revision", pyshake_info_hardware_revision, 1, "Hardware revision" },
	{ "info_serial_number", pyshake_info_serial_number, 1, "Serial number" },
	{ "info_module_slot1",	pyshake_info_module_slot1,	1,	"Expansion module in slot 1" },
	{ "info_module_slot2",	pyshake_info_module_slot2,	1,	"Expansion module in slot 2" },
	{ "info_retrieve",		pyshake_info_retrieve, 1, "Retrieves device info" },

	{ "factory_reset",		pyshake_factory_reset, 1,	"Factory reset" },

	{ "accx",			pyshake_accx,				1,	"x-axis acceleration" },
	{ "accy",			pyshake_accy,				1,	"y-axis acceleration" },
	{ "accz",			pyshake_accz,				1,	"z-axis acceleration" },
	{ "acc",			pyshake_acc,				1,	"x,y,z-axis acceleration" },

	{ "gyrx",			pyshake_gyrx,				1,	"x-axis gyro" },
	{ "gyry",			pyshake_gyry,				1,	"y-axis gyro" },
	{ "gyrz",			pyshake_gyrz,				1,	"z-axis gyro" },
	{ "gyr",			pyshake_gyr,				1,	"x,y,z-axis gyro" },

	{ "magx",			pyshake_magx,				1,	"x-axis magnetometer" },
	{ "magy",			pyshake_magy,				1,	"y-axis magnetometer" },
	{ "magz",			pyshake_magz,				1,	"z-axis magnetometer" },
	{ "mag",			pyshake_mag,				1,	"x,y,z-axis magnetometer" },

	{ "heading",		pyshake_heading,			1,	"heading" },
	{ "sk7_roll_pitch_heading", pyshake_sk7_roll_pitch_heading, 1, "roll, pitch, heading" },

	{ "sk6_cap0",			pyshake_sk6_cap0,				1,	"cap sw 0 proximity" },
	{ "sk6_cap1",			pyshake_sk6_cap1,				1,	"cap sw 1 proximity" },
	{ "sk6_cap",			pyshake_sk6_cap,				1,	"cap sw 0+1 proximity (SK6)" },
	{ "sk7_cap",			pyshake_sk7_cap,				1, "cap sw (SK7)" },
	{ "sk7_cap_ext",		pyshake_sk7_cap_ext,			1, "cap sw (ext, SK7)" },

	{ "analog0",		pyshake_analog0,			1,	"analog input 0" },
	{ "analog1",		pyshake_analog1,			1,	"analog input 1" },
	{ "analog",			pyshake_analog,				1,	"analog inputs 0 + 1" },

	{ "heart_rate",		pyshake_heart_rate,			1,	"heart rate" },

	{ "read",			pyshake_read,				1,	"read a register" },
	{ "write",			pyshake_write,				1,	"write a register" },

	{ "playvib",			pyshake_playvib,				1, "play a vibration" },
	{ "sk6_upload_vib_sample",	pyshake_sk6_upload_vib_sample, 1, "uploads a vibration sample" },
	{ "upload_vib_sample_extended", pyshake_upload_vib_sample_extended, 1, "uploads a vibration sample with extra parameters" },
	{ "read_temperature", pyshake_read_temperature, 1, "reads temperature register" },

	{ "register_event_callback", pyshake_register_event_callback, METH_VARARGS, "registers callback for SHAKE events" },

	{ "logging_play",	pyshake_logging_play,		1,	"logging playback" },
	{ "logging_packets_read", pyshake_logging_packets_read, 1, "get number of logged packets received" },

	{ "shaking_peakaccel", pyshake_shaking_peakaccel,		1, "peak acceleration for last shaking event" },
	{ "shaking_direction", pyshake_shaking_direction,		1, "direction of last shaking event" },
	{ "shaking_timestamp", pyshake_shaking_timestamp,		1, "timestamp for last shaking event" },

	{ "data_timestamp", pyshake_data_timestamp,				1, "data timestamp" },

	{ "upload_audio_sample", pyshake_upload_audio_sample, 1, "upload audio sample" },
	{ "play_audio_sample", pyshake_play_audio_sample, 1, "play audio sample" },
	{ "register_audio_callback", pyshake_register_audio_callback, 1, "registers audio callback" },

	{ "exp_upload_vib_sample", pyshake_exp_upload_vib_sample, 1, "upload vib sample" },
	{ "exp_play_vib_sample", pyshake_exp_play_vib_sample, 1, "play vib sample" },

	{ "rfid_tid",	pyshake_rfid_tid, 1, "read last RFID tag" },
	{ "rfid_scan", pyshake_rfid_scan, 1, "trigger RFID scan" },

	{ "wait_for_acks", pyshake_wait_for_acks, 1, "enable/disable waiting for acks"},

	{ "cleanup", 		pyshake_cleanup, 			1, "clean up on exit" },

	{ NULL, NULL, 0, NULL }
};

#endif
