#ifndef _SK7_PARSING_H_
#define _SK7_PARSING_H_

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


#include "shake_io.h"
#include "shake_driver.h"

#define SK7_NUM_INFO_LINES	10
#define SK7_LINE_MAX 200

#define SK7_ASCII_READ_OK			1
#define SK7_ASCII_READ_ERROR		-1
#define SK7_ASCII_READ_CONTINUE	2

#define SK7_RAW_READ_OK			1
#define SK7_RAW_READ_ERROR		-1
#define SK7_RAW_READ_CONTINUE		2

enum sk7_info {
	SK7_HEADER,
	SK7_COPYRIGHT,
	SK7_HARDWARE_REV,
	SK7_FIRMWARE_REV,
	SK7_SERIAL_NUMBER,
	SK7_SLOT0,
	SK7_SLOT1,
	SK7_SLOT2,
	SK7_SLOT3,
	SK7_BLUETOOTH_FIRMWARE,
};

static char* SK7_modules[] = { 
	"No option module",
};

#endif