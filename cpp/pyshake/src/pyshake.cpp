#undef _DEBUG /* Link with python24.lib and not python24_d.lib */

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <mmsystem.h>
#else
#include <stdlib.h>
#endif

#include "shake_driver.h"
#include "shake_btdefs.h"

#ifdef _WIN32
#include "Python.h"
#else
#include <python2.5/Python.h>
#endif
#include "pyshake.h"

static PyObject* pyshake_ex;

/*	Maximum number of SHAKES to allow on a single host device. For PocketPCs this
*	is limited to 1 by the OS as far as I can tell. For Windows PCs/laptops, it could
*	be higher, but since every SHAKE would have to use the same BT adapter, and I guess
*	that would eventually cause problems... */

#define MAX_SHAKES		8

static shake_device* devicelist[MAX_SHAKES];
static PyObject* callbacks[MAX_SHAKES][2];
static PyObject* audio_callbacks[MAX_SHAKES][4];
static int devicelist_count = 0;

static int event_callback_active[MAX_SHAKES] = { 0 };
static int audio_callback_active[MAX_SHAKES] = { 0 };

static void SHAKE_CALLBACK main_callback(shake_device* dev, int ev) {
	int i;
    PyObject *result;

	for(i=0;i<devicelist_count;i++) {
		if(devicelist[i] == dev) {
			// call python callback in callbacks[i]
			PyGILState_STATE gstate;
			gstate = PyGILState_Ensure();

			/* Perform Python actions here.  */
			result = PyEval_CallFunction(callbacks[i][0], "(Oi)", callbacks[i][1], ev);
			PyGILState_Release(gstate);
			
			Py_XDECREF(result);
			break;
		}
	}
}

static void unregister_audio_callback(int id) {
	// if this value is 0 safe to delete immediately, not in a callback
	if(audio_callback_active[id] == 0 || audio_callback_active[id] == 2) {
		audio_callbacks[id][1] = NULL;
		Py_XDECREF(audio_callbacks[id][0]);
		shake_register_audio_callback(devicelist[id], NULL);
		// TODO delete list objects??
		audio_callback_active[id] = 0;
	} else if (audio_callback_active[id] == 1) {
		// if value is 1, callback is active - set value to 2 and this function will be called automatically
		// when the callback returns
		audio_callback_active[id] = 2;
	}
}

static void SHAKE_CALLBACK main_audio_callback(shake_device* dev, short* mic_samples, int mic_len, short* playback_samples, int playback_len) {
	int i;
    PyObject *result = NULL;

	//printf("pyshake_audio_callback\n");

	for(i=0;i<devicelist_count;i++) {
		if(devicelist[i] == dev && audio_callbacks[i][1] != NULL) {
			int j = 0;
			PyGILState_STATE gstate;
			gstate = PyGILState_Ensure();
			audio_callback_active[i] = 1;

			//printf("pyshake_audio_callback for device %d, %d samples for mic (%d), %d samples for playback (%d)\n", i, mic_len, mic_samples == NULL, playback_len, playback_samples == NULL);
			
			if(mic_samples == NULL) {
				audio_callbacks[i][3] = PyList_New(SHAKE_AUDIO_DATA_LEN);
				for(j=0;j<playback_len;j++) {
					PyList_SetItem(audio_callbacks[i][3], j, PyLong_FromLong(playback_samples[j]));
				}
				Py_INCREF(audio_callbacks[i][3]);

				Py_INCREF(Py_None);
				audio_callbacks[i][2] = Py_None; // mic samples
			} else {
				audio_callbacks[i][2] = PyList_New(SHAKE_AUDIO_DATA_LEN);
				for(j=0;j<mic_len;j++) {
					PyList_SetItem(audio_callbacks[i][2], j, PyLong_FromLong(mic_samples[j]));
				}
				Py_INCREF(audio_callbacks[i][2]);
				//printf("Building list of samples\n");

				Py_INCREF(Py_None);
				audio_callbacks[i][3] = Py_None; // playback samples
			}
			
			// call python callback in audio_callbacks[i]
			if(audio_callbacks[i][1] != NULL) {
				result = PyEval_CallFunction(audio_callbacks[i][0], "(OOO)", audio_callbacks[i][1], audio_callbacks[i][2], audio_callbacks[i][3]);
				Py_XDECREF(result);
			}
			
			Py_XDECREF(audio_callbacks[i][2]);
			Py_XDECREF(audio_callbacks[i][3]);

			// if value still == 1, do nothing
			// if value now == -1, means that callback should be removed immediately as it has been unregistered
			if(audio_callback_active[i] == 1) {
				audio_callback_active[i] = 0;
			} else {
				unregister_audio_callback(i);
			}
			PyGILState_Release(gstate);
			break;
		}
	}
}

static PyObject* pyshake_cleanup(PyObject* self, PyObject* args) {
	int i;

	for(i=0;i<MAX_SHAKES;i++) {
		if(devicelist[i] != NULL)
			shake_free_device(devicelist[i]);
	}

	Py_INCREF(Py_None);
	return Py_None;
}

