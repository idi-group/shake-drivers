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


#include "shake_packets.h"
#include "shake_io.h"
#include "shake_serial_win32.h"
#include "shake_serial_usb.h"
#include "shake_rfcomm.h"
#ifdef SHAKE_S60
#include "shake_s60_rfcomm.h"
#endif

int read_debug_bytes(shake_device_private* devpriv, char* buf, int bytes_to_read) {
	DWORD bytes_read;
	int sleepcounter = 0;
	int attempts = 0;
	int remaining_bytes = bytes_to_read;

	shake_sleep(10);

	while(1) {
		SHAKE_DBG("Reading %d bytes from position %ld\n", remaining_bytes, ftell(devpriv->port.dbg_read));
		if(devpriv->rthread_done) {
			shake_thread_exit(102);
		}
		bytes_read = fread(buf + (bytes_to_read - remaining_bytes), 1, remaining_bytes, devpriv->port.dbg_read);

		/* subtract the bytes we just read from the total amount we want */
		remaining_bytes -= bytes_read;

		/* if we didn't get them all.. */
		if(remaining_bytes != 0) {
			/* check if the thread is exiting, and if so break out of the loop and return */
			if(devpriv->rthread_done) {
				shake_thread_exit(103);	
			}

			if(feof(devpriv->port.dbg_read)) {
				// seek back to start
				fseek(devpriv->port.dbg_read, 0, SEEK_SET);
				SHAKE_DBG("******************************** \n\n RETURNING TO START OF INPUT FILE \n ********************************* \n\n\n");
			}

			/* otherwise just loop back round and read the port again */
			++sleepcounter;
			if(sleepcounter > 15) {
				shake_sleep(1);
				attempts++;
				sleepcounter = 0;

				if(attempts > 30)
					break;
			}
			continue;
		} else
			/* if all bytes have been read, break out of loop and return */
			break;
	}
	devpriv->data_recv += (bytes_to_read - remaining_bytes);
	return bytes_to_read - remaining_bytes;
}

/*	The driver supports several possible methods of creating a connection. 
*	It can be given a virtual serial port number (currently Windows only), a Bluetooth address
*	in string or 64-bit integer formats (opens an RFCOMM socket), or a filename. In the latter case data read
*	from the file is treated exactly the same way as data coming from a Bluetooth connection
*	making it very useful for debugging.
*
*	This requires different methods of opening the connection, so this function
*	acts as a standard "read" interface for the rest of the code */
int read_bytes(shake_device_private* dev, char* buf, int bytes_to_read) {
	if(dev == NULL)
		return 0;

	if(dev->rthread_done)
		shake_thread_exit(101);

	int returned_bytes = 0;
		
	// if we've "peeked" a byte, put it back before reading any more real bytes
	if(dev->peek_flag) {
		buf[0] = dev->peek;
		bytes_to_read--;
		dev->peek_flag	= FALSE;
		SHAKE_DBG("Peek flag reset, added byte %d/%02X\n", dev->peek, dev->peek);
		returned_bytes = 1;

		// can maybe just return here
		if(bytes_to_read == 0) {
			return 1;
		}

		// increase buf ptr
		buf++;
	}

	switch(dev->port.comms_type) {
		/* virtual serial port */
		#ifdef _WIN32
		case SHAKE_CONN_VIRTUAL_SERIAL_WIN32: {
			returned_bytes += read_serial_bytes_win32(dev, buf, bytes_to_read);
			return returned_bytes;
		}
		#endif
		/* RFCOMM socket */
		#ifdef SHAKE_RFCOMM_SUPPORTED
		case SHAKE_CONN_RFCOMM_I64:
		case SHAKE_CONN_RFCOMM_STR: {
			returned_bytes += read_rfcomm_bytes(dev, buf, bytes_to_read);
			return returned_bytes;
		}
		#endif
		/* File */
		case SHAKE_CONN_DEBUGFILE: {
			returned_bytes += read_debug_bytes(dev, buf, bytes_to_read);
			return returned_bytes;
		}	
		/* S60 Bluetooth */
		#ifdef SHAKE_S60
		case SHAKE_CONN_S60_RFCOMM: {
			returned_bytes += read_s60_rfcomm_bytes(dev, buf, bytes_to_read);
			return returned_bytes;
		}
		#endif
		#ifndef _WIN32
		case SHAKE_CONN_USB_SERIAL: {
			returned_bytes += read_serial_bytes_usb(dev, buf, bytes_to_read);
			return returned_bytes;
		}
		#endif
		default:
			break;
	}
	return 0;
}

int write_bytes(shake_device_private* dev, char* buf, int bytes_to_write) {
	switch(dev->port.comms_type) {
		#ifdef _WIN32
		case SHAKE_CONN_VIRTUAL_SERIAL_WIN32:
			return write_serial_bytes_win32(dev, buf, bytes_to_write);
		#endif
		#ifdef SHAKE_RFCOMM_SUPPORTED
		case SHAKE_CONN_RFCOMM_I64:
		case SHAKE_CONN_RFCOMM_STR:
			return write_rfcomm_bytes(dev, buf, bytes_to_write);
		#endif
		case SHAKE_CONN_DEBUGFILE:
			return fwrite(buf, 1, bytes_to_write, dev->port.dbg_write);
		#ifdef SHAKE_S60
		case SHAKE_CONN_S60_RFCOMM: 
			return write_s60_rfcomm_bytes(dev, buf, bytes_to_write);
		#endif	
		#ifndef _WIN32
		case SHAKE_CONN_USB_SERIAL:
			return write_serial_bytes_usb(dev, buf, bytes_to_write);
		#endif
		default:
			break;
	}
	return 0;
}

/* the SHAKE often has problems dealing with large incoming data packets. this typically occurs with audio
*	or vibration uploads which can total 1063 bytes per packet. To avoid overflowing the receive buffer on the
*	SHAKE, this function is used to upload packets in chunks of <chunk_size> bytes, with a gap of <delay_ms>
*	between successive chunks */
int write_bytes_delayed(shake_device_private* dev, char* buf, int bytes_to_write, int chunk_size, int delay_ms) {
	switch(dev->port.comms_type) {
		#ifdef _WIN32
		case SHAKE_CONN_VIRTUAL_SERIAL_WIN32:
			return write_serial_bytes_delayed_win32(dev, buf, bytes_to_write, chunk_size, delay_ms);
		#endif
		#ifdef SHAKE_RFCOMM_SUPPORTED
		case SHAKE_CONN_RFCOMM_I64:
		case SHAKE_CONN_RFCOMM_STR:
			return write_rfcomm_bytes_delayed(dev, buf, bytes_to_write, chunk_size, delay_ms);
		#endif
		// no need to delay here
		case SHAKE_CONN_DEBUGFILE:
			return fwrite(buf, 1, bytes_to_write, dev->port.dbg_write);
		#ifdef SHAKE_S60
		case SHAKE_CONN_S60_RFCOMM:
			return write_s60_rfcomm_bytes_delayed(dev, buf, bytes_to_write, chunk_size, delay_ms);
		#endif
		#ifndef _WIN32
		case SHAKE_CONN_USB_SERIAL:
			return write_serial_bytes_usb(dev, buf, bytes_to_write);
		#endif
		default:
			break;
	}
	return 0;
}
