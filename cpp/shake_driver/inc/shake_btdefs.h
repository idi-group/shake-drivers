#ifndef _SHAKE_BTDEFS_H_
#define _SHAKE_BTDEFS_H_

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


#undef SHAKE_RFCOMM_SUPPORTED

/* This is all meant to decide if RFCOMM is supported on current OS.
*		Assumes: supported on XP and recent Windows CE and on Linux */

#ifdef _WIN32
#include <winsock2.h>
#endif

// if on recent WinCE, include proper headers and define macro
#ifdef _WIN32_WCE
	#if _WIN32_WCE >= 0x420
		#define SOCKADDR_BTH SOCKADDR_BT
		#define SHAKE_RFCOMM_SUPPORTED 1
		#include <ws2bth.h>
		#include <bthapi.h>
		#include <bthutil.h>
	#endif
#endif

// if on Win32 and not CE, include and define macro
#ifdef _WIN32
	#ifndef _WIN32_WCE
		#define SHAKE_RFCOMM_SUPPORTED 1
		#include "initguid.h"
		#include "ws2bth.h"
	#endif
#endif

// if not on Windows and not on OSX, define macro
#ifndef _WIN32
	#ifndef __APPLE__
		#define SHAKE_RFCOMM_SUPPORTED 1
	#endif
#endif

#endif
