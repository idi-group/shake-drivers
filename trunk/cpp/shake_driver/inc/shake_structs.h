#ifndef _SHAKE_STRUCTS_H_
#define _SHAKE_STRUCTS_H_

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


#include "shake_platform.h"
#include "shake_thread.h"
#include "shake_registers.h"

/* holds pointer to and data about BT serial port being used */
typedef struct {
#ifdef _WIN32
	HANDLE port;
	DCB dcb;
	COMMTIMEOUTS cto;
#endif
} shake_serial_port;

#include "shake_btdefs.h"

#ifdef SHAKE_RFCOMM_SUPPORTED
	#ifdef _WIN32
		typedef struct {
			SOCKET sock;
			SOCKADDR_BTH addr;
		} shake_rfcomm_socket;
	#else
	typedef struct {
		int sock;
	} shake_rfcomm_socket;
	#endif
#endif

typedef struct {
	int comms_type;
	#ifdef _WIN32
	shake_serial_port serial;
	#endif
	#ifdef SHAKE_RFCOMM_SUPPORTED
		shake_rfcomm_socket rfcomm;
	#endif
	FILE* dbg_read;
	FILE* dbg_write;
} shake_port;

enum shake_connection_types {
	SHAKE_CONN_VIRTUAL_SERIAL = 0,
	SHAKE_CONN_RFCOMM_I64,
	SHAKE_CONN_RFCOMM_STR,
	SHAKE_CONN_DEBUGFILE,
	SHAKE_CONN_S60_RFCOMM, 
};

typedef struct {
	int type;
	int com_port;
	SHAKE_INT64 btaddr;
	char btaddr_str[20];
	char readfile[256];
	char writefile[256];
	int devtype;
} shake_conn_data;

class SHAKE;

/* private data about a shake device, hidden from user */
typedef struct {
	int device_type;
	SHAKE* shake;

	shake_port port;			// structure containing port details
	shake_thread thread;		// structure containing threading objects
	BOOL rthread_done;			// indicates when the reader thread should exit
	BOOL cthread_done;
	BOOL rthread_exit;			

	BOOL waiting_for_ack;		// TRUE if currently expecting an ACK/NAK, FALSE otherwise
	BOOL waiting_for_ack_signal; 	// TRUE if app is waiting for an ack to arrive, FALSE otherwise
	//BOOL synced;				// TRUE if packet reading code is synced properly
	char serial[20];			// Serial number
	float fwrev;				// Firmware revision
	float hwrev;				// Hardware revision
	char lastrfid[SHAKE_RFID_TAG_LENGTH];			// Last RFID tag ID found
	//int module1;				// expansion module in slot 1
	//int module2;				// expansion module in slot 2
	int modules[4];				// expansion module IDs. SK6 has 2, SK7 has 4
	float bluetoothfwrev;		// Bluetooth firmware revision (SK7 only)

	BOOL checksum;				// TRUE if device has checksums enabled
	//BOOL ascii_mode;			// TRUE if device in ASCII output mode, FALSE if not
	void (SHAKE_CALLBACK *navcb)(void*, int);	// callback func for events
	long long data_recv;		// total data received
	int wait_for_acks;			// set to 1 if the driver should wait for an ACK after sending a command
	BOOL lastack;				// TRUE if last ack was positive, FALSE if negative
	int lastaddr, lastval;		// Address and value from last ack received

#ifdef _WIN32
	void (SHAKE_STDCALL_CALLBACK *navcb_STDCALL)(void*, int);	// callback func for events
#else
	void *navcb_STDCALL;
#endif

	void (SHAKE_CALLBACK *audio_cb)(void*, short*, int, short*, int);

#ifdef _WIN32
	void (SHAKE_STDCALL_CALLBACK *audio_cb_STDCALL)(void*, short*, int, short*, int);
#else
	void *audio_cb_STDCALL;
#endif
	short audiobuf[SHAKE_AUDIO_DATA_LEN];
	short playbackbuf[SHAKE_AUDIO_DATA_LEN];
	char playback_packet[5+SHAKE_AUDIO_DATA_LEN];
	int lastevent;				// last nav/cap switch event received
	FILE* log;					// output file pointer for writing logged data into
	unsigned long packets_read;	// gives number of logged packets received when playing back data from SHAKE
	BOOL peek_flag;
	char peek;
} shake_device_private;

#endif