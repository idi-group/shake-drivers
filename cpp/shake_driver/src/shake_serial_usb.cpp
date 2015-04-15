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

#include "shake_serial_usb.h"

#ifndef _WIN32
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#endif

#include "shake_platform.h"
#include "shake_driver.h"

shake_serial_port_usb* shake_open_serial_usb(shake_serial_port_usb* port, char* usb_dev, int device_type) {
#ifndef _WIN32
	struct termios newopt;	
	struct termios oldopt;

	port->port = open(usb_dev, O_RDWR | O_NOCTTY | O_NDELAY);
	if(port->port == -1) {
		return NULL;
	}

	tcgetattr(port->port, &oldopt);
	bzero(&newopt, sizeof(newopt));
	
	int baudrate = (device_type == SHAKE_SK7) ? 460800 : 115200;

#ifndef __APPLE__
	newopt.c_cflag = baudrate | CRTSCTS | CS8 | CLOCAL;
	newopt.c_iflag = 0; //IGNPAR | IGNCR | ICRNL | IGNBRK;
	newopt.c_oflag = 0;
	newopt.c_lflag = 0;

	tcflush(port->port, TCIFLUSH);
	tcsetattr(port->port, TCSANOW, &newopt);
#else
	fcntl(port->port, F_SETFL, 0);
	tcgetattr(port->port, &oldopt);
	cfsetispeed(&oldopt, baudrate);
   	cfsetospeed(&oldopt, baudrate);
  	oldopt.c_cflag |= (CLOCAL | CREAD);
   	oldopt.c_cflag &= ~CSIZE; // Mask the character size bits 
   	oldopt.c_cflag |= CS8;    // Select 8 data bits 
   	oldopt.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
   	tcsetattr(port->port, TCSANOW, &oldopt);
#endif // __APPLE__
	return port;
#else
	return NULL;
#endif
}

int shake_close_serial_usb(shake_serial_port_usb* port) {
#ifndef _WIN32
	return close(port->port);
#else
	return 0;
#endif
}

/*	utility func, reads <bytes_to_read> bytes from the port associated with <devpriv> 
*	into <buf>, handling any timeouts that occur during this operation.
*	Returns number of bytes read. */
int read_serial_bytes_usb(shake_device_private* devpriv, char* buf, int bytes_to_read) {
#ifndef _WIN32 
	DWORD bytes_read;
	int sleepcounter = 0;
	int attempts = 0;
	int remaining_bytes = bytes_to_read;

	/* read the port in a loop to deal with timeouts */
	while(1) {
		//int res = ReadFile(devpriv->port.serial_usb.port, buf + (bytes_to_read - remaining_bytes), remaining_bytes, &bytes_read, NULL);
		bytes_read = read(devpriv->port.serial_usb.port, buf + (bytes_to_read - remaining_bytes), remaining_bytes);
		if(bytes_read == -1)
			return remaining_bytes;

		/* subtract the bytes we just read from the total amount we want */
		remaining_bytes -= bytes_read;

		/* if we didn't get them all.. */
		if(remaining_bytes != 0) {
			/* check if the thread is exiting, and if so break out of the loop and return */
			if(devpriv->rthread_done)
				break;

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
#else
	return 0;
#endif
}

/*	utility func, writes <bytes_to_write> bytes to the port associated with <devpriv>
*	from <buf>, handling any timeouts that occur during this operation.
*	Returns number of bytes written. */
int write_serial_bytes_usb(shake_device_private* devpriv, char* buf, int bytes_to_write) {
#ifndef _WIN32
	DWORD bytes_written;
	int remaining_bytes = bytes_to_write;

	/* write the port in a loop to deal with timeouts */
	while(1) {
		//if(!WriteFile(devpriv->port.serial_usb.port, buf + (bytes_to_write - remaining_bytes), remaining_bytes, &bytes_written, NULL))
		//	return bytes_to_write - remaining_bytes;
		bytes_written = write(devpriv->port.serial_usb.port, buf + (bytes_to_write - remaining_bytes), remaining_bytes);
		if(bytes_written == -1)
			return remaining_bytes;

		/* subtract the bytes we just wrote from the total amount we want */
		remaining_bytes -= bytes_written;

		/* if we didn't get them all.. */
		if(remaining_bytes != 0) {
			/* loop back round and write the port again */
			continue;
		} else
			/* if all bytes have been written, break out of loop and return */
			break;
	}
	return bytes_to_write - remaining_bytes;
#else
	return 0;
#endif
}

int write_serial_bytes_delayed_usb(shake_device_private* devpriv, char* buf, int bytes_to_write, int chunk_size, int delay_ms) {
#ifndef _WIN32
	DWORD bytes_written;
	int remaining_bytes = bytes_to_write;

	/* write the port in a loop to deal with timeouts */
	while(1) {
 		int current_remaining_bytes = remaining_bytes;// ((chunk_size >= remaining_bytes) ? chunk_size : remaining_bytes);
		if(remaining_bytes > chunk_size)
			current_remaining_bytes = chunk_size;
		//if(!WriteFile(devpriv->port.serial_usb.port, buf + (bytes_to_write - remaining_bytes), current_remaining_bytes, &bytes_written, NULL))
		//	return bytes_to_write - remaining_bytes;
		bytes_written = write(devpriv->port.serial_usb.port, buf + (bytes_to_write - remaining_bytes), remaining_bytes);
		if(bytes_written == -1)
			return remaining_bytes;

		/* subtract the bytes we just wrote from the total amount we want */
		remaining_bytes -= bytes_written;

		/* delay */
		shake_sleep(delay_ms);

		/* if we didn't get them all.. */
		if(remaining_bytes != 0) {
			/* loop back round and write the port again */
			continue;
		} else
			/* if all bytes have been written, break out of loop and return */
			break;
	}
	return bytes_to_write - remaining_bytes;
#else
	return 0;
#endif
}