PYSHAKE_EXPORT void initpyshake(void) {
	PyObject* mod = Py_InitModule("pyshake", pyshake_methods);
	PyEval_InitThreads();
	pyshake_ex = PyErr_NewException("pyshake.error", NULL, NULL);
	Py_INCREF(pyshake_ex);
	PyModule_AddObject(mod, "PyshakeError", pyshake_ex);
}

// arguments: a COM port number or a BT addr 
static PyObject* pyshake_init_device(PyObject* self, PyObject* args) {
	int com_port, parsedok;
	char* btaddr;
	shake_device* dev;
	int devtype;

	if(devicelist_count >= MAX_SHAKES) {
		PyRun_SimpleString("print 'Maximum number of SHAKES reached!'");
		return Py_BuildValue("i", SHAKE_ERROR);
	}

	#ifdef _WIN32
	parsedok = PyArg_ParseTuple(args, "ii", &com_port, &devtype);
	dev = shake_init_device(com_port, devtype);
	#endif
	
	if(dev == NULL)
		return Py_BuildValue("i", SHAKE_ERROR);

	devicelist[devicelist_count] = dev;
	devicelist_count++;

	return Py_BuildValue("i", devicelist_count - 1);
}

static PyObject* pyshake_init_device_rfcomm_i64(PyObject* self, PyObject* args) {
	#ifdef SHAKE_RFCOMM_SUPPORTED
	shake_device* dev;
	PY_LONG_LONG btaddr;
	int devtype;

	if(devicelist_count >= MAX_SHAKES) {
		PyRun_SimpleString("print 'Maximum number of SHAKES reached!'");
		return Py_BuildValue("i", SHAKE_ERROR);
	}

	// XXX has to be "L" not "l" despite what API docs say
	PyArg_ParseTuple(args, "Li", &btaddr, &devtype);
	dev = shake_init_device_rfcomm_i64(btaddr, devtype);

	if(dev == NULL)
		return Py_BuildValue("i", SHAKE_ERROR);

	devicelist[devicelist_count] = dev;
	devicelist_count++;

	return Py_BuildValue("i", devicelist_count - 1);

	#else
	return Py_BuildValue("i", SHAKE_ERROR);
	#endif
}

static PyObject* pyshake_init_device_rfcomm_str(PyObject* self, PyObject* args) {
	#ifdef SHAKE_RFCOMM_SUPPORTED
	shake_device* dev;
	char* btaddr;	
	int devtype;

	if(devicelist_count >= MAX_SHAKES) {
		PyRun_SimpleString("print 'Maximum number of SHAKES reached!'");
		return Py_BuildValue("i", SHAKE_ERROR);
	}

	PyArg_ParseTuple(args, "si", &btaddr, &devtype);
	dev = shake_init_device_rfcomm_str(btaddr, devtype);

	if(dev == NULL)
		return Py_BuildValue("i", SHAKE_ERROR);

	devicelist[devicelist_count] = dev;
	devicelist_count++;

	return Py_BuildValue("i", devicelist_count - 1);

	#else
	return Py_BuildValue("i", SHAKE_ERROR);
	#endif
}

static PyObject* pyshake_init_device_osx_usb(PyObject* self, PyObject* args) {
#ifdef __APPLE__
	shake_device* dev;
	char* usb_dev;	
	int devtype;

	if(devicelist_count >= MAX_SHAKES) {
		PyRun_SimpleString("print 'Maximum number of SHAKES reached!'");
		return Py_BuildValue("i", SHAKE_ERROR);
	}

	PyArg_ParseTuple(args, "si", &usb_dev, &devtype);
	dev = shake_init_device_osx_usb(usb_dev, devtype);

	if(dev == NULL)
		return Py_BuildValue("i", SHAKE_ERROR);

	devicelist[devicelist_count] = dev;
	devicelist_count++;

	return Py_BuildValue("i", devicelist_count - 1);
#else
	return Py_BuildValue("i", SHAKE_ERROR);
#endif
}

static PyObject* pyshake_init_device_debug(PyObject* self, PyObject* args) {
	shake_device* dev;
	char* inp, *outp;
	int devtype;

	if(devicelist_count >= MAX_SHAKES) {
		PyRun_SimpleString("print 'Maximum number of SHAKES reached!'");
		return Py_BuildValue("i", SHAKE_ERROR);
	}

	PyArg_ParseTuple(args, "ssi", &inp, &outp, &devtype);

	printf("Connecting to: %s | %s\n", inp, outp);
	dev = shake_init_device_DEBUGFILE(inp, outp, devtype);

	if(dev == NULL)
		return Py_BuildValue("i", SHAKE_ERROR);

	devicelist[devicelist_count] = dev;
	devicelist_count++;

	return Py_BuildValue("i", devicelist_count - 1);
}

// arguments: 1 int, ID number
static PyObject* pyshake_free_device(PyObject* self, PyObject* args) {
	int id, i;

	PyArg_ParseTuple(args, "i", &id);

	if(id < 0 || id >= MAX_SHAKES)
		return Py_BuildValue("i", SHAKE_ERROR);

	shake_free_device(devicelist[id]);
	if(audio_callback_active[id] != 0) {
		//printf("Waiting on an audio callback!\n");
		while(audio_callback_active[id] != 0) {
			shake_sleep(1000);
			//printf("%d\n", audio_callback_active[id]);
		}
	}


	devicelist[id] = NULL;
	for(i=id;i<MAX_SHAKES-1;i++) {
		devicelist[id] = devicelist[id+1];
		callbacks[id][0] = callbacks[id+1][0];
		callbacks[id][1] = callbacks[id+1][1];
	}
	devicelist_count--;

	return Py_BuildValue("i", 1);
}

