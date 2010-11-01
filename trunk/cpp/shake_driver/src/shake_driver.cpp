
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


#include <stdlib.h>
#include <string.h>
#include "shake_driver.h"
#include "shake_serial_win32.h"
#include "shake_serial_usb.h"
#ifdef SHAKE_RFCOMM_SUPPORTED
	#include "shake_rfcomm.h"
#endif
#include "shake_packets.h"
#include "shake_thread.h"
#include "shake_mulaw.h"

#include "shake_io.h"

#include "SHAKE.h"
#include "shake_parsing.h"
#include "SK6_parsing.h"
#include "shake_packets.h"
#include "SK6_packets.h"
#include "SK7_parsing.h"

#include "SK6.h"
#include "SK7.h"

/* This thread deals with callbacks to an application to avoid blocking the main parsing thread */
#ifdef _WIN32
unsigned long __stdcall shake_callback_thread(void* shakedev) {
#else
void* shake_callback_thread(void *shakedev) {
#endif
	shake_device* dev;
	shake_device_private* devpriv;

	dev = (shake_device*)shakedev;
	devpriv = (shake_device_private*)dev->priv;

	while(!devpriv->cthread_done) {
		if(shake_thread_wait(&(devpriv->thread), 60, 0, CALLBACK_THREAD))

			if(devpriv->cthread_done)
				break;

			if(devpriv->navcb)
				devpriv->navcb(dev, devpriv->lastevent);
			#ifdef _WIN32
			else if(devpriv->navcb_STDCALL)
				devpriv->navcb_STDCALL(dev, devpriv->lastevent);
			#endif
	}

	#ifdef _WIN32
	return 1;
	#else
	pthread_exit((void*)1);
	#endif
}


/*	this function is where all the parsing of incoming data is done, running in a dedicated thread */
#ifdef _WIN32
unsigned long __stdcall shake_read_thread(void* shakedev) {
#else
void* shake_read_thread(void *shakedev) {
#endif
	shake_device* dev;
	shake_device_private* devpriv;
	char packetbuf[256];	// buffer for incoming packet
	int packet_type;		// type of packet

	dev = (shake_device*)shakedev;
	devpriv = (shake_device_private*)dev->priv;

	/* loop while thread hasn't been told to exit */
	while(!devpriv->rthread_done) {
		memset(packetbuf, 0, 256);
		packet_type = SHAKE_BAD_PACKET;

		SHAKE_DBG("--------------------- NEW LOOP\n");
		devpriv->rthread_exit = 1;

		/* the job of this loop is:
		*	a) make sure a valid header has been detected before continuing
		*	b) classify the header 
		*/
		do {
			packet_type = devpriv->shake->get_next_packet(packetbuf, 256);
		} while(!devpriv->rthread_done && (packet_type == SHAKE_BAD_PACKET));

		if(devpriv->rthread_done)
			shake_thread_exit(105);

		devpriv->rthread_exit = 4;

		devpriv->shake->parse_packet(packetbuf, packet_type);
	}

	devpriv->rthread_exit = TRUE;

	#ifdef _WIN32
	return 1;
	#else
	pthread_exit((void*)1);
	#endif
}

shake_device* shake_init_internal(shake_conn_data* scd) {
	shake_device* dev;
	shake_device_private* devpriv;
	int elapsed;
	TCHAR eventname[20], eventname2[20], eventname3[20];

	if(scd->devtype != SHAKE_SK7 && scd->devtype != SHAKE_SK6)
		return NULL;

	#ifdef _WIN32
	if((scd->type == SHAKE_CONN_VIRTUAL_SERIAL_WIN32) && (scd->com_port != -1 && (scd->com_port < 1 || scd->com_port > 100)))
		return NULL;
	#endif
	if((scd->type == SHAKE_CONN_RFCOMM_I64) && (scd->btaddr == 0))
		return NULL;
	if((scd->type == SHAKE_CONN_RFCOMM_STR) && scd->btaddr_str == NULL)
		return NULL;

	// setup serial port etc
	dev = (shake_device*)malloc(sizeof(shake_device));
	dev->priv = devpriv = (shake_device_private*)(void*)malloc(sizeof(shake_device_private));
	dev->handle = ++shake_handle_count;
	memset(devpriv, 0, sizeof(shake_device_private));

	devpriv->port.comms_type = -1;
	#ifdef _WIN32
		if(scd->type == SHAKE_CONN_VIRTUAL_SERIAL_WIN32) {
			devpriv->port.comms_type = SHAKE_CONN_VIRTUAL_SERIAL_WIN32;
			
			if(shake_open_serial_win32(&(devpriv->port.serial_win32), scd->com_port, scd->devtype) == NULL) {
				free(devpriv);
				free(dev);
				return NULL;
			}
		} 
		#ifdef SHAKE_RFCOMM_SUPPORTED
		if(scd->type == SHAKE_CONN_RFCOMM_I64 || scd->type == SHAKE_CONN_RFCOMM_STR) {
			devpriv->port.comms_type = scd->type;
			if(scd->type == SHAKE_CONN_RFCOMM_I64) {
				if(shake_open_rfcomm_i64(&(devpriv->port.rfcomm), scd->btaddr) == NULL) {
					free(devpriv);
					free(dev);
					return NULL;
				}
			} else {
				if(shake_open_rfcomm_str(&(devpriv->port.rfcomm), scd->btaddr_str) == NULL) {
					free(devpriv);
					free(dev);
					return NULL;
				}
			}
		}
		#endif
		// open the input/output files 
		// BINARY mode ("b") important on Windows to avoid fread returning on newlines and
		// other silly stuff
		if(scd->type == SHAKE_CONN_DEBUGFILE) {
			devpriv->port.comms_type = scd->type;
			devpriv->port.dbg_read = fopen(scd->readfile, "rb");
			if(devpriv->port.dbg_read == NULL) 
				return NULL;
			devpriv->port.dbg_write = fopen(scd->writefile, "wb");
			if(devpriv->port.dbg_write == NULL) {
				fclose(devpriv->port.dbg_read);
				return NULL;
			}
		}
	#else
		#ifndef __APPLE__
		if(scd->type == SHAKE_CONN_RFCOMM_I64 || scd->type == SHAKE_CONN_RFCOMM_STR) {
			devpriv->port.comms_type = scd->type;
			if(scd->type == SHAKE_CONN_RFCOMM_I64) {
				if(shake_open_rfcomm_i64(&(devpriv->port.rfcomm), scd->btaddr) == NULL) {
					free(devpriv);
					free(dev);
					return NULL;
				}
			} else {
				if(shake_open_rfcomm_str(&(devpriv->port.rfcomm), scd->btaddr_str) == NULL) {
					free(devpriv);
					free(dev);
					return NULL;
				}
			}
		}
		#endif
		if(scd->type == SHAKE_CONN_USB_SERIAL) {
			devpriv->port.comms_type = scd->type;
			printf("Opening USB serial port: %s\n", scd->usbdev);
			if(shake_open_serial_usb(&(devpriv->port.serial_usb), scd->usbdev, scd->devtype) == NULL) {
				free(devpriv);
				free(dev);
				return NULL;
			}
		}
	#endif

	if(devpriv->port.comms_type == -1) {
		free(devpriv);
		free(dev);
		return NULL;
	}

	// name of event to be used for signalling the arrival of ACK/NAK packets
	// (this is only used on Windows)
	#ifdef _WIN32
	_stprintf(eventname, _T("shakecmd%d"), dev->handle);
	_stprintf(eventname2, _T("shakecb%d"), dev->handle);
	_stprintf(eventname3, _T("shakeau%d"), dev->handle);
	#endif

	// start thread to read from serial port
	devpriv->rthread_done = FALSE;
	devpriv->rthread_exit = FALSE;
	devpriv->cthread_done = FALSE;
	//devpriv->synced = FALSE;

	devpriv->navcb  = NULL;
	devpriv->navcb_STDCALL = NULL;
	devpriv->log = NULL;
	devpriv->data_recv = 0;
	devpriv->wait_for_acks = 1; // NOTE
	devpriv->hwrev = devpriv->fwrev = devpriv->bluetoothfwrev = 0.0;
	devpriv->device_type = scd->devtype;

	sprintf(devpriv->playback_packet, "$STRW");

	if(scd->devtype == SHAKE_SK7) {
		devpriv->shake = new SK7(dev, devpriv);
	} else {
		devpriv->shake = new SK6(dev, devpriv);
	}
	memset(&(devpriv->shake->data), 0, sizeof(sk_sensor_data));

	// launch the threads used internally
	shake_thread_init(&(devpriv->thread), 
		shake_read_thread, (void*)dev, eventname, 
		shake_callback_thread, (void*)dev, eventname2,
		NULL, NULL, NULL);

	return dev;
}

/*	Main initialisation function for setting up a link to a SHAKE device.
*	On Windows:
*		<com_port> is the number of the COM port to use for Bluetooth comms. 
*		If you use -1, the correct port number will be searched for in the
*		Windows registry, but this isn't guaranteed to work.
*	On Linux:
*		<btaddr> is the Bluetooth MAC address to connect to.
*		TODO: support device names eg "SHAKE_ROO"
*
*	Returns NULL on failure, otherwise a handle to the SHAKE device. */
#ifdef _WIN32
SHAKE_API shake_device* shake_init_device(int com_port, int device_type) {
	shake_conn_data scd = { 0 };
	scd.type = SHAKE_CONN_VIRTUAL_SERIAL_WIN32;
	scd.com_port = com_port;
	scd.devtype = device_type;
	return shake_init_internal(&scd);
}
#endif

#ifdef SHAKE_RFCOMM_SUPPORTED
SHAKE_API shake_device* shake_init_device_rfcomm_i64(SHAKE_INT64 btaddr, int device_type) {
	shake_conn_data scd = { 0 };
	scd.type = SHAKE_CONN_RFCOMM_I64;
	scd.btaddr = btaddr;
	scd.devtype = device_type;
	return shake_init_internal(&scd);
}

SHAKE_API shake_device* shake_init_device_rfcomm_str(char* btaddr, int device_type) {
	shake_conn_data scd = { 0 };
	scd.type = SHAKE_CONN_RFCOMM_STR;
	if(btaddr == NULL || strlen(btaddr) > 20)
		return NULL;
	strcpy(scd.btaddr_str, btaddr);
	scd.devtype = device_type;
	return shake_init_internal(&scd);
}
#endif

#ifndef _WIN32
SHAKE_API shake_device* shake_init_device_usb_serial(char* usb_dev, int device_type) {
	shake_conn_data scd = { 0 };
	scd.type = SHAKE_CONN_USB_SERIAL;
	if(usb_dev == NULL || strlen(usb_dev) > 127) 
		return NULL;
	strcpy(scd.usbdev, usb_dev);
	scd.devtype = device_type;
	return shake_init_internal(&scd);
}
#endif

SHAKE_API shake_device* shake_init_device_DEBUGFILE(char* readfile, char* writefile, int device_type) {
	shake_conn_data scd = { 0 };
	scd.type = SHAKE_CONN_DEBUGFILE;
	if(readfile == NULL || writefile == NULL || strlen(readfile) > 255 || strlen(writefile) > 255) 
		return NULL;
	strcpy(scd.readfile, readfile);
	strcpy(scd.writefile, writefile);
	scd.devtype = device_type;
	return shake_init_internal(&scd);
}

int shake_close(shake_port* port) {
	#ifdef _WIN32
	if(port->comms_type == SHAKE_CONN_VIRTUAL_SERIAL_WIN32) {
		return shake_close_serial_win32(&(port->serial_win32));
	} 
	#endif
	#ifdef SHAKE_RFCOMM_SUPPORTED
	if(port->comms_type == SHAKE_CONN_RFCOMM_I64) {
		return shake_close_rfcomm(&(port->rfcomm));
	}
	else if(port->comms_type == SHAKE_CONN_RFCOMM_STR) {
		return shake_close_rfcomm(&(port->rfcomm));
	}
	#endif
	#ifndef _WIN32_
	if(port->comms_type == SHAKE_CONN_USB_SERIAL) {
		return shake_close_serial_usb(&(port->serial_usb));
	}
	#endif
	if(port->comms_type == SHAKE_CONN_DEBUGFILE) {
		fclose(port->dbg_read);
		fclose(port->dbg_write);
		port->dbg_read = NULL;
		port->dbg_write = NULL;
		return SHAKE_SUCCESS;
	}
	return SHAKE_ERROR;
}

/*	Call this function to close the link with a SHAKE device and free up any resources
*	used in maintaining the connection.
*	<sh> is a handle to a SHAKE device,
*
*	Returns SHAKE_SUCCESS on success, SHAKE_ERROR on failure. */
SHAKE_API int shake_free_device(shake_device* sh) {
	shake_device_private* devpriv;
	int c = 0;

	if(!sh) return SHAKE_ERROR;

	devpriv = (shake_device_private*)sh->priv;

	// stop callback thread
	devpriv->cthread_done = TRUE;
	shake_thread_signal(&(devpriv->thread), CALLBACK_THREAD);

	#ifdef _WIN32
	DWORD status = 0;
	while(GetExitCodeThread(&(devpriv->thread.cthread), &status)) {
		if(status != STILL_ACTIVE) {
			break;
		}
	}
	#endif

	devpriv->rthread_done = TRUE;

	#ifdef _WIN32
	while(GetExitCodeThread(&(devpriv->thread.rthread), &status)) {
		if(status != STILL_ACTIVE) {
			break;
		}
	}
	#endif

	shake_close(&(devpriv->port));
	while(!devpriv->rthread_exit) {
		shake_sleep(1);
	}

	free(devpriv);
	free(sh);

	devpriv = NULL;
	sh = NULL;
	return SHAKE_SUCCESS;
}

SHAKE_API float shake_info_firmware_revision(shake_device* sh) {
	if(!sh) return 0.0;

	shake_device_private* devpriv = (shake_device_private*)sh->priv;
	if(devpriv->fwrev == 0.0)
		shake_info_retrieve(sh);
	return devpriv->fwrev;
}

SHAKE_API float shake_info_hardware_revision(shake_device* sh) {
	if(!sh) return 0.0;

	shake_device_private* devpriv = (shake_device_private*)sh->priv;
	if(devpriv->hwrev == 0.0)
		shake_info_retrieve(sh);

	return ((shake_device_private*)sh->priv)->hwrev;
}

SHAKE_API char* shake_info_serial_number(shake_device* sh) {
	if(!sh) return NULL;

	shake_device_private* devpriv = (shake_device_private*)sh->priv;
	if(devpriv->hwrev == 0.0)
		shake_info_retrieve(sh);

	return ((shake_device_private*)sh->priv)->serial;
}

SHAKE_API float shake_info_bluetooth_firmware_revision(shake_device* sh) {
	if(!sh) return NULL;

	shake_device_private* devpriv = (shake_device_private*)sh->priv;
	if(devpriv->bluetoothfwrev == 0.0)
		shake_info_retrieve(sh);

	return ((shake_device_private*)sh->priv)->bluetoothfwrev;
}

SHAKE_API int shake_info_module_slot1(shake_device* sh) {
	if(!sh) return SHAKE_ERROR;

	shake_device_private* devpriv = (shake_device_private*)sh->priv;
	if(devpriv->hwrev == 0.0)
		shake_info_retrieve(sh);

	return ((shake_device_private*)sh->priv)->modules[0];
}

SHAKE_API int shake_info_module_slot2(shake_device* sh) {
		if(!sh) return SHAKE_ERROR;

	shake_device_private* devpriv = (shake_device_private*)sh->priv;
	if(devpriv->hwrev == 0.0)
		shake_info_retrieve(sh);

	return ((shake_device_private*)sh->priv)->modules[1];
}

SHAKE_API int shake_info_module(shake_device* sh, int slot_number) {
	if(slot_number < 0 || slot_number > 3)
		return SHAKE_ERROR;

	return ((shake_device_private*)sh->priv)->modules[slot_number];
}

SHAKE_API char* shake_info_module_name(int module) {
	int i;
	for(i=SK6_MODULE_NONE;i<SK6_MODULE_LAST;i++) {
		if(module == i)
			return SK6_modules[i];
	}
	for(i=SK7_MODULE_NONE;i<SK7_MODULE_LAST;i++) {
		if(module == i)
			return SK7_modules[i-SK7_MODULE_NONE];
	}
	return unknown_module;
}

SHAKE_API int shake_info_retrieve(shake_device* sh) {
	if(!sh) return SHAKE_ERROR;

	int ret = shake_write_data_request(sh, 0x01);
	if(ret == SHAKE_SUCCESS) {
		shake_device_private* devpriv = (shake_device_private*)sh->priv;
		int count = 0;
		while(count++ < 1000 && devpriv->fwrev == 0.0)
			shake_sleep(1);
	}
	return ret;
}

SHAKE_API int shake_factory_reset(shake_device* sh, int repeat) {
	shake_device_private* devpriv;
	int i, repeat_count = 1;

	if(!sh) return SHAKE_ERROR;

	if(repeat > 1)
		repeat_count = repeat;

	shake_wait_for_acks(sh, 0);

	for(i=0;i<repeat_count;i++) {
		shake_write(sh, 0x0000, 0xFB);	// Pwr Ctrl1
		shake_write(sh, 0x0001, 0xFB);	// Pwr Ctrl2
		shake_write(sh, 0x0002, 0x08);	// ODF
		shake_write(sh, 0x0003, 0x00);	// Comms
		shake_write(sh, 0x0004, 0x00);	// Acc flags
		shake_write(sh, 0x0005, 0x00);	// Acc flags
		shake_write(sh, 0x0006, 0x00);	// Data cal mod
		shake_write(sh, 0x0007, 0x00);	// X-Axes en
		shake_write(sh, 0x0008, 0x00);	// ACC
		shake_write(sh, 0x0009, 0x00);	// GYRO
		shake_write(sh, 0x000a, 0x00);	// MAG
		shake_write(sh, 0x000b, 0x00);	// CH
		shake_write(sh, 0x000c, 0x00);	// CS0
		shake_write(sh, 0x000d, 0x00);	// CS1
		shake_write(sh, 0x000e, 0x00);	// AIN0
		shake_write(sh, 0x000f, 0x00);	// AIN1
		shake_write(sh, 0x0010, 0x02);	// Gyro OS TC enable
		shake_write(sh, 0x0011, 0x00);	// TC unused
		shake_write(sh, 0x0012, 0x00);	// Streaming
		shake_write(sh, 0x0013, 0x00);	// Streaming
		shake_write(sh, 0x0014, 0x01);	// A Filt
		shake_write(sh, 0x0015, 0x01);	// G Filt
		shake_write(sh, 0x0016, 0x01);	// M Filt
		shake_write(sh, 0x0018, 0x01);	// CS0 Filt
		shake_write(sh, 0x0019, 0x01);	// CS1 Filt
		shake_write(sh, 0x001a, 0x01);	// AIN0 Filt
		shake_write(sh, 0x001b, 0x01);	// AIN1 Filt
		shake_write(sh, 0x001e, 0x00);	// AO1 off
		shake_write(sh, 0x001f, 0x00);	// AO1 off
		shake_write(sh, 0x0026, 0x00);	// E0x off
		shake_write(sh, 0x0030, 0x00);	// shake detect off
		shake_write(sh, 0x003a, 0x00);	// H01 off
		shake_write(sh, 0x003c, 0x00);	// R01 off

		int sample1[] = { 0xFF, 0x03, 0x00, 0x30, 0xFF, 0x03, 0x01, 0x30 };
		sk6_upload_vib_sample(sh, 0x01, sample1, 4);

		int sample2[] = { 0xFF, 0x03, 0x00, 0x30, 0xFF, 0x03, 0x01, 0x30, 0xFF, 0x03, 0x00, 0x30, 0xFF, 0x03, 0x01, 0x30, 0xFF, 0x03, 0x00, 0x30, 0xFF, 0x03, 0x01, 0x30 };
		sk6_upload_vib_sample(sh, 0x01, sample2, 12);

		int sample3[] = { 0xFF, 0x03, 0x00, 0x30, 0xFF, 0x03, 0x01, 0x30, 0xFF, 0x03, 0x00, 0x30, 0xFF, 0x03, 0x01, 0x30, 0xFF, 0x03, 0x00, 0x30, 0xFF, 0x03, 0x01, 0x30 };
		sk6_upload_vib_sample(sh, 0x01, sample3, 12);

		int sample4[] = { 0xFF, 0x03, 0x00, 0x30, 0xFF, 0x03, 0x01, 0x30 };
		sk6_upload_vib_sample(sh, 0x01, sample4, 4);
	}

	return SHAKE_SUCCESS;
}

SHAKE_API int shake_logging_play(shake_device* sh, char* output_filename) {
	shake_device_private* devpriv;

	if(!sh || !output_filename) return SHAKE_ERROR;

	devpriv = (shake_device_private*)sh->priv;

	// close any existing file pointer
	if(devpriv->log) 
		fclose(devpriv->log);
	devpriv->packets_read = 0;

	// open log file
	if((devpriv->log = fopen(output_filename, "w")) == NULL)
		return SHAKE_ERROR;

	// send logging playback command
	if(shake_write(sh, SHAKE_VO_REG_LOGGING_CTRL, SHAKE_LOGGING_PLAY) == SHAKE_ERROR) {
		fclose(devpriv->log);
		return SHAKE_ERROR;
	}
	return SHAKE_SUCCESS;
}

SHAKE_API int shake_logging_pause(shake_device* sh) {
	if(!sh) return SHAKE_ERROR;

	// send logging pause command
	if(shake_write(sh, SHAKE_VO_REG_LOGGING_CTRL, SHAKE_LOGGING_PAUSE) == SHAKE_ERROR) {
		return SHAKE_ERROR;
	}

	return SHAKE_SUCCESS;
}

SHAKE_API int shake_logging_stop(shake_device* sh) {
	shake_device_private* devpriv;

	if(!sh) return SHAKE_ERROR;

	// send logging stop command
	if(shake_write(sh, SHAKE_VO_REG_LOGGING_CTRL, SHAKE_LOGGING_STOP) == SHAKE_ERROR) {
		return SHAKE_ERROR;
	}

	// close output file if playback was in progress
	devpriv = (shake_device_private*)sh->priv;
	if(devpriv->log) {
		fclose(devpriv->log);
	}

	return SHAKE_SUCCESS;
}

SHAKE_API int shake_logging_record(shake_device* sh) {
	if(!sh) return SHAKE_ERROR;

	// send logging record command
	if(shake_write(sh, SHAKE_VO_REG_LOGGING_CTRL, SHAKE_LOGGING_RECORD) == SHAKE_ERROR) {
		return SHAKE_ERROR;
	}

	return SHAKE_SUCCESS;
}

SHAKE_API int shake_logging_reset(shake_device* sh) {
	if(!sh) return SHAKE_ERROR;

	// send logging reset command
	if(shake_write(sh, SHAKE_VO_REG_LOGGING_CTRL, SHAKE_LOGGING_RESET) == SHAKE_ERROR) {
		return SHAKE_ERROR;
	}

	return SHAKE_SUCCESS;
}


SHAKE_API int shake_logging_status(shake_device* sh) {
	unsigned char val;

	if(!sh) return SHAKE_ERROR;

	if(shake_read(sh, SHAKE_VO_REG_LOGGING_STATUS, &val) == SHAKE_ERROR)
		return SHAKE_ERROR;

	return val;
}	


SHAKE_API int shake_logging_memory_status(shake_device* sh) {
	unsigned char val;

	if(!sh) return SHAKE_ERROR;

	if(shake_read(sh, SHAKE_VO_REG_LOGGING_MEM, &val) == SHAKE_ERROR)
		return SHAKE_ERROR;

	return val;
}

SHAKE_API int shake_logging_packet_count(shake_device* sh) {
	unsigned char lsb = 0, msb = 0;

	if(!sh) return SHAKE_ERROR;

	if(shake_read(sh, SHAKE_NV_REG_LOGGING_PKT_LSB, &lsb) == SHAKE_ERROR)
		return SHAKE_ERROR;
	if(shake_read(sh, SHAKE_NV_REG_LOGGING_PKT_MSB, &msb) == SHAKE_ERROR)
		return SHAKE_ERROR;
	SHAKE_DBG("MSB: %d (%d) LSB: %d\n", msb, msb << 8, lsb);

	return 100 * ((msb << 8) + lsb);
}

SHAKE_API int shake_logging_packets_read(shake_device* sh) {
	if (!sh) return SHAKE_ERROR;

	return ((shake_device_private*)sh->priv)->packets_read;
}

SHAKE_API int sk7_logging_bt_power_down(shake_device* sh) {
	if(!sh) return SHAKE_ERROR;

	return shake_write(sh, SHAKE_VO_REG_LOGGING_CTRL, SHAKE_LOGGING_BT_POWER_DOWN);
}

SHAKE_API int shake_read_power_state(shake_device* sh, unsigned char* value) {
	if(!sh || !value) return SHAKE_ERROR;

	return shake_read(sh, SHAKE_NV_REG_POWER1, value);
}

SHAKE_API int shake_write_power_state(shake_device* sh, unsigned char value) {
	if(!sh) return SHAKE_ERROR;

	return shake_write(sh, SHAKE_NV_REG_POWER1, value);
}

SHAKE_API int shake_read_power_state_2(shake_device* sh, unsigned char* value) {
	if(!sh || !value) return SHAKE_ERROR;

	return shake_read(sh, SHAKE_NV_REG_POWER2, value);
}

SHAKE_API int shake_write_power_state_2(shake_device* sh, unsigned char value) {
	if(!sh) return SHAKE_ERROR;

	return shake_write(sh, SHAKE_NV_REG_POWER2, value);
}

SHAKE_API int shake_read_data_format(shake_device* sh, unsigned char* value) {
	if(!sh || !value) return SHAKE_ERROR;

	return shake_read(sh, SHAKE_NV_REG_DATAFMT, value);
}

SHAKE_API int shake_write_data_format(shake_device* sh, unsigned char value) {
	if(!sh) return SHAKE_ERROR;

	return shake_write(sh, SHAKE_NV_REG_DATAFMT, value);	
}

SHAKE_API int shake_read_calib_bypass(shake_device* sh, unsigned char* value) {
	if(!sh) return SHAKE_ERROR;

	return shake_read(sh, SHAKE_NV_REG_CALIB_BYPASS, value);
}

SHAKE_API int shake_write_calib_bypass(shake_device* sh, unsigned char value) {
	if(!sh) return SHAKE_ERROR;

	return shake_write(sh, SHAKE_NV_REG_CALIB_BYPASS, value);
}

SHAKE_API int shake_read_cx_bypass(shake_device* sh, unsigned char* value) {
	if(!sh) return SHAKE_ERROR;

	return shake_read(sh, SHAKE_NV_REG_CX_CALIB_BYPASS, value);
}

SHAKE_API int shake_write_cx_bypass(shake_device* sh, unsigned char value) {
	if(!sh) return SHAKE_ERROR;

	return shake_write(sh, SHAKE_NV_REG_CX_CALIB_BYPASS, value);
}

SHAKE_API int shake_read_temp_compensation(shake_device* sh, unsigned char* value) {
	if(!sh || !value) return SHAKE_ERROR;

	return shake_read(sh, SHAKE_NV_REG_TEMP_COMPENSATION, value);
}

SHAKE_API int shake_write_temp_compensation(shake_device* sh, unsigned char value) {
	if(!sh) return SHAKE_ERROR;

	return shake_write(sh, SHAKE_NV_REG_TEMP_COMPENSATION, value);
}

SHAKE_API int shake_read_packet_streaming(shake_device* sh, unsigned char* value) {
	if(!sh) return SHAKE_ERROR;

	return shake_read(sh, SHAKE_NV_REG_STREAM_DISABLE, value);
}

SHAKE_API int shake_write_packet_streaming(shake_device* sh, unsigned char value) {
	if(!sh) return SHAKE_ERROR;

	return shake_write(sh, SHAKE_NV_REG_STREAM_DISABLE, value);
}

SHAKE_API int shake_read_acc_config(shake_device* sh, unsigned char* value) {
	if(!sh || !value) return SHAKE_ERROR;

	return shake_read(sh, SHAKE_NV_REG_ACC_CONFIG, value);
}

SHAKE_API int shake_write_acc_config(shake_device* sh, unsigned char value) {
	if(!sh) return SHAKE_ERROR;

	return shake_write(sh, SHAKE_NV_REG_ACC_CONFIG, value);
}

SHAKE_API int sk6_read_cs0_inc(shake_device* sh, unsigned char* value) {
	if(!sh || !value) return SHAKE_ERROR;

	return shake_read(sh, SK6_NV_REG_CS0_INC, value);
}

SHAKE_API int sk6_write_cs0_inc(shake_device* sh, unsigned char value) {
	if(!sh) return SHAKE_ERROR;

	return shake_write(sh, SK6_NV_REG_CS0_INC, value);
}

SHAKE_API int sk6_read_cs0_dec(shake_device* sh, unsigned char* value) {
	if(!sh || !value) return SHAKE_ERROR;

	return shake_read(sh, SK6_NV_REG_CS0_DEC, value);
}

SHAKE_API int sk6_write_cs0_dec(shake_device* sh, unsigned char value) {
	if(!sh) return SHAKE_ERROR;

	return shake_write(sh, SK6_NV_REG_CS0_DEC, value);
}

SHAKE_API int sk6_read_cs0_inc_profile(shake_device* sh, unsigned char* value) {
	if(!sh || !value) return SHAKE_ERROR;

	return shake_read(sh, SK6_NV_REG_CS0_INC_PROFILE, value);
}

SHAKE_API int sk6_write_cs0_inc_profile(shake_device* sh, unsigned char value) {
	if(!sh) return SHAKE_ERROR;

	return shake_write(sh, SK6_NV_REG_CS0_INC_PROFILE, value);
}

SHAKE_API int sk6_read_cs0_dec_profile(shake_device* sh, unsigned char* value) {
	if(!sh || !value) return SHAKE_ERROR;

	return shake_read(sh, SK6_NV_REG_CS0_DEC_PROFILE, value);
}

SHAKE_API int sk6_write_cs0_dec_profile(shake_device* sh, unsigned char value) {
	if(!sh) return SHAKE_ERROR;

	return shake_write(sh, SK6_NV_REG_CS0_DEC_PROFILE, value);
}

SHAKE_API int sk6_read_cs1_inc(shake_device* sh, unsigned char* value) {
	if(!sh || !value) return SHAKE_ERROR;

	return shake_read(sh, SK6_NV_REG_CS1_INC, value);
}

SHAKE_API int sk6_write_cs1_inc(shake_device* sh, unsigned char value) {
	if(!sh) return SHAKE_ERROR;

	return shake_write(sh, SK6_NV_REG_CS1_INC, value);
}

SHAKE_API int sk6_read_cs1_dec(shake_device* sh, unsigned char* value) {
	if(!sh || !value) return SHAKE_ERROR;

	return shake_read(sh, SK6_NV_REG_CS1_DEC, value);
}

SHAKE_API int sk6_write_cs1_dec(shake_device* sh, unsigned char value) {
	if(!sh) return SHAKE_ERROR;

	return shake_write(sh, SK6_NV_REG_CS1_DEC, value);
}

SHAKE_API int sk6_read_cs1_inc_profile(shake_device* sh, unsigned char* value) {
	if(!sh || !value) return SHAKE_ERROR;

	return shake_read(sh, SK6_NV_REG_CS1_INC_PROFILE, value);
}

SHAKE_API int sk6_write_cs1_inc_profile(shake_device* sh, unsigned char value) {
	if(!sh) return SHAKE_ERROR;

	return shake_write(sh, SK6_NV_REG_CS1_INC_PROFILE, value);
}

SHAKE_API int sk6_read_cs1_dec_profile(shake_device* sh, unsigned char* value) {
	if(!sh || !value) return SHAKE_ERROR;

	return shake_read(sh, SK6_NV_REG_CS1_DEC_PROFILE, value);
}

SHAKE_API int sk6_write_cs1_dec_profile(shake_device* sh, unsigned char value) {
	if(!sh) return SHAKE_ERROR;

	return shake_write(sh, SK6_NV_REG_CS1_DEC_PROFILE, value);
}

SHAKE_API int sk6_read_cap_thresholds(shake_device* sh, unsigned char* values) {
	if(!sh || !values) return SHAKE_ERROR;

	int reg, offset;
	for(reg = SK6_NV_REG_CS0_INC, offset = 0; reg <= SK6_NV_REG_CS1_DEC_PROFILE; reg++, offset++) {
		if(shake_read(sh, reg, values + offset) == SHAKE_ERROR)
			return SHAKE_ERROR;
	}

	return SHAKE_SUCCESS;
}

SHAKE_API int sk6_write_cap_thresholds(shake_device* sh, unsigned char* values) {
	if(!sh || !values) return SHAKE_ERROR;

	int reg, offset;
	for(reg = SK6_NV_REG_CS0_INC, offset = 0; reg <= SK6_NV_REG_CS1_DEC_PROFILE; reg++, offset++) {
		if(shake_write(sh, reg, values[offset]) == SHAKE_ERROR)
			return SHAKE_ERROR;
	}

	return SHAKE_SUCCESS;
}

SHAKE_API int sk7_read_cap_thresholds(shake_device* sh, unsigned char* values) {
	if(!sh || !values) return SHAKE_ERROR;

	int reg, offset;
	for(reg = SK7_NV_REG_CAP_INC, offset = 0; reg <= SK7_NV_REG_CAP_DEC_PROFILE; reg++, offset++) {
		if(shake_read(sh, reg, values + offset) == SHAKE_ERROR)
			return SHAKE_ERROR;
	}

	return SHAKE_SUCCESS;
}

SHAKE_API int sk7_write_cap_thresholds(shake_device* sh, unsigned char* values) {
	if(!sh || !values) return SHAKE_ERROR;

	int reg, offset;
	for(reg = SK7_NV_REG_CAP_INC, offset = 0; reg <= SK7_NV_REG_CAP_DEC_PROFILE; reg++, offset++) {
		if(shake_write(sh, reg, values[offset]) == SHAKE_ERROR)
			return SHAKE_ERROR;
	}

	return SHAKE_SUCCESS;
}

SHAKE_API int shake_read_sample_rate(shake_device* sh, int sensor_id, unsigned char* value) {
	if(!sh || !value) return SHAKE_ERROR;

	if(sensor_id < SHAKE_SENSOR_ACC || sensor_id > SHAKE_SENSOR_ANA1)
		return SHAKE_ERROR;

	return shake_read(sh, SHAKE_NV_REG_ACCOUT + sensor_id, value);
}

SHAKE_API int shake_write_sample_rate(shake_device* sh, int sensor_id, unsigned char value) {
	if(!sh) return SHAKE_ERROR;

	if(sensor_id < SHAKE_SENSOR_ACC || sensor_id > SHAKE_SENSOR_ANA1)
		return SHAKE_ERROR;

	return shake_write(sh, SHAKE_NV_REG_ACCOUT + sensor_id, value);
}

SHAKE_API int shake_read_digital_filter(shake_device* sh, int sensor_id, unsigned char* value) {
	if(!sh || !value) return SHAKE_ERROR;

	if(sensor_id < SHAKE_SENSOR_ACC || sensor_id > SHAKE_SENSOR_ANA1)
		return SHAKE_ERROR;

	return shake_read(sh, SHAKE_NV_REG_DIGFIL_ACC + sensor_id, value);
}

SHAKE_API int shake_write_digital_filter(shake_device* sh, int sensor_id, unsigned char value) {
	if(!sh) return SHAKE_ERROR;

	if(sensor_id < SHAKE_SENSOR_ACC || sensor_id > SHAKE_SENSOR_ANA1)
		return SHAKE_ERROR;

	return shake_write(sh, SHAKE_NV_REG_DIGFIL_ACC + sensor_id, value);
}

SHAKE_API int shake_read_shaking_config(shake_device* sh, unsigned char* value) {
	if(!sh) return SHAKE_ERROR;

	return shake_read(sh, SHAKE_NV_REG_SHAKING_CONFIG, value);
}

SHAKE_API int shake_write_shaking_config(shake_device* sh, unsigned char value) {
	if(!sh) return SHAKE_ERROR;

	return shake_write(sh, SHAKE_NV_REG_SHAKING_CONFIG, value);
}

SHAKE_API int shake_read_shaking_accel_threshold(shake_device* sh, unsigned char* value) {
	if(!sh) return SHAKE_ERROR;

	return shake_read(sh, SHAKE_NV_REG_SHAKING_CONFIG, value);
}

SHAKE_API int shake_write_shaking_accel_threshold(shake_device* sh, unsigned char value) {
	if(!sh) return SHAKE_ERROR;

	return shake_write(sh, SHAKE_NV_REG_SHAKING_CONFIG, value);
}

SHAKE_API int shake_read_shaking_holdoff_time(shake_device* sh, unsigned char* value) {
	if(!sh) return SHAKE_ERROR;

	return shake_read(sh, SHAKE_NV_REG_SHAKING_HOLDOFF_TIME, value);
}

SHAKE_API int shake_write_shaking_holdoff_time(shake_device* sh, unsigned char value) {
	if(!sh) return SHAKE_ERROR;

	return shake_write(sh, SHAKE_NV_REG_SHAKING_HOLDOFF_TIME, value);
}

SHAKE_API int shake_read_shaking_vibration_profile(shake_device* sh, unsigned char* value) {
	if(!sh) return SHAKE_ERROR;

	return shake_read(sh, SHAKE_NV_REG_SHAKING_VIBRATION_PROFILE, value);
}

SHAKE_API int shake_write_shaking_vibration_profile(shake_device* sh, unsigned char value) {
	if(!sh) return SHAKE_ERROR;

	return shake_write(sh, SHAKE_NV_REG_SHAKING_VIBRATION_PROFILE, value);
}

SHAKE_API int shake_read_shaking_hpf_constant(shake_device* sh, unsigned char* value) {
	if(!sh) return SHAKE_ERROR;

	return shake_read(sh, SHAKE_NV_REG_SHAKING_HPF_CONSTANT, value);
}

SHAKE_API int shake_write_shaking_hpf_constant(shake_device* sh, unsigned char value) {
	if(!sh) return SHAKE_ERROR;

	return shake_write(sh, SHAKE_NV_REG_SHAKING_HPF_CONSTANT, value);
}

SHAKE_API int shake_read_shaking_lpf_constant(shake_device* sh, unsigned char* value) {
	if(!sh) return SHAKE_ERROR;

	return shake_read(sh, SHAKE_NV_REG_SHAKING_LPF_CONSTANT, value);
}

SHAKE_API int shake_write_shaking_lpf_constant(shake_device* sh, unsigned char value) {
	if(!sh) return SHAKE_ERROR;

	return shake_write(sh, SHAKE_NV_REG_SHAKING_LPF_CONSTANT, value);
}

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
SHAKE_API int shake_reset_shaking_detection(shake_device* sh) {
	if(!sh) return SHAKE_ERROR;

	if(shake_write(sh, SHAKE_NV_REG_SHAKING_CONFIG, 0xFF) == SHAKE_ERROR) return SHAKE_ERROR;
	if(shake_write(sh, SHAKE_NV_REG_SHAKING_ACCEL_THRESHOLD, 0x0A) == SHAKE_ERROR) return SHAKE_ERROR;
	if(shake_write(sh, SHAKE_NV_REG_SHAKING_HOLDOFF_TIME, 0x06) == SHAKE_ERROR) return SHAKE_ERROR;
	if(shake_write(sh, SHAKE_NV_REG_SHAKING_VIBRATION_PROFILE, 0x00) == SHAKE_ERROR) return SHAKE_ERROR;
	if(shake_write(sh, SHAKE_NV_REG_SHAKING_HPF_CONSTANT, 0x20) == SHAKE_ERROR) return SHAKE_ERROR;
	if(shake_write(sh, SHAKE_NV_REG_SHAKING_LPF_CONSTANT, 0x06) == SHAKE_ERROR) return SHAKE_ERROR;

	return SHAKE_SUCCESS;
}

SHAKE_API int shake_read_heart_rate_config(shake_device* sh, unsigned char* value) {
	if(!sh) return SHAKE_ERROR;

	return shake_read(sh, SHAKE_NV_REG_HEART_RATE_CONFIG, value);
}

SHAKE_API int shake_write_heart_rate_config(shake_device* sh, unsigned char value) {
	if(!sh) return SHAKE_ERROR;

	return shake_write(sh, SHAKE_NV_REG_HEART_RATE_CONFIG, value);
}

SHAKE_API int shake_write_packet_request(shake_device* sh, unsigned char value) {
	if(!sh) return SHAKE_ERROR;

	return shake_write(sh, SHAKE_VO_REG_PKTREQ, value);
}

SHAKE_API int shake_write_data_request(shake_device* sh, unsigned char value) {
	if(!sh) return SHAKE_ERROR;

	return shake_write(sh, SHAKE_VO_REG_DATAREQ, value);
}

SHAKE_API int shake_playvib(shake_device* sh, int channel, unsigned char profile) {
	//printf("shake_playvib: channel = %d, profile = %d)\n", channel, profile);
	if(!sh) return SHAKE_ERROR;

	if(profile < 1) 
		return SHAKE_ERROR;

	shake_device_private* dev = (shake_device_private*)sh->priv;

	//printf("shake_playvib: device_type = %d\n", dev->device_type);
	if(dev->device_type == SHAKE_SK6) {
		//printf("shake_playvib: SK6 mode\n");
		int addr = SHAKE_VO_REG_VIB_MAIN + channel;
		return shake_write(sh, addr, profile);
	} else {
		//printf("shake_playvib: SK7 mode\n");
		char buf[5];
		buf[4] = 0;
		switch(channel) {
			case SHAKE_VIB_MAIN: 
				sprintf(buf, "vm%02X", profile);
				break;
			case SHAKE_VIB_LEFT: 
				sprintf(buf, "vl%02X", profile);
				break;
			case SHAKE_VIB_RIGHT:
				sprintf(buf, "vr%02X", profile);
				break;
			case SHAKE_VIB_FORCEREACTOR:
				sprintf(buf, "vf%02X", profile);
				break;
			// TODO check what this actually means
			case SHAKE_VIB_EXT_ACTUATOR:
				sprintf(buf, "vd%02X", profile);
				break;
			default:
				//printf("shake_playvib: unknown channel\n");
				return SHAKE_ERROR;
		}
		//printf("shake_playvib: sending buf = %s\n", buf);
		write_bytes(dev, buf, 4);

	}
	return SHAKE_SUCCESS;
}

SHAKE_API int sk6_playvib_continuous(shake_device* sh, int channel, unsigned char amplitude, unsigned char time) {
	if(!sh) return SHAKE_ERROR;

	if(channel != SHAKE_VIB_LEFT || channel != SHAKE_VIB_RIGHT)
		return SHAKE_ERROR;

	if(amplitude != 0 && amplitude != 33 && amplitude != 66 && amplitude != 100)
		return SHAKE_ERROR;

	if(time < 0 || time > 64)
		return SHAKE_ERROR;

	// construct the byte to send to the vibration register
	unsigned char vibbyte = 0;
	
	switch(amplitude) {
		case 0:
			vibbyte = 0;
			break;
		case 33:
			vibbyte = 0x40;
			break;
		case 66:
			vibbyte = 0x80;
			break;
		case 100:
			vibbyte = 0xc0;
			break;
	}

	vibbyte += time;

	unsigned char vibaddr = SHAKE_VO_REG_VIB_LEFT_CONTINUOUS;
	if(channel == SHAKE_VIB_RIGHT)
		vibaddr = SHAKE_VO_REG_VIB_RIGHT_CONTINUOUS;

	return shake_write(sh, vibaddr, vibbyte);
}

SHAKE_API int sk6_upload_vib_sample(shake_device* sh, unsigned char profile, int* sample, int sample_length) {
	return shake_upload_vib_sample_extended(sh, profile, sample, sample_length, 0, 0, 0);
}

SHAKE_API int shake_upload_vib_sample_extended(shake_device* sh, unsigned char profile, int* sample, int sample_length, unsigned char mode, unsigned char freq, unsigned char duty) {
	char packetbuf[256];
	shake_device_private* dev;
	int bufpos = 0, i, cursample = 0, timeout = 250;

	if(!sh || !sample) return SHAKE_ERROR;

	if(profile < 1) return SHAKE_ERROR;

	if(sample_length <= 0 || sample_length > SHAKE_VIB_SAMPLE_MAX_LENGTH) return SHAKE_ERROR;

	bufpos += sprintf(packetbuf, "$VIB,%02X,%02X,%02X,%02X,", profile, mode, freq, duty);

	for(i=0;i<sample_length-1;i++) {
		bufpos += sprintf(packetbuf + bufpos, "%02X,%02X,", sample[cursample], sample[cursample+1]);
		cursample += 2;
	}

	bufpos += sprintf(packetbuf + bufpos, "%02X,%02X~", sample[cursample], sample[cursample+1]);

	dev = (shake_device_private*)sh->priv;

	if(write_bytes(dev, packetbuf, bufpos) != bufpos)
		return SHAKE_ERROR;

	if(dev->wait_for_acks == 0)
		return SHAKE_SUCCESS;

	dev->waiting_for_ack_signal = TRUE;	
	dev->waiting_for_ack = TRUE;

	while(dev->waiting_for_ack_signal == TRUE) {
		shake_sleep(1);
		--timeout;
		if(timeout == 0)
			break;
	}

	dev->waiting_for_ack = FALSE;
	
	if(!dev->lastack)
		return SHAKE_ERROR;

	return SHAKE_SUCCESS;
}

SHAKE_API int shake_read_battery_level(shake_device* sh, unsigned char* value) {
	if(!sh || !value) return SHAKE_ERROR;

	return shake_read(sh, SHAKE_VO_REG_BATTERY, value);
}

SHAKE_API int shake_read_power_status(shake_device* sh, unsigned char* value) {
	if(!sh || !value) return SHAKE_ERROR;

	return shake_read(sh, SHAKE_VO_REG_PWRSTA, value);
}

SHAKE_API float shake_read_temperature(shake_device* sh) {
	float ret;
	unsigned char value = 0;

	if(!sh) return -1.0f;

	ret = shake_read(sh, SHAKE_VO_REG_TEMPERATURE, &value);
	if(ret == SHAKE_ERROR)
		return -1.0f;

	ret = value / 4.0;

	return ret;
}

int shake_accx(shake_device* sh) {
	if(!sh) return SHAKE_ERROR;

	shake_device_private* devpriv = (shake_device_private*)sh->priv;

	int x = devpriv->shake->data.accx;
	devpriv->shake->data.timestamps[SHAKE_SENSOR_ACC] = devpriv->shake->data.internal_timestamps[SHAKE_SENSOR_ACC];
	return x;
}

int shake_accy(shake_device* sh){
	if(!sh) return SHAKE_ERROR;

	shake_device_private* devpriv = (shake_device_private*)sh->priv;

	int y = devpriv->shake->data.accy;
	devpriv->shake->data.timestamps[SHAKE_SENSOR_ACC] = devpriv->shake->data.internal_timestamps[SHAKE_SENSOR_ACC];
	return y;
}

int shake_accz(shake_device* sh){
	if(!sh) return SHAKE_ERROR;

	shake_device_private* devpriv = (shake_device_private*)sh->priv;

	int z = devpriv->shake->data.accz;
	devpriv->shake->data.timestamps[SHAKE_SENSOR_ACC] = devpriv->shake->data.internal_timestamps[SHAKE_SENSOR_ACC];
	return z;
}

int shake_acc(shake_device* sh, int* xyz) {
	shake_device_private* dev;

	if(!sh || !xyz) return SHAKE_ERROR;

	dev = (shake_device_private*)sh->priv;

	xyz[0] = dev->shake->data.accx;
	xyz[1] = dev->shake->data.accy;
	xyz[2] = dev->shake->data.accz;
	dev->shake->data.timestamps[SHAKE_SENSOR_ACC] = dev->shake->data.internal_timestamps[SHAKE_SENSOR_ACC];
	return SHAKE_SUCCESS;
}

int shake_gyrx(shake_device* sh) {
	if(!sh) return SHAKE_ERROR;

	shake_device_private* devpriv = (shake_device_private*)sh->priv;

	int x = devpriv->shake->data.gyrx;
	devpriv->shake->data.timestamps[SHAKE_SENSOR_GYRO] = devpriv->shake->data.internal_timestamps[SHAKE_SENSOR_GYRO];
	return x;
}

int shake_gyry(shake_device* sh) {
	if(!sh) return SHAKE_ERROR;

	shake_device_private* devpriv = (shake_device_private*)sh->priv;

	int y = devpriv->shake->data.gyry;
	devpriv->shake->data.timestamps[SHAKE_SENSOR_GYRO] = devpriv->shake->data.internal_timestamps[SHAKE_SENSOR_GYRO];
	return y;
}

int shake_gyrz(shake_device* sh) {
	if(!sh) return SHAKE_ERROR;

	shake_device_private* devpriv = (shake_device_private*)sh->priv;

	int z = devpriv->shake->data.gyrz;
	devpriv->shake->data.timestamps[SHAKE_SENSOR_GYRO] = devpriv->shake->data.internal_timestamps[SHAKE_SENSOR_GYRO];
	return z;
}

int shake_gyr(shake_device* sh, int* xyz) {
	shake_device_private* dev;

	if(!sh || !xyz) return SHAKE_ERROR;

	dev = (shake_device_private*)sh->priv;

	xyz[0] = dev->shake->data.gyrx;
	xyz[1] = dev->shake->data.gyry;
	xyz[2] = dev->shake->data.gyrz;
	dev->shake->data.timestamps[SHAKE_SENSOR_GYRO] = dev->shake->data.internal_timestamps[SHAKE_SENSOR_GYRO];
	return SHAKE_SUCCESS;
}

int shake_magx(shake_device* sh) {
	if(!sh) return SHAKE_ERROR;

	shake_device_private* devpriv = (shake_device_private*)sh->priv;

	int x = devpriv->shake->data.magx;
	devpriv->shake->data.timestamps[SHAKE_SENSOR_MAG] = devpriv->shake->data.internal_timestamps[SHAKE_SENSOR_MAG];
	return x;
}

int shake_magy(shake_device* sh) {
	if(!sh) return SHAKE_ERROR;

	shake_device_private* devpriv = (shake_device_private*)sh->priv;

	int y = devpriv->shake->data.magy;
	devpriv->shake->data.timestamps[SHAKE_SENSOR_MAG] = devpriv->shake->data.internal_timestamps[SHAKE_SENSOR_MAG];
	return y;
}

int shake_magz(shake_device* sh) {
	if(!sh) return SHAKE_ERROR;

	shake_device_private* devpriv = (shake_device_private*)sh->priv;

	int z = devpriv->shake->data.magz;
	devpriv->shake->data.timestamps[SHAKE_SENSOR_MAG] = devpriv->shake->data.internal_timestamps[SHAKE_SENSOR_MAG];
	return z;
}

int shake_mag(shake_device* sh, int* xyz) {
	shake_device_private* dev;

	if(!sh || !xyz) return SHAKE_ERROR;

	dev = (shake_device_private*)sh->priv;

	xyz[0] = dev->shake->data.magx;
	xyz[1] = dev->shake->data.magy;
	xyz[2] = dev->shake->data.magz;
	dev->shake->data.timestamps[SHAKE_SENSOR_MAG] = dev->shake->data.internal_timestamps[SHAKE_SENSOR_MAG];
	return SHAKE_SUCCESS;
}

int shake_heading(shake_device* sh) {
	if(!sh) return SHAKE_ERROR;
	
	shake_device_private* devpriv = (shake_device_private*)sh->priv;
	int hdg = devpriv->shake->data.heading;
	devpriv->shake->data.timestamps[SHAKE_SENSOR_HEADING] = devpriv->shake->data.internal_timestamps[SHAKE_SENSOR_HEADING];
	return hdg;
}

SHAKE_API int sk7_roll_pitch_heading(shake_device* sh, int* rph) {
	if(!sh || !rph) return SHAKE_ERROR;

	shake_device_private* devpriv = (shake_device_private*)sh->priv;
	for(int i=0;i<3;i++)
		rph[i] = devpriv->shake->data.rph[i];

	return SHAKE_SUCCESS;
}

SHAKE_API int sk7_roll_pitch_heading_quaternions(shake_device* sh, int* rphq) {
	if(!sh || !rphq) return SHAKE_ERROR;

	shake_device_private* devpriv = (shake_device_private*)sh->priv;
	for(int i=0;i<4;i++)
		rphq[i] = devpriv->shake->data.rphq[i];

	return SHAKE_SUCCESS;
}

int sk6_cap(shake_device* sh, int* proxboth) {
	shake_device_private* dev;

	if(!sh || !proxboth) return SHAKE_ERROR;

	dev = (shake_device_private*)sh->priv;

	proxboth[0] = dev->shake->data.cap_sk6[0];
	proxboth[1] = dev->shake->data.cap_sk6[1];
	dev->shake->data.timestamps[SHAKE_SENSOR_SK6_CAP0] = dev->shake->data.internal_timestamps[SHAKE_SENSOR_SK6_CAP0];
	dev->shake->data.timestamps[SHAKE_SENSOR_SK6_CAP1] = dev->shake->data.internal_timestamps[SHAKE_SENSOR_SK6_CAP1];
	return SHAKE_SUCCESS;
}

int sk6_cap0(shake_device* sh) {
	shake_device_private* dev;

	if(!sh) return SHAKE_ERROR;

	dev = (shake_device_private*)sh->priv;
	
	int c0 = dev->shake->data.cap_sk6[0];
	dev->shake->data.timestamps[SHAKE_SENSOR_SK6_CAP0] = dev->shake->data.internal_timestamps[SHAKE_SENSOR_SK6_CAP0];
	return c0;
}

int sk6_cap1(shake_device* sh) {
	shake_device_private* dev;

	if(!sh) return SHAKE_ERROR;

	dev = (shake_device_private*)sh->priv;
	
	int c1 = dev->shake->data.cap_sk6[1];
	dev->shake->data.timestamps[SHAKE_SENSOR_SK6_CAP1] = dev->shake->data.internal_timestamps[SHAKE_SENSOR_SK6_CAP1];
	return c1;
}

int sk7_cap(shake_device* sh, int* prox) {
	shake_device_private* dev;

	if(!sh || !prox) return SHAKE_ERROR;

	dev = (shake_device_private*)sh->priv;

	memcpy(prox, &(dev->shake->data.cap_sk7[0]), sizeof(int) * 12);
	dev->shake->data.timestamps[SHAKE_SENSOR_CAP] = dev->shake->data.internal_timestamps[SHAKE_SENSOR_CAP];
	
	return SHAKE_SUCCESS;
}

int sk7_cap_ext(shake_device* sh, int blocks, int* prox) {
	shake_device_private* dev;

	if(!sh || !prox) return SHAKE_ERROR;

	dev = (shake_device_private*)sh->priv;

	switch(blocks) {
		case 0:
			memcpy(prox, &(dev->shake->data.cap_sk7[1]), sizeof(int) * 12);
			break;
		case 1:
			memcpy(prox, &(dev->shake->data.cap_sk7[2]), sizeof(int) * 12);
			break;
		case 2:
			memcpy(prox, &(dev->shake->data.cap_sk7[1]), sizeof(int) * 12);
			memcpy(prox+12, &(dev->shake->data.cap_sk7[2]), sizeof(int) * 12);
			break;
		default:
			return SHAKE_ERROR;
	}
	//dev->shake->data.timestamps[SHAKE_SENSOR_CAP] = dev->shake->data.internal_timestamps[SHAKE_SENSOR_CAP];
	return SHAKE_SUCCESS;
}

int shake_analog0(shake_device* sh) {
	if(!sh) return SHAKE_ERROR;

	shake_device_private* dev = (shake_device_private*)sh->priv;
	
	int a0 = dev->shake->data.ana0;
	dev->shake->data.timestamps[SHAKE_SENSOR_ANA0] = dev->shake->data.internal_timestamps[SHAKE_SENSOR_ANA0];
	return a0;
}

int shake_analog1(shake_device* sh) {
	if(!sh) return SHAKE_ERROR;

	shake_device_private* dev = (shake_device_private*)sh->priv;
	
	int a1 = dev->shake->data.ana1;
	dev->shake->data.timestamps[SHAKE_SENSOR_ANA1] = dev->shake->data.internal_timestamps[SHAKE_SENSOR_ANA1];
	return a1;
}

int shake_analog(shake_device* sh, int* a0a1) {
	shake_device_private* dev;

	if(!sh) return SHAKE_ERROR;

	dev = (shake_device_private*)sh->priv;

	a0a1[0] = dev->shake->data.ana0;
	a0a1[1] = dev->shake->data.ana1;
	dev->shake->data.timestamps[SHAKE_SENSOR_ANA0] = dev->shake->data.internal_timestamps[SHAKE_SENSOR_ANA0];
	dev->shake->data.timestamps[SHAKE_SENSOR_ANA1] = dev->shake->data.internal_timestamps[SHAKE_SENSOR_ANA1];
	return SHAKE_SUCCESS;
}

SHAKE_API int shake_shaking_peakaccel(shake_device* sh) {
	shake_device_private* dev;

	if(!sh) return SHAKE_ERROR;

	dev = (shake_device_private*)sh->priv;
	return dev->shake->data.shaking_peak_accel;
}

SHAKE_API int shake_shaking_direction(shake_device* sh) {
	shake_device_private* dev;

	if(!sh) return SHAKE_ERROR;

	dev = (shake_device_private*)sh->priv;
	return dev->shake->data.shaking_direction;
}

SHAKE_API int shake_shaking_timestamp(shake_device* sh) {
	shake_device_private* dev;

	if(!sh) return SHAKE_ERROR;

	dev = (shake_device_private*)sh->priv;
	return dev->shake->data.shaking_timestamp;
}

SHAKE_API int shake_heart_rate(shake_device* sh) {
	shake_device_private* dev;

	if(!sh) return SHAKE_ERROR;

	dev = (shake_device_private*)sh->priv;

	return dev->shake->data.hr_bpm;
}

SHAKE_API int shake_data_timestamp(shake_device* sh, int sensor) {
	shake_device_private* dev;

	if(!sh) return SHAKE_ERROR;

	dev = (shake_device_private*)sh->priv;

	return dev->shake->data.timestamps[sensor];
}

SHAKE_API void shake_wait_for_acks(shake_device* sh, int wait_for_ack) {
	shake_device_private* dev;

	if(!sh) return;

	dev = (shake_device_private*)sh->priv;

	dev->wait_for_acks = wait_for_ack;
}

SHAKE_API int shake_register_event_callback(shake_device* sh, void (SHAKE_CALLBACK *callback)(shake_device*, int)) {
	shake_device_private* dev;

	if(!sh) return SHAKE_ERROR;

	dev = (shake_device_private*)sh->priv;

	dev->navcb = (void(SHAKE_CALLBACK *)(void*, int))callback;
	return SHAKE_SUCCESS;
}

#ifdef _WIN32
SHAKE_API int shake_register_event_callback_STDCALL(shake_device* sh, void (SHAKE_STDCALL_CALLBACK *callback)(shake_device*, int)) {
	shake_device_private* dev;

	if(!sh) return SHAKE_ERROR;

	dev = (shake_device_private*)sh->priv;

	dev->navcb_STDCALL = (void(SHAKE_STDCALL_CALLBACK *)(void*, int))callback;
	return SHAKE_SUCCESS;
}
#endif

SHAKE_API int shake_read_audio_config(shake_device* sh, unsigned char* value) {
	if(!sh) return SHAKE_ERROR;

	return shake_read(sh, SHAKE_NV_REG_AUDIO_CONFIG, value);
}

SHAKE_API int shake_write_audio_config(shake_device* sh, unsigned char value) {
	if(!sh) return SHAKE_ERROR;

	return shake_write(sh, SHAKE_NV_REG_AUDIO_CONFIG, value);
}

SHAKE_API int shake_write_midi_amplitude(shake_device* sh, unsigned char value) {
	if(!sh) return SHAKE_ERROR;

	return shake_write(sh, SHAKE_VO_REG_MIDI_AMPLITUDE, value);
}

SHAKE_API int shake_write_midi_note(shake_device* sh, unsigned char value) {
	if(!sh) return SHAKE_ERROR;

	return shake_write(sh, SHAKE_VO_REG_MIDI_NOTE, value);
}

SHAKE_API int shake_write_midi_waveform(shake_device* sh, unsigned char value) {
	if(!sh) return SHAKE_ERROR;

	return shake_write(sh, SHAKE_VO_REG_MIDI_WAVEFORM, value);
}

SHAKE_API int shake_register_audio_callback(shake_device* sh, void (SHAKE_CALLBACK *callback)(shake_device*, short*, int, short*, int)) {
	shake_device_private* dev;

	if(!sh) return SHAKE_ERROR;

	dev = (shake_device_private*)sh->priv;

	dev->audio_cb = (void(SHAKE_CALLBACK *)(void*, short*, int, short*, int))callback;
	return SHAKE_SUCCESS;
}

#ifdef _WIN32
SHAKE_API int shake_register_audio_callback_STDCALL(shake_device* sh, void (SHAKE_STDCALL_CALLBACK *callback)(shake_device*, short*, int, short*, int)) {
	shake_device_private* dev;

	if(!sh) return SHAKE_ERROR;

	dev = (shake_device_private*)sh->priv;

	dev->audio_cb_STDCALL = (void(SHAKE_STDCALL_CALLBACK *)(void*, short*, int, short*, int))callback; 
	return SHAKE_SUCCESS;
}
#endif

SHAKE_API int shake_upload_audio_sample(shake_device* sh, unsigned short address, short* sample_data, unsigned short sample_len) {
	shake_device_private* dev;
	int timeout = 1000;

	if(!sh) return SHAKE_ERROR;

	dev = (shake_device_private*)sh->priv;

	if(!sh || !sample_data || dev->waiting_for_ack || address < SHAKE_UPLOAD_MIN_PAGE) return SHAKE_ERROR;

	/* construct the packet header*/
	char packetbuf[1063];
	memset(packetbuf, 0, 1063);
	sprintf(packetbuf, "$STRU");

	// sample data can be longer than the page size on the SHAKE, so need to split and upload separately
	int complete_pages = sample_len / SHAKE_UPLOAD_PAGE_SIZE;
	int last_page_size = sample_len % SHAKE_UPLOAD_PAGE_SIZE;
	int starting_address, current_address;

	starting_address = current_address = address;
	int end_address = starting_address + complete_pages;
	if(last_page_size != 0)
		end_address++;

	if(end_address > SHAKE_UPLOAD_MAX_PAGE) 
		return SHAKE_ERROR;

	bool reset_acks = false;
	if(dev->wait_for_acks == 0) {
		dev->wait_for_acks = 1;
		reset_acks = true;
	}

	// upload complete pages first
	int page;
	for(page = 0; page < complete_pages; current_address++, page++) {
		// set the address of the current page, lsb first
		packetbuf[5] = current_address & 0x00FF;
		packetbuf[6] = (current_address & 0xFF00) >> 8;

		// compress the original data to 8-bit format
		shake_mulaw_compress(sample_data + (page * SHAKE_UPLOAD_PAGE_SIZE), SHAKE_UPLOAD_PAGE_SIZE, (unsigned char*)(packetbuf+7));

		// send packet
		write_bytes_delayed(dev, packetbuf, 1063, SHAKE_UPLOAD_CHUNK_SIZE, SHAKE_UPLOAD_DELAY);

		// wait for reply
		dev->waiting_for_ack_signal = TRUE;	
		dev->waiting_for_ack = TRUE;

		while(dev->waiting_for_ack_signal == TRUE) {
			shake_sleep(1);
			--timeout;
			if(timeout == 0)
				break;
		}

		dev->waiting_for_ack = FALSE;

		/* read the dev->lastack, dev->lastaddr and dev->lastval entries to get the response from the ack packet */
		if(!dev->lastack) {
			if(reset_acks) dev->wait_for_acks = 0;
			return SHAKE_ERROR;
		}

		dev->lastack = FALSE;
	}

	// upload final page if required
	memset(packetbuf+7, 0, SHAKE_UPLOAD_PAGE_SIZE);
	if(last_page_size > 0) {
		// set the address of the current page, lsb first
		packetbuf[5] = current_address & 0x00FF;
		packetbuf[6] = (current_address & 0xFF00) >> 8;

		// compress the original data to 8-bit format
		shake_mulaw_compress(sample_data + (complete_pages * SHAKE_UPLOAD_PAGE_SIZE), last_page_size, (unsigned char*)(packetbuf+7));

		// send packet
		write_bytes_delayed(dev, packetbuf, 1063, SHAKE_UPLOAD_CHUNK_SIZE, SHAKE_UPLOAD_DELAY);

		// wait for reply
		dev->waiting_for_ack_signal = TRUE;	
		dev->waiting_for_ack = TRUE;

		while(dev->waiting_for_ack_signal == TRUE) {
			shake_sleep(1);
			--timeout;
			if(timeout == 0)
				break;
		}

		dev->waiting_for_ack = FALSE;

		/* read the dev->lastack, dev->lastaddr and dev->lastval entries to get the response from the ack packet */
		if(!dev->lastack) {
			if(reset_acks) dev->wait_for_acks = 0;
			return SHAKE_ERROR;
		}

		dev->lastack = FALSE;
	}

	if(reset_acks) dev->wait_for_acks = 0;
	return SHAKE_SUCCESS;
}

SHAKE_API int shake_play_audio_sample(shake_device* sh, unsigned short start_address, unsigned short end_address, unsigned short amplitude) {
	shake_device_private* dev;
	int timeout = 250;

	if(!sh) return SHAKE_ERROR;

	dev = (shake_device_private*)sh->priv;

	if(!sh || dev->waiting_for_ack) return SHAKE_ERROR;

	/* construct the packet */
	char packetbuf[20];
	memset(packetbuf, 0, 20);
	sprintf(packetbuf, "$STRP"); 
	// set start addr, end addr and amplitude, lsb first in each case
	packetbuf[5] = start_address & 0x00FF;
	packetbuf[6] = (start_address & 0xFF00) >> 8;
	packetbuf[7] = end_address & 0x00FF;
	packetbuf[8] = (end_address & 0xFF00) >> 8;
	packetbuf[9] = amplitude & 0x00FF;
	packetbuf[10] = (amplitude & 0xFF00) >> 8;

	/* here we send a command packet containing the new value for the register, then
	*	wait for an ack packet to come back with a success/failure code */
	write_bytes(dev, packetbuf, 15);

	if(dev->wait_for_acks == 0)
		return SHAKE_SUCCESS;

	dev->waiting_for_ack_signal = TRUE;
	dev->waiting_for_ack = TRUE;

	while(dev->waiting_for_ack_signal == TRUE) {
		shake_sleep(1);
		--timeout;
		if(timeout == 0)
			break;
	}

	dev->waiting_for_ack = FALSE;

	/* read the dev->lastack, dev->lastaddr and dev->lastval entries to get the response from the ack packet */
	if(!dev->lastack)
		return SHAKE_ERROR;

	dev->lastack = FALSE;
	return SHAKE_SUCCESS;
}

SHAKE_API int shake_exp_play_vib_sample(shake_device* sh, unsigned short start_address, unsigned short end_address, unsigned short amplitude) {
	shake_device_private* dev;
	int timeout = 250;

	if(!sh) return SHAKE_ERROR;

	dev = (shake_device_private*)sh->priv;

	if(!sh || dev->waiting_for_ack) return SHAKE_ERROR;

	/* construct the packet */
	char packetbuf[20];
	memset(packetbuf, 0, 20);
	sprintf(packetbuf, "$STRV"); 
	// set start addr, end addr and amplitude, lsb first in each case
	packetbuf[5] = start_address & 0x00FF;
	packetbuf[6] = (start_address & 0xFF00) >> 8;
	packetbuf[7] = end_address & 0x00FF;
	packetbuf[8] = (end_address & 0xFF00) >> 8;
	packetbuf[9] = amplitude & 0x00FF;
	packetbuf[10] = (amplitude & 0xFF00) >> 8;

	/* here we send a command packet containing the new value for the register, then
	*	wait for an ack packet to come back with a success/failure code */
	write_bytes(dev, packetbuf, 15);

	if(dev->wait_for_acks == 0)
		return SHAKE_SUCCESS;

	dev->waiting_for_ack_signal = TRUE;
	dev->waiting_for_ack = TRUE;

	//shake_thread_wait(&(dev->thread), 2, 500, CMD_THREAD);
	while(dev->waiting_for_ack_signal == TRUE) {
		shake_sleep(1);
		--timeout;
		if(timeout == 0)
			break;
	}

	dev->waiting_for_ack = FALSE;

	/* read the dev->lastack, dev->lastaddr and dev->lastval entries to get the response from the ack packet */
	if(!dev->lastack)
		return SHAKE_ERROR;

	dev->lastack = FALSE;
	return SHAKE_SUCCESS;
}

SHAKE_API int shake_exp_write_gp_register(shake_device* sh, int reg_number, unsigned value) {
	if(!sh || (reg_number < 0 || reg_number > 5)) return SHAKE_ERROR;

	return shake_write(sh, SHAKE_VO_REG_EXP_PWM1+reg_number, value);
}

SHAKE_API int shake_read_expansion_config(shake_device* sh, unsigned char* value) {
	if(!sh) return SHAKE_ERROR;

	return shake_read(sh, SHAKE_NV_REG_EXPANSION_CONFIG, value);
}

SHAKE_API int shake_write_expansion_config(shake_device* sh, unsigned char value) {
	if(!sh) return SHAKE_ERROR;

	return shake_write(sh, SHAKE_NV_REG_EXPANSION_CONFIG, value);
}

SHAKE_API int shake_exp_upload_vib_sample(shake_device* sh, unsigned short address, char* sample_data, unsigned short sample_len) {
	shake_device_private* dev;
	int timeout = 500;

	if(!sh) return SHAKE_ERROR;

	dev = (shake_device_private*)sh->priv;

	if(!sh || !sample_data || dev->waiting_for_ack) return SHAKE_ERROR;

	/* construct the packet */
	char packetbuf[1063];
	memset(packetbuf, 0, 1063);
	sprintf(packetbuf, "$STRU");

	// sample data can be longer than the page size on the SHAKE, so need to split and upload separately
	int complete_pages = sample_len / SHAKE_UPLOAD_PAGE_SIZE;
	int last_page_size = sample_len % SHAKE_UPLOAD_PAGE_SIZE;
	int starting_address, current_address;

	starting_address = current_address = address;
	int end_address = starting_address + complete_pages;
	if(last_page_size != 0)
		end_address++;

	if(end_address > SHAKE_UPLOAD_MAX_PAGE) 
		return SHAKE_ERROR;

	bool reset_acks = false;
	if(dev->wait_for_acks == 0) {
		dev->wait_for_acks = 1;
		reset_acks = true;
	}

	// upload complete pages first
	int page;
	for(page = 0; page < complete_pages; current_address++, page++) {
		// set the address of the current page, lsb first
		packetbuf[5] = current_address & 0x00FF;
		packetbuf[6] = (current_address & 0xFF00) >> 8;

		// compress the original data to 8-bit format
		//shake_mulaw_compress(sample_data + (page * SHAKE_UPLOAD_PAGE_SIZE), SHAKE_UPLOAD_PAGE_SIZE, (unsigned char*)(packetbuf+7));
		memcpy(packetbuf+7, sample_data + (page * SHAKE_UPLOAD_PAGE_SIZE), sample_len);

		// send packet
		//write_bytes(dev, packetbuf, 1063);
		write_bytes_delayed(dev, packetbuf, 1063, SHAKE_UPLOAD_CHUNK_SIZE, SHAKE_UPLOAD_DELAY);

		// wait for reply
		dev->waiting_for_ack_signal = TRUE;	
		dev->waiting_for_ack = TRUE;

		while(dev->waiting_for_ack_signal == TRUE) {
			shake_sleep(1);
			--timeout;
			if(timeout == 0)
				break;
		}

		dev->waiting_for_ack = FALSE;

		/* read the dev->lastack, dev->lastaddr and dev->lastval entries to get the response from the ack packet */
		if(!dev->lastack) {
			if(reset_acks) dev->wait_for_acks = 0;
			return SHAKE_ERROR;
		}

		dev->lastack = FALSE;
	}

	// upload final page if required
	memset(packetbuf+7, 0, SHAKE_UPLOAD_PAGE_SIZE);
	if(last_page_size > 0) {
		// set the address of the current page
		packetbuf[5] = current_address & 0x00FF;
		packetbuf[6] = (current_address & 0xFF00) >> 8;

		// compress the original data to 8-bit format
		//shake_mulaw_compress(sample_data + (complete_pages * SHAKE_UPLOAD_PAGE_SIZE), last_page_size, (unsigned char*)(packetbuf+7));
		memcpy(packetbuf+7, sample_data + (complete_pages * SHAKE_UPLOAD_PAGE_SIZE), last_page_size);

		// send packet
		write_bytes_delayed(dev, packetbuf, 1063, SHAKE_UPLOAD_CHUNK_SIZE, SHAKE_UPLOAD_DELAY);

		// wait for reply
		dev->waiting_for_ack_signal = TRUE;	
		dev->waiting_for_ack = TRUE;

		while(dev->waiting_for_ack_signal == TRUE) {
			shake_sleep(1);
			--timeout;
			if(timeout == 0)
				break;
		}

		dev->waiting_for_ack = FALSE;

		/* read the dev->lastack, dev->lastaddr and dev->lastval entries to get the response from the ack packet */
		if(!dev->lastack) {
			if(reset_acks) dev->wait_for_acks = 0;
			return SHAKE_ERROR;
		}

		dev->lastack = FALSE;
	}
	if(reset_acks) dev->wait_for_acks = 0;
	return SHAKE_SUCCESS;
}

SHAKE_API char* shake_rfid_tid(shake_device* sh) {
	if(!sh) return NULL;
		
	shake_device_private* dev;

	dev = (shake_device_private*)sh->priv;

	return dev->lastrfid;
}

SHAKE_API int shake_rfid_scan(shake_device* sh) {
	if(!sh) return SHAKE_ERROR;

	return shake_write(sh, SHAKE_VO_REG_RFID_SCAN, 1);
}

SHAKE_API int shake_read_rfid_config(shake_device* sh, unsigned char* value) {
	if(!sh) return SHAKE_ERROR;

	return shake_read(sh, SHAKE_NV_REG_RFID_CONFIG, value);
}

SHAKE_API int shake_write_rfid_config(shake_device* sh, unsigned char value) {
	if(!sh) return SHAKE_ERROR;

	return shake_write(sh, SHAKE_NV_REG_RFID_CONFIG, value);
}

SHAKE_API int shake_read_rfid_scan_freq(shake_device* sh, unsigned char* value) {
	if(!sh) return SHAKE_ERROR;

	return shake_read(sh, SHAKE_NV_REG_RFID_FREQUENCY, value);
}

SHAKE_API int shake_write_rfid_scan_freq(shake_device* sh, unsigned char value) {
	if(!sh) return SHAKE_ERROR;

	return shake_write(sh, SHAKE_NV_REG_RFID_FREQUENCY, value);
}


SHAKE_API int sk7_override_led(shake_device* sh, unsigned char red, unsigned char green, unsigned char blue) {
	if(!sh) return SHAKE_ERROR;

	int ret1 = shake_write(sh, SHAKE_VO_REG_LED_RED, red);
	int ret2 = shake_write(sh, SHAKE_VO_REG_LED_GREEN, green);
	int ret3 = shake_write(sh, SHAKE_VO_REG_LED_BLUE, blue);

	if(ret1 == SHAKE_ERROR || ret2 == SHAKE_ERROR || ret3 == SHAKE_ERROR)
		return SHAKE_ERROR;

	return SHAKE_SUCCESS;
}

SHAKE_API int sk7_configure_heading_feedback(shake_device* sh, int lower_threshold, int upper_threshold, int hysteresis, int vib_profile) {
	if(!sh) return SHAKE_ERROR;

	// lower threshold; lsb
	if(shake_write(sh, SK7_NV_REG_HEADING_LOWER_LSB, lower_threshold & 0x00FF) != SHAKE_SUCCESS)
		return SHAKE_ERROR;

	// lower threshold; msb
	if(shake_write(sh, SK7_NV_REG_HEADING_LOWER_MSB, (lower_threshold & 0xFF00) >> 8) != SHAKE_SUCCESS)
		return SHAKE_ERROR;
	
	// upper threshold; lsb
	if(shake_write(sh, SK7_NV_REG_HEADING_UPPER_LSB, upper_threshold & 0x00FF) != SHAKE_SUCCESS)
		return SHAKE_ERROR;

	// upper threshold; msb
	if(shake_write(sh, SK7_NV_REG_HEADING_UPPER_MSB, (upper_threshold & 0xFF00) >> 8) != SHAKE_SUCCESS)
		return SHAKE_ERROR;

	// hysteresis
	if(hysteresis > 255)
		hysteresis = 255;
	if(shake_write(sh, SK7_NV_REG_HEADING_HYSTERESIS, hysteresis) != SHAKE_SUCCESS)
		return SHAKE_ERROR;

	// vibration profile
	if(shake_write(sh, SK7_NV_REG_HEADING_VIB_PROFILE, vib_profile) != SHAKE_SUCCESS)
		return SHAKE_ERROR;

	return SHAKE_SUCCESS;
}

SHAKE_API int sk7_control_heading_feedback(shake_device* sh, int enabled, int vib_looping, int led_feedback) {
	if(!sh) return SHAKE_ERROR;

	int val = 0;
	if(enabled)
		val |= 0x01;

	if(vib_looping)
		val |= 0x02;

	if(led_feedback)
		val |= 0x04;

	return shake_write(sh, SK7_NV_REG_HEADING_FEEDBACK, val);
}

SHAKE_API int sk7_configure_roll_pitch_heading(shake_device* sh, int value) {
	if(!sh) return SHAKE_ERROR;

	return shake_write(sh, SK7_NV_REG_RPH_CONFIG, value);
}

/* generic function to read a register on the SHAKE */
SHAKE_API int shake_read(shake_device* sh, int addr, unsigned char* value) {
	shake_device_private* dev;
	char scpbuf[20];
	int timeout = 250;

	if(!sh) return SHAKE_ERROR;

	dev = (shake_device_private*)sh->priv;
	
	/* construct the packet */
	int cmdlen = sprintf(scpbuf, "$REA,%04X,00", addr);

	/*  send a command packet requesting the contents of the appropriate
	*	register, then wait for an ack to appear with the value */
	write_bytes(dev, scpbuf, cmdlen);

	dev->waiting_for_ack_signal = TRUE;	
	dev->waiting_for_ack = TRUE;

	while(dev->waiting_for_ack_signal == TRUE) {
		shake_sleep(1);
		--timeout;
		if(timeout == 0)
			break;
	}

	dev->waiting_for_ack = FALSE;
	
	/* read the dev->lastack, dev->lastaddr and dev->lastval entries to get the response from the ack packet */
	if(!dev->lastack) {
		SHAKE_DBG("FAILED TO READ %04X\n", addr);
		return SHAKE_ERROR;
	}

	*value = dev->lastval;

	return SHAKE_SUCCESS;
}

/* generic function to write a register on the SHAKE */
SHAKE_API int shake_write(shake_device* sh, int addr, unsigned char value) {
	shake_device_private* dev;
	char scpbuf[20];
	int timeout = 250;

	dev = (shake_device_private*)sh->priv;

	if(!sh || dev->waiting_for_ack) return SHAKE_ERROR;

	/* construct the packet */
	int cmdlen = sprintf(scpbuf, "$WRI,%04X,%02X", addr, value);

	/* send a command packet containing the new value for the register, then
	*	wait for an ack packet to come back with a success/failure code */
	write_bytes(dev, scpbuf, cmdlen);

	if(dev->wait_for_acks == 0)
		return SHAKE_SUCCESS;

	dev->waiting_for_ack_signal = TRUE;	
	dev->waiting_for_ack = TRUE;

	while(dev->waiting_for_ack_signal == TRUE) {
		shake_sleep(1);
		--timeout;
		if(timeout == 0)
			break;
	}

	dev->waiting_for_ack = FALSE;
	/* read the dev->lastack, dev->lastaddr and dev->lastval entries to get the response from the ack packet */
	if(!dev->lastack)
		return SHAKE_ERROR;

	dev->lastack = FALSE;
                            
	return SHAKE_SUCCESS;
}
