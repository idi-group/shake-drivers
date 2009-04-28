#ifndef _SHAKE_RFCOMM_H_
#define _SHAKE_RFCOMM_H_

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
#include "shake_btdefs.h"

#ifdef SHAKE_RFCOMM_SUPPORTED

#include "shake_structs.h"

#ifdef __cplusplus
extern "C" {
#endif

shake_rfcomm_socket* shake_open_rfcomm_i64(shake_rfcomm_socket* port, long long btaddr);
shake_rfcomm_socket* shake_open_rfcomm_str(shake_rfcomm_socket* port, char* btaddr);

int read_rfcomm_bytes(shake_device_private* devpriv, char* buf, int bytes_to_read);
int write_rfcomm_bytes(shake_device_private* devpriv, char* buf, int bytes_to_write);
int write_rfcomm_bytes_delayed(shake_device_private* devpriv, char* buf, int bytes_to_write, int chunk_size, int delay_ms);

int shake_close_rfcomm(shake_rfcomm_socket* port);

#ifdef __cplusplus
}
#endif

#endif

#endif	/* _SHAKE_RFCOMM_H_ */