static PyObject* pyshake_info_firmware_revision(PyObject* self, PyObject* args) {
	int id;

	PyArg_ParseTuple(args, "i", &id);

	if(id < 0 || id >= MAX_SHAKES)
		return Py_BuildValue("d", 0.0);

	return Py_BuildValue("f", shake_info_firmware_revision(devicelist[id]));
}

static PyObject* pyshake_info_hardware_revision(PyObject* self, PyObject* args) {
	int id;

	PyArg_ParseTuple(args, "i", &id);

	if(id < 0 || id >= MAX_SHAKES)
		return Py_BuildValue("d", 0.0);

	return Py_BuildValue("f", shake_info_hardware_revision(devicelist[id]));
}

static PyObject* pyshake_info_serial_number(PyObject* self, PyObject* args) {
	int id;

	PyArg_ParseTuple(args, "i", &id);

	if(id < 0 || id >= MAX_SHAKES)
		return Py_BuildValue("s", "");

	return Py_BuildValue("s", shake_info_serial_number(devicelist[id]));
}


static PyObject* pyshake_info_module_slot1(PyObject* self, PyObject* args) {
	int id;

	PyArg_ParseTuple(args, "i", &id);

	if(id < 0 || id >= MAX_SHAKES)
		return Py_BuildValue("i", SHAKE_ERROR);

	return Py_BuildValue("i", shake_info_module_slot1(devicelist[id]));
}

static PyObject* pyshake_info_module_slot2(PyObject* self, PyObject* args) {
	int id;

	PyArg_ParseTuple(args, "i", &id);

	if(id < 0 || id >= MAX_SHAKES)
		return Py_BuildValue("i", SHAKE_ERROR);

	return Py_BuildValue("i", shake_info_module_slot2(devicelist[id]));
}

static PyObject* pyshake_info_retrieve(PyObject* self, PyObject* args) {
	int id;

	PyArg_ParseTuple(args, "i", &id);

	printf("pyshake_info_retrieve...\n");

	if(id < 0 || id >= MAX_SHAKES)
		return Py_BuildValue("s", "");

	return Py_BuildValue("i", shake_info_retrieve(devicelist[id]));
}

static PyObject* pyshake_factory_reset(PyObject* self, PyObject* args) {
	int id, repeats;

	PyArg_ParseTuple(args, "ii", &id, &repeats);

	if(id < 0 || id >= MAX_SHAKES)
		return Py_BuildValue("i", SHAKE_ERROR);

	return Py_BuildValue("i", shake_factory_reset(devicelist[id], repeats));
}

static PyObject* pyshake_wait_for_acks(PyObject* self, PyObject* args) {
	int id, wait_for_ack;

	PyArg_ParseTuple(args, "ii", &id, &wait_for_ack);

	if(id < 0 || id >= MAX_SHAKES)
		return Py_BuildValue("i", 0);

	shake_wait_for_acks(devicelist[id], wait_for_ack);
	return Py_BuildValue("i", 1);
}

// arguments: 1 int, ID number
static PyObject* pyshake_accx(PyObject* self, PyObject* args) {
	int id;

	PyArg_ParseTuple(args, "i", &id);

	if(id < 0 || id >= MAX_SHAKES)
		return Py_BuildValue("i", SHAKE_ERROR);

	if(devicelist[id] != NULL)
		return Py_BuildValue("i", shake_accx(devicelist[id]));

	return Py_BuildValue("i", SHAKE_ERROR);
}

// arguments: 1 int, ID number
static PyObject* pyshake_accy(PyObject* self, PyObject* args) {
	int id;

	PyArg_ParseTuple(args, "i", &id);

	if(id < 0 || id >= MAX_SHAKES)
		return Py_BuildValue("i", SHAKE_ERROR);

	if(devicelist[id] != NULL)
		return Py_BuildValue("i", shake_accy(devicelist[id]));

	return Py_BuildValue("i", SHAKE_ERROR);
}

// arguments: 1 int, ID number
static PyObject* pyshake_accz(PyObject* self, PyObject* args) {
	int id;

	PyArg_ParseTuple(args, "i", &id);

	if(id < 0 || id >= MAX_SHAKES)
		return Py_BuildValue("i", SHAKE_ERROR);

	if(devicelist[id] != NULL)
		return Py_BuildValue("i", shake_accz(devicelist[id]));

	return Py_BuildValue("i", SHAKE_ERROR);
}

// arguments: 1 int, ID number
static PyObject* pyshake_acc(PyObject* self, PyObject* args) {
	int id;

	PyArg_ParseTuple(args, "i", &id);

	if(id < 0 || id >= MAX_SHAKES) {
		Py_INCREF(Py_None);
		return Py_None;
	}

	if(devicelist[id] != NULL) {
		int xyz[3];
		PyObject* obj;

		shake_acc(devicelist[id], xyz);
		obj = Py_BuildValue("[i, i, i]", xyz[0], xyz[1], xyz[2]);
		Py_INCREF(obj);
		return obj;
	}

	Py_INCREF(Py_None);
	return Py_None;
}

