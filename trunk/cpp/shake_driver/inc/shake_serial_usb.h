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

#ifndef _SHAKE_SERIAL_USB_
#define _SHAKE_SERIAL_USB_

#include "shake_structs.h"

#ifdef __cplusplus
extern "C" {
#endif

/*	Opens the indicated USB serial port device for use with a SHAKE.
*	<port> is a pointer to a shake_serial_port_usb structure that will store all the
*			information about the port.
*	<usb_dev> is the name of the /dev node representing the USB serial port
*	<device type> is either 0 for SK6 or 1 for SK7
*	Returns the <port> pointer on success, NULL on failure. */
shake_serial_port_usb* shake_open_serial_usb(shake_serial_port_usb* port, char* usb_dev, int device_type);

int read_serial_bytes_usb(shake_device_private* devpriv, char* buf, int bytes_to_read);

int write_serial_bytes_usb(shake_device_private* devpriv, char* buf, int bytes_to_write);

int write_serial_bytes_delayed_usb(shake_device_private* devpriv, char* buf, int bytes_to_write, int chunk_size, int delay_ms);

/*	Closes the indicated serial port
*	<port is a pointer to a populated shake_serial_port_usb structure.
*	Always returns 1. */
int shake_close_serial_usb(shake_serial_port_usb* port);

#ifdef __cplusplus
}
#endif

#endif /* _SHAKE_SERIAL_ */
