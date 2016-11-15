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

#ifndef _SHAKE_SERIAL_WIN32_
#define _SHAKE_SERIAL_WIN32_

#include "shake_structs.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _WIN32
/* serial port parameters, only important on Windows which uses
*	a Bluetooth COM port. */
enum shake_serial {
	SHAKE_SERIAL_BAUDRATE = 0,
	SHAKE_SERIAL_PARITY,
	SHAKE_SERIAL_STOP_BITS,
	SHAKE_SERIAL_START_BITS,
	SHAKE_SERIAL_HWFLOW,
	SHAKE_SERIAL_SWFLOW,
};

/*	SHAKE serial port parameters. To get a particular parameter, just do
*		int baudrate = shake_serial_params[SHAKE_SERIAL_BAUDRATE]
*	etc.. */
static unsigned sk6_serial_params[] = {
	115200,		// baud rate
	NOPARITY,	// no parity
	ONESTOPBIT, // 1 stop bit
	1,			// 1 start bit
	1,			// hardware flow control
	0,			// no software flow control
};

static unsigned sk7_serial_params[] = {
	460800,		// baud rate
	NOPARITY,	// no parity
	ONESTOPBIT, // 1 stop bit
	1,			// 1 start bit
	1,			// hardware flow control
	0,			// no software flow control
};
#endif

/*	++ Windows only ++
*	Opens the indicated serial port for use with a SHAKE device.
*	<port> is a pointer to a shake_serial_port_win32 structure that will store all the
*			information about the port.
*	<number> is the number of the COM port to open. -1 to autodetect.
*	<device type> is either 0 for SK6 or 1 for SK7
*	Returns the <port> pointer on success, NULL on failure. */
shake_serial_port_win32* shake_open_serial_win32(shake_serial_port_win32* port, int number, int device_type);

size_t read_serial_bytes_win32(shake_device_private* devpriv, char* buf, size_t bytes_to_read);

size_t write_serial_bytes_win32(shake_device_private* devpriv, char* buf, size_t bytes_to_write);

size_t write_serial_bytes_delayed_win32(shake_device_private* devpriv, char* buf, size_t bytes_to_write, size_t chunk_size, int delay_ms);

/*	Closes the indicated serial port
*	<port is a pointer to a populated shake_serial_port_win32 structure.
*	Always returns 1. */
int shake_close_serial_win32(shake_serial_port_win32* port);

#ifdef __cplusplus
}
#endif

#endif /* _SHAKE_SERIAL_WIN32_ */