static PyObject* pyshake_gyrx(PyObject* self, PyObject* args) {
	int id;

	PyArg_ParseTuple(args, "i", &id);

	if(id < 0 || id >= MAX_SHAKES)
		return Py_BuildValue("i", SHAKE_ERROR);

	if(devicelist[id] != NULL)
		return Py_BuildValue("i", shake_gyrx(devicelist[id]));

	return Py_BuildValue("i", SHAKE_ERROR);
}
static PyObject* pyshake_gyry(PyObject* self, PyObject* args) {
	int id;

	PyArg_ParseTuple(args, "i", &id);

	if(id < 0 || id >= MAX_SHAKES)
		return Py_BuildValue("i", SHAKE_ERROR);

	if(devicelist[id] != NULL)
		return Py_BuildValue("i", shake_gyry(devicelist[id]));

	return Py_BuildValue("i", SHAKE_ERROR);
}

static PyObject* pyshake_gyrz(PyObject* self, PyObject* args) {
	int id;

	PyArg_ParseTuple(args, "i", &id);

	if(id < 0 || id >= MAX_SHAKES)
		return Py_BuildValue("i", SHAKE_ERROR);

	if(devicelist[id] != NULL)
		return Py_BuildValue("i", shake_gyrz(devicelist[id]));

	return Py_BuildValue("i", SHAKE_ERROR);
}

static PyObject* pyshake_gyr(PyObject* self, PyObject* args) {
	int id;

	PyArg_ParseTuple(args, "i", &id);

	if(id < 0 || id >= MAX_SHAKES) {
		Py_INCREF(Py_None);
		return Py_None;
	}

	if(devicelist[id] != NULL) {
		int xyz[3];
		PyObject* obj;

		shake_gyr(devicelist[id], xyz);
		obj = Py_BuildValue("[i, i, i]", xyz[0], xyz[1], xyz[2]);
		Py_INCREF(obj);
		return obj;
	}

	Py_INCREF(Py_None);
	return Py_None;
}

static PyObject* pyshake_magx(PyObject* self, PyObject* args) {
	int id;

	PyArg_ParseTuple(args, "i", &id);

	if(id < 0 || id >= MAX_SHAKES)
		return Py_BuildValue("i", SHAKE_ERROR);

	if(devicelist[id] != NULL)
		return Py_BuildValue("i", shake_magx(devicelist[id]));

	return Py_BuildValue("i", SHAKE_ERROR);
}

static PyObject* pyshake_magy(PyObject* self, PyObject* args) {
	int id;

	PyArg_ParseTuple(args, "i", &id);

	if(id < 0 || id >= MAX_SHAKES)
		return Py_BuildValue("i", SHAKE_ERROR);

	if(devicelist[id] != NULL)
		return Py_BuildValue("i", shake_magy(devicelist[id]));

	return Py_BuildValue("i", SHAKE_ERROR);
}

static PyObject* pyshake_magz(PyObject* self, PyObject* args) {
	int id;

	PyArg_ParseTuple(args, "i", &id);

	if(id < 0 || id >= MAX_SHAKES)
		return Py_BuildValue("i", SHAKE_ERROR);

	if(devicelist[id] != NULL)
		return Py_BuildValue("i", shake_magz(devicelist[id]));

	return Py_BuildValue("i", SHAKE_ERROR);
}

static PyObject* pyshake_mag(PyObject* self, PyObject* args) {
	int id;

	PyArg_ParseTuple(args, "i", &id);

	if(id < 0 || id >= MAX_SHAKES) {
		Py_INCREF(Py_None);
		return Py_None;
	}

	if(devicelist[id] != NULL) {
		int xyz[3];
		PyObject* obj;

		shake_mag(devicelist[id], xyz);
		obj = Py_BuildValue("[i, i, i]", xyz[0], xyz[1], xyz[2]);
		Py_INCREF(obj);
		return obj;
	}

	Py_INCREF(Py_None);
	return Py_None;
}

static PyObject* pyshake_heading(PyObject* self, PyObject* args) {
	int id;

	PyArg_ParseTuple(args, "i", &id);

	if(id < 0 || id >= MAX_SHAKES)
		return Py_BuildValue("i", SHAKE_ERROR);

	if(devicelist[id] != NULL)
		return Py_BuildValue("i", shake_heading(devicelist[id]));

	return Py_BuildValue("i", SHAKE_ERROR);
}

static PyObject* pyshake_sk7_roll_pitch_heading(PyObject* self, PyObject* args) {
	int id;

	PyArg_ParseTuple(args, "i", &id);

	if(id < 0 || id >= MAX_SHAKES) {
		Py_INCREF(Py_None);
		return Py_None;
	}

	if(devicelist[id] != NULL) {
		int rph[3];
		PyObject* obj;

		sk7_roll_pitch_heading(devicelist[id], rph);
		obj = Py_BuildValue("[i, i, i]", rph[0], rph[1], rph[2]);
		Py_INCREF(obj);
		return obj;
	}

	Py_INCREF(Py_None);
	return Py_None;
}

