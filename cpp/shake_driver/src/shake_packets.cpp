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
#include <ctype.h>

// Given a number represented in ASCII of the form sdddd, where s is an optional
// sign character (+/-) and each d is a decimal digit 0-9, returns the numeric equivalent.
// Can also handle dddd, ddd, dd, sdd, etc
int dec_ascii_to_int(char* ascii_buf, int buflen, int digits) {
	int decval = 0, i, mult = 1;
	for(i=buflen-1;i>=buflen-digits;i--) {
		decval += (ascii_buf[i] - '0') * mult;
		mult *= 10;
	}
	if(digits == buflen - 1 && ascii_buf[0] == '-')
		decval *= -1;
	return decval;
}

// as above but for hex. Doesn't handle 0x prefixes or h suffix, and relies on
// any letters being upper case.
int hex_ascii_to_int(char* ascii_buf, int buflen, int digits) {
	int hexval = 0, i, mult = 1;
	for(i=buflen-1;i>=buflen-digits;i--) {
		if(isdigit(ascii_buf[i]))
			hexval += (ascii_buf[i] - '0') * mult;
		else
			hexval += ((ascii_buf[i] - 'A') + 10) * mult;
		mult *= 16;
	}
	if(digits == buflen - 1 && ascii_buf[0] == '-')
		hexval *= -1;
	return hexval;
}
