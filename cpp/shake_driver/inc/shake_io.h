#ifndef _SHAKE_IO_H_
#define _SHAKE_IO_H_

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
#include "shake_structs.h"

/*	The driver supports several possible methods of creating a connection. 
*	It can be given a virtual serial port number (currently Windows only), a Bluetooth address
*	in string or 64-bit integer formats (opens an RFCOMM socket), or a filename. In the latter case data read
*	from the file is treated exactly the same way as data coming from a Bluetooth connection
*	making it very useful for debugging.
*
*	This requires different methods of opening the connection, so this function
*	acts as a standard "read" interface for the rest of the code */
size_t read_bytes(shake_device_private* dev, char* buf, size_t bytes_to_read);

/*	The driver supports several possible methods of creating a connection. 
*	It can be given a virtual serial port number (currently Windows only), a Bluetooth address
*	in string or 64-bit integer formats (opens an RFCOMM socket), or a filename. In the latter case data read
*	from the file is treated exactly the same way as data coming from a Bluetooth connection
*	making it very useful for debugging.
*
*	This requires different methods of opening the connection, so this function
*	acts as a standard "write" interface for the rest of the code */
size_t write_bytes(shake_device_private* dev, char* buf, size_t bytes_to_write);

/* the SHAKE often has problems dealing with large incoming data packets. this typically occurs with audio
*	or vibration uploads which can total 1063 bytes per packet. To avoid overflowing the receive buffer on the
*	SHAKE, this function is used to upload packets in chunks of <chunk_size> bytes, with a gap of <delay_ms>
*	between successive chunks */
size_t write_bytes_delayed(shake_device_private* dev, char* buf, size_t bytes_to_write, size_t chunk_size, int delay_ms);

size_t read_debug_bytes(shake_device_private* devpriv, char* buf, size_t bytes_to_read);

#endif /* _SHAKE_IO_H_ */