static PyObject* pyshake_sk6_cap0(PyObject* self, PyObject* args) {
	int id;

	PyArg_ParseTuple(args, "i", &id);

	if(id < 0 || id >= MAX_SHAKES)
		return Py_BuildValue("i", SHAKE_ERROR);

	if(devicelist[id] != NULL)
		return Py_BuildValue("i", sk6_cap0(devicelist[id]));

	return Py_BuildValue("i", SHAKE_ERROR);
}

static PyObject* pyshake_sk6_cap1(PyObject* self, PyObject* args) {
	int id;

	PyArg_ParseTuple(args, "i", &id);

	if(id < 0 || id >= MAX_SHAKES)
		return Py_BuildValue("i", SHAKE_ERROR);

	if(devicelist[id] != NULL)
		return Py_BuildValue("i", sk6_cap1(devicelist[id]));

	return Py_BuildValue("i", SHAKE_ERROR);
}

static PyObject* pyshake_sk6_cap(PyObject* self, PyObject* args) {
	int id;

	PyArg_ParseTuple(args, "i", &id);

	if(id < 0 || id >= MAX_SHAKES) {
		Py_INCREF(Py_None);
		return Py_None;
	}

	if(devicelist[id] != NULL) {
		PyObject* obj;
		int prox[2];

		sk6_cap(devicelist[id], prox);
		obj = Py_BuildValue("[i, i]", prox[0], prox[1]);
		Py_INCREF(obj);
		return obj;
	}

	Py_INCREF(Py_None);
	return Py_None;
}

static PyObject* pyshake_sk7_cap(PyObject* self, PyObject* args) {
	int id;

	PyArg_ParseTuple(args, "i", &id);

	if(id < 0 || id >= MAX_SHAKES) {
		Py_INCREF(Py_None);
		return Py_None;
	}

	if(devicelist[id] != NULL) {
		PyObject* obj;
		int prox[12];

		sk7_cap(devicelist[id], prox);
		obj = Py_BuildValue("[i, i, i, i, i, i, i, i, i, i, i, i]", prox[0], prox[1], prox[2], prox[3], prox[4], prox[5], prox[6], prox[7], prox[8], prox[9], prox[10], prox[11]);
		Py_INCREF(obj);
		return obj;
	}

	Py_INCREF(Py_None);
	return Py_None;
}

static PyObject* pyshake_sk7_cap_ext(PyObject* self, PyObject* args) {
	int id, blocks;

	PyArg_ParseTuple(args, "ii", &id, &blocks);

	if(id < 0 || id >= MAX_SHAKES) {
		Py_INCREF(Py_None);
		return Py_None;
	}

	if(devicelist[id] != NULL) {
		int prox[24];

		if(sk7_cap_ext(devicelist[id], blocks, prox) == SHAKE_ERROR) {
			Py_INCREF(Py_None);
			return Py_None;
		}
		PyObject* result = NULL;
		if(blocks == 0 || blocks == 1) {
			result = PyList_New(12);
			for(int j=0;j<12;j++) {
				PyList_SetItem(result, j, PyLong_FromLong(prox[j]));
			}
		} else {
			result = PyList_New(24);
			for(int j=0;j<24;j++) {
				PyList_SetItem(result, j, PyLong_FromLong(prox[j]));
			}
		}
			
		Py_INCREF(result);
		return result;
	}

	Py_INCREF(Py_None);
	return Py_None;
}

static PyObject* pyshake_analog0(PyObject* self, PyObject* args) {
	int id;

	PyArg_ParseTuple(args, "i", &id);

	if(id < 0 || id >= MAX_SHAKES)
		return Py_BuildValue("i", SHAKE_ERROR);

	if(devicelist[id] != NULL)
		return Py_BuildValue("i", shake_analog0(devicelist[id]));

	return Py_BuildValue("i", SHAKE_ERROR);
}

static PyObject* pyshake_analog1(PyObject* self, PyObject* args) {
	int id;

	PyArg_ParseTuple(args, "i", &id);

	if(id < 0 || id >= MAX_SHAKES)
		return Py_BuildValue("i", SHAKE_ERROR);

	if(devicelist[id] != NULL)
		return Py_BuildValue("i", shake_analog1(devicelist[id]));

	return Py_BuildValue("i", SHAKE_ERROR);
}

static PyObject* pyshake_analog(PyObject* self, PyObject* args) {
	int id;

	PyArg_ParseTuple(args, "i", &id);

	if(id < 0 || id >= MAX_SHAKES) {
		Py_INCREF(Py_None);
		return Py_None;
	}

	if(devicelist[id] != NULL) {
		PyObject* obj;
		int a0a1[2];

		shake_analog(devicelist[id], a0a1);
		obj = Py_BuildValue("[i, i]", a0a1[0], a0a1[1]);
		Py_INCREF(obj);
		return obj;
	}

	Py_INCREF(Py_None);
	return Py_None;
}

