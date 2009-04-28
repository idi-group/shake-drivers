#ifndef _SHAKE_PARSING_H_
#define _SHAKE_PARSING_H_

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
#include "shake_structs.h"

static char* unknown_module = "unknown/invalid module";

/*	Compresses 16-bit signed audio in the <samples> array to 8-bit mu-Law format ready
*	for transmission to the SHAKE, storing the result in the <output> array. <count> gives
*	the number of samples (not bytes) in the <samples> array. */
void shake_mulaw_compress(short* samples, int count, unsigned char* output);

void shake_mulaw_lookup(short* dest, unsigned char* data, int len);

/*	Compresses 16-bit audio to the 8-bit format required by the SHAKE, and sends the data */
void shake_compress_and_send_audio(shake_device_private* dev);

/*	Reads a line of the information that a SHAKE outputs when first connected.
*	devpriv: the device to read the info from
*	buf: receive buffer
*	maxlen: length of buffer */
int shake_read_info_line(shake_device_private* devpriv, char* buf, int maxlen);

#endif