static PyObject* pyshake_read(PyObject* self, PyObject* args) {
	int id, reg;

	PyArg_ParseTuple(args, "ii", &id, &reg);

	if(id < 0 || id >= MAX_SHAKES) {
		Py_INCREF(Py_None);
		return Py_None;
	}

	if(devicelist[id] != NULL) {
		unsigned char value, ret;
		PyObject* obj;

		value = 0;
		ret = shake_read(devicelist[id], reg, &value);
		obj = Py_BuildValue("[i, i]", ret, value);
		Py_INCREF(obj);
		return obj;
	}

	Py_INCREF(Py_None);
	return Py_None;
}

static PyObject* pyshake_write(PyObject* self, PyObject* args) {
	int id, reg, val;

	PyArg_ParseTuple(args, "iii", &id, &reg, &val);

	if(id < 0 || id >= MAX_SHAKES) {
		Py_INCREF(Py_None);
		return Py_None;
	}

	if(devicelist[id] != NULL) {
		short ret;

		ret = shake_write(devicelist[id], reg, val);
		return Py_BuildValue("i", ret);
	}

	Py_INCREF(Py_None);
	return Py_None;
}

static PyObject* pyshake_playvib(PyObject* self, PyObject* args) {
	int id;
	int channel, profile;

	PyArg_ParseTuple(args, "iii", &id, &channel, &profile);

	if(id < 0 || id >= MAX_SHAKES) {
		return Py_BuildValue("i", SHAKE_ERROR);
	}

	if(devicelist[id] != NULL) {
		return Py_BuildValue("i", shake_playvib(devicelist[id], channel, profile));
	}

	return Py_BuildValue("i", SHAKE_ERROR);
}

static PyObject* pyshake_sk6_upload_vib_sample(PyObject* self, PyObject* args) {
	int id;
	unsigned profile;
	int sample[64], sample_length;
	PyObject* list;

	PyArg_ParseTuple(args, "iiO", &id, &profile, &list);

	if(id < 0 || id >= MAX_SHAKES) {
		//Py_INCREF(Py_None);
		//return Py_None;
		return Py_BuildValue("i", SHAKE_ERROR);
	}

	if(devicelist[id] != NULL) {
		int ret, pos = 0;

		sample_length = PyList_GET_SIZE(list);
		/* size is already checked in shake.py */

		/* copy each list value into sample array */
		for(pos = 0; pos < sample_length; pos++) {
			if(!PyInt_Check(PyList_GET_ITEM(list, pos))) {
				PyObject* ex = PyExc_TypeError;
				PyErr_SetString(ex, "Found non-integer argument in list!");
				return NULL;
			}
			sample[pos] = PyInt_AsLong(PyList_GET_ITEM(list, pos));
		}

		ret = sk6_upload_vib_sample(devicelist[id], profile, sample, sample_length / 2);
		return Py_BuildValue("i", ret);
	}

	//Py_INCREF(Py_None);
	//return Py_None;
	return Py_BuildValue("i", SHAKE_ERROR);
}

static PyObject* pyshake_upload_vib_sample_extended(PyObject* self, PyObject* args) {
	int id;
	unsigned profile;
	int sample[64], sample_length;
	PyObject* list;
	int mode, freq, duty;

	PyArg_ParseTuple(args, "iiOiii", &id, &profile, &list, &mode, &freq, &duty);

	if(id < 0 || id >= MAX_SHAKES) {
		//Py_INCREF(Py_None);
		//return Py_None;
		return Py_BuildValue("i", SHAKE_ERROR);
	}

	if(devicelist[id] != NULL) {
		int ret, pos = 0;

		sample_length = PyList_GET_SIZE(list);
		/* size is already checked in shake.py */

		/* copy each list value into sample array */
		for(pos = 0; pos < sample_length; pos++) {
			if(!PyInt_Check(PyList_GET_ITEM(list, pos))) {
				PyObject* ex = PyExc_TypeError;
				PyErr_SetString(ex, "Found non-integer argument in list!");
				return NULL;
			}
			sample[pos] = PyInt_AsLong(PyList_GET_ITEM(list, pos));
		}

		ret = shake_upload_vib_sample_extended(devicelist[id], profile, sample, sample_length / 2, mode, freq, duty);
		return Py_BuildValue("i", ret);
	}

	//Py_INCREF(Py_None);
	//return Py_None;
	return Py_BuildValue("i", SHAKE_ERROR);
}

static PyObject* pyshake_read_temperature(PyObject* self, PyObject* args) {
	int id;
	float val;
	PyArg_ParseTuple(args, "i", &id);

	if(id < 0 || id >= MAX_SHAKES) {
		Py_INCREF(Py_None);
		return Py_None;
	}

	if(devicelist[id] != NULL) {
		val = shake_read_temperature(devicelist[id]);
		return Py_BuildValue("f", val);
	}

	Py_INCREF(Py_None);
	return Py_None;
}

static PyObject* pyshake_register_event_callback(PyObject* self, PyObject* args) {
    PyObject *callback;
	PyObject *device_object;
	int id;
	PyObject *result = NULL;

    if (PyArg_ParseTuple(args, "iOO", &id, &callback, &device_object)) {
		if(id < 0 || id >= MAX_SHAKES) {
			Py_INCREF(Py_None);
			return Py_None;
		}	

		// if unregistering the callback
		if(callback == Py_None) {
			Py_INCREF(Py_None);
			result = Py_None;
			shake_register_event_callback(devicelist[id], NULL);
		} else {
			// registering a new callback
			if (PyCallable_Check(callback) == 0) {
				PyErr_SetString(PyExc_TypeError, "parameter must be callable");
				Py_INCREF(Py_None);
				return Py_None;
			}

			Py_XINCREF(callback);         /* Add a reference to new callback */
			Py_XDECREF(callbacks[id][0]);  /* Dispose of previous callback */
			callbacks[id][0] = callback;       /* Remember new callback */
			callbacks[id][1] = device_object;
			shake_register_event_callback(devicelist[id], main_callback);

			
			//main_callback(devicelist[id], 1);
			//result = PyEval_CallFunction(callbacks[id], "(i)", 3);
		}
    }
	result = Py_BuildValue("i", SHAKE_SUCCESS);
    return result;
}

static PyObject* pyshake_logging_play(PyObject* self, PyObject* args) {
	int id;
	char* filename;

	PyArg_ParseTuple(args, "is", &id, &filename);

	if(id < 0 || id >= MAX_SHAKES)
		return Py_BuildValue("i", SHAKE_ERROR);

	if(devicelist[id] != NULL)
		return Py_BuildValue("i", shake_logging_play(devicelist[id], filename));

	return Py_BuildValue("i", SHAKE_ERROR);
}

static PyObject* pyshake_logging_packets_read(PyObject* self, PyObject* args) {
	int id;

	PyArg_ParseTuple(args, "i", &id);

	if(id < 0 || id >= MAX_SHAKES)
		return Py_BuildValue("i", SHAKE_ERROR);

	if(devicelist[id] != NULL)
		return Py_BuildValue("i", shake_logging_packets_read(devicelist[id]));

	return Py_BuildValue("i", SHAKE_ERROR);
}

static PyObject* pyshake_shaking_peakaccel(PyObject* self, PyObject* args) {
	int id;

	PyArg_ParseTuple(args, "i", &id);

	if(id < 0 || id >= MAX_SHAKES)
		return Py_BuildValue("i", SHAKE_ERROR);

	if(devicelist[id] != NULL)
		return Py_BuildValue("i", shake_shaking_peakaccel(devicelist[id]));

	return Py_BuildValue("i", SHAKE_ERROR);
}

static PyObject* pyshake_shaking_direction(PyObject* self, PyObject* args) {
	int id;

	PyArg_ParseTuple(args, "i", &id);

	if(id < 0 || id >= MAX_SHAKES)
		return Py_BuildValue("i", SHAKE_ERROR);

	if(devicelist[id] != NULL)
		return Py_BuildValue("i", shake_shaking_direction(devicelist[id]));

	return Py_BuildValue("i", SHAKE_ERROR);
}

static PyObject* pyshake_shaking_timestamp(PyObject* self, PyObject* args) {
	int id;

	PyArg_ParseTuple(args, "i", &id);

	if(id < 0 || id >= MAX_SHAKES)
		return Py_BuildValue("i", SHAKE_ERROR);

	if(devicelist[id] != NULL)
		return Py_BuildValue("i", shake_shaking_timestamp(devicelist[id]));

	return Py_BuildValue("i", SHAKE_ERROR);
}

static PyObject* pyshake_data_timestamp(PyObject* self, PyObject* args) {
	int id, sensor;

	PyArg_ParseTuple(args, "ii", &id, &sensor);

	if(id < 0 || id >= MAX_SHAKES)
		return Py_BuildValue("i", SHAKE_ERROR);

	if(devicelist[id] != NULL)
		return Py_BuildValue("i", shake_data_timestamp(devicelist[id], sensor));

	return Py_BuildValue("i", SHAKE_ERROR);
}

// (shake_device* sh, unsigned short address, short* sample_data, unsigned short sample_len);

static PyObject* pyshake_upload_audio_sample(PyObject* self, PyObject* args) {
	int id;
	unsigned address;
	short sample[1056], sample_length;
	PyObject* list;

	PyArg_ParseTuple(args, "iiO", &id, &address, &list);

	if(id < 0 || id >= MAX_SHAKES) {
		Py_INCREF(Py_None);
		return Py_None;
	}

	if(devicelist[id] != NULL) {
		int ret, pos = 0;

		sample_length = PyList_GET_SIZE(list);
		/* size is already checked in shake.py */

		/* copy each list value into sample array */
		memset(sample, 0, 1056);
		for(pos = 0; pos < sample_length; pos++) {
			if(!PyInt_Check(PyList_GET_ITEM(list, pos))) {
				PyObject* ex = PyExc_TypeError;
				PyErr_SetString(ex, "Found non-integer argument in list!");
				return NULL;
			}
			sample[pos] = PyInt_AsLong(PyList_GET_ITEM(list, pos));
		}

		ret = shake_upload_audio_sample(devicelist[id], address, sample, sample_length / 2);
		return Py_BuildValue("i", ret);
	}

	Py_INCREF(Py_None);
	return Py_None;
}

static PyObject* pyshake_play_audio_sample(PyObject* self, PyObject* args) {
	int id;
	unsigned start_addr, end_addr, amplitude;

	PyArg_ParseTuple(args, "iiii", &id, &start_addr, &end_addr, &amplitude);

	if(id < 0 || id >= MAX_SHAKES) {
		Py_INCREF(Py_None);
		return Py_None;
	}

	if(devicelist[id] != NULL) {
		int ret = shake_play_audio_sample(devicelist[id], start_addr, end_addr, amplitude);
		return Py_BuildValue("i", ret);
	}

	Py_INCREF(Py_None);
	return Py_None;
}

static PyObject* pyshake_register_audio_callback(PyObject* self, PyObject* args) {
	PyObject *callback;
	PyObject *device_object;
	int id;
	PyObject *result = NULL;

    if (PyArg_ParseTuple(args, "iOO", &id, &callback, &device_object)) {
		if(id < 0 || id >= MAX_SHAKES) {
			Py_INCREF(Py_None);
			return Py_None;
		}	

		// if unregistering the callback
		if(callback == Py_None) {
			//printf("register_audio_callback : clearing\n");
			unregister_audio_callback(id);
		} else {
			// registering a new callback
			if (PyCallable_Check(callback) == 0) {
				PyErr_SetString(PyExc_TypeError, "parameter must be callable");
				Py_INCREF(Py_None);
				return Py_None;
			}

			//printf("Setting audio callback\n");

			Py_XINCREF(callback);         /* Add a reference to new callback */
			Py_XDECREF(audio_callbacks[id][0]);  /* Dispose of previous callback */
			audio_callbacks[id][0] = callback;       /* Remember new callback */
			audio_callbacks[id][1] = device_object;
			audio_callbacks[id][2] = NULL; //PyList_New(SHAKE_AUDIO_DATA_LEN);
			audio_callbacks[id][3] = NULL; //PyList_New(SHAKE_AUDIO_DATA_LEN);
			shake_register_audio_callback(devicelist[id], main_audio_callback);
			
		}
    }
	result = Py_BuildValue("i", SHAKE_SUCCESS);
    return result;
}

static PyObject* pyshake_heart_rate(PyObject* self, PyObject* args) {
	PyObject *callback;
	PyObject *device_object;
	int id;
	PyObject *result = NULL;

    if (PyArg_ParseTuple(args, "i", &id)) {
		if(id < 0 || id >= MAX_SHAKES) {
			Py_INCREF(Py_None);
			return Py_None;
		}	

		return Py_BuildValue("i", shake_heart_rate(devicelist[id]));
	}

	return Py_BuildValue("i", SHAKE_ERROR);
}

static PyObject* pyshake_exp_upload_vib_sample(PyObject* self, PyObject* args) {
	int id;
	unsigned address;
	unsigned char sample[1056];
	unsigned sample_length;
	PyObject* list;

	PyArg_ParseTuple(args, "iiO", &id, &address, &list);

	if(id < 0 || id >= MAX_SHAKES) {
		Py_INCREF(Py_None);
		return Py_None;
	}

	if(devicelist[id] != NULL) {
		int ret, pos = 0;

		sample_length = PyList_GET_SIZE(list);
		/* size is already checked in shake.py */

		/* copy each list value into sample array */
		memset(sample, 0, 1056);
		for(pos = 0; pos < sample_length; pos++) {
			if(!PyInt_Check(PyList_GET_ITEM(list, pos))) {
				PyObject* ex = PyExc_TypeError;
				PyErr_SetString(ex, "Found non-integer argument in list!");
				return NULL;
			}
			sample[pos] = PyInt_AsLong(PyList_GET_ITEM(list, pos));
		}

		ret = shake_exp_upload_vib_sample(devicelist[id], address, (char*)sample, sample_length);
		return Py_BuildValue("i", ret);
	}

	Py_INCREF(Py_None);
	return Py_None;
}

static PyObject* pyshake_exp_play_vib_sample(PyObject* self, PyObject* args) {
	int id;
	unsigned start_addr, end_addr, amplitude;

	PyArg_ParseTuple(args, "iiii", &id, &start_addr, &end_addr, &amplitude);

	if(id < 0 || id >= MAX_SHAKES) {
		Py_INCREF(Py_None);
		return Py_None;
	}

	if(devicelist[id] != NULL) {
		int ret = shake_exp_play_vib_sample(devicelist[id], start_addr, end_addr, amplitude);
		return Py_BuildValue("i", ret);
	}

	Py_INCREF(Py_None);
	return Py_None;
}

static PyObject* pyshake_rfid_tid(PyObject* self, PyObject* args) {
	int id;

    if (PyArg_ParseTuple(args, "i", &id)) {
		if(id < 0 || id >= MAX_SHAKES) {
			Py_INCREF(Py_None);
			return Py_None;
		}	

		return Py_BuildValue("s", shake_rfid_tid(devicelist[id]));
	}

	return Py_BuildValue("s", "");
}

static PyObject* pyshake_rfid_scan(PyObject* self, PyObject* args) {
	int id;

    if (PyArg_ParseTuple(args, "i", &id)) {
		if(id < 0 || id >= MAX_SHAKES) {
			Py_INCREF(Py_None);
			return Py_None;
		}	

		return Py_BuildValue("i", shake_rfid_scan(devicelist[id]));
	}

	return Py_BuildValue("i", SHAKE_ERROR);
}